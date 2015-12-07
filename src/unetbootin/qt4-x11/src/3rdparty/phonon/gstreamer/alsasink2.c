/* GStreamer
 * Copyright (C) 2001 CodeFactory AB
 * Copyright (C) 2001 Thomas Nyberg <thomas@codefactory.se>
 * Copyright (C) 2001-2002 Andy Wingo <apwingo@eos.ncsu.edu>
 * Copyright (C) 2003 Benjamin Otte <in7y118@public.uni-hamburg.de>
 * Copyright (C) 2005 Wim Taymans <wim@fluendo.com>
 * Copyright (C) 2005, 2006 Tim-Philipp Müller <tim centricular net>
 * Copyright (C) 2008 Matthias Kretz <kretz@kde.org>
 *
 * gstalsasink2.c:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public 
 *  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * SECTION:element-alsasink2
 * @short_description: play audio to an ALSA device
 * @see_also: alsasrc, alsamixer
 *
 * <refsect2>
 * <para>
 * This element renders raw audio samples using the ALSA api.
 * </para>
 * <title>Example pipelines</title>
 * <para>
 * Play an Ogg/Vorbis file.
 * </para>
 * <programlisting>
 * gst-launch -v filesrc location=sine.ogg ! oggdemux ! vorbisdec ! audioconvert ! audioresample ! alsasink2
 * </programlisting>
 * </refsect2>
 *
 * Last reviewed on 2006-03-01 (0.10.4)
 */

#define _XOPEN_SOURCE 600

#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <alsa/asoundlib.h>

#include "alsasink2.h"

#include <gst/interfaces/propertyprobe.h>
#include <gst/audio/multichannel.h>

#define _(text) (text)

#define GST_CHECK_ALSA_VERSION(major,minor,micro) \
    (SND_LIB_MAJOR > (major) || \
     (SND_LIB_MAJOR == (major) && SND_LIB_MINOR > (minor)) || \
     (SND_LIB_MAJOR == (major) && SND_LIB_MINOR == (minor) && \
      SND_LIB_SUBMINOR >= (micro)))

static const GList *
gst_alsa_device_property_probe_get_properties (GstPropertyProbe * probe)
{
  GObjectClass *klass = G_OBJECT_GET_CLASS (probe);
  static GList *list = NULL;

  /* well, not perfect, but better than no locking at all.
   * In the worst case we leak a list node, so who cares? */
  GST_CLASS_LOCK (GST_OBJECT_CLASS (klass));

  if (!list) {
    GParamSpec *pspec;

    pspec = g_object_class_find_property (klass, "device");
    list = g_list_append (NULL, pspec);
  }

  GST_CLASS_UNLOCK (GST_OBJECT_CLASS (klass));

  return list;
}

static GList *
gst_alsa_get_device_list (snd_pcm_stream_t stream)
{
  snd_ctl_t *handle;
  int card, err, dev;
  snd_ctl_card_info_t *info;
  snd_pcm_info_t *pcminfo;
  gboolean mixer = (stream == ~0u);
  GList *list = NULL;

  if (stream == ~0u)
    stream = 0;

  snd_ctl_card_info_malloc (&info);
  snd_pcm_info_malloc (&pcminfo);
  card = -1;

  if (snd_card_next (&card) < 0 || card < 0) {
    /* no soundcard found */
    return NULL;
  }

  while (card >= 0) {
    gchar name[32];

    g_snprintf (name, sizeof (name), "hw:%d", card);
    if ((err = snd_ctl_open (&handle, name, 0)) < 0) {
      goto next_card;
    }
    if ((err = snd_ctl_card_info (handle, info)) < 0) {
      snd_ctl_close (handle);
      goto next_card;
    }

    if (mixer) {
      list = g_list_append (list, g_strdup (name));
    } else {
      g_snprintf (name, sizeof (name), "default:CARD=%d", card);
      list = g_list_append (list, g_strdup (name));
      dev = -1;
      while (1) {
        gchar *gst_device;

        snd_ctl_pcm_next_device (handle, &dev);

        if (dev < 0)
          break;
        snd_pcm_info_set_device (pcminfo, dev);
        snd_pcm_info_set_subdevice (pcminfo, 0);
        snd_pcm_info_set_stream (pcminfo, stream);
        if ((err = snd_ctl_pcm_info (handle, pcminfo)) < 0) {
          continue;
        }

        gst_device = g_strdup_printf ("hw:%d,%d", card, dev);
        list = g_list_append (list, gst_device);
      }
    }
    snd_ctl_close (handle);
  next_card:
    if (snd_card_next (&card) < 0) {
      break;
    }
  }

  snd_ctl_card_info_free (info);
  snd_pcm_info_free (pcminfo);

  return list;
}

static void
gst_alsa_device_property_probe_probe_property (GstPropertyProbe * probe,
    guint prop_id, const GParamSpec * pspec)
{
  if (!g_str_equal (pspec->name, "device")) {
    G_OBJECT_WARN_INVALID_PROPERTY_ID (probe, prop_id, pspec);
  }
}

static gboolean
gst_alsa_device_property_probe_needs_probe (GstPropertyProbe * probe,
    guint prop_id, const GParamSpec * pspec)
{
  /* don't cache probed data */
  return TRUE;
}

static GValueArray *
gst_alsa_device_property_probe_get_values (GstPropertyProbe * probe,
    guint prop_id, const GParamSpec * pspec)
{
  GstElementClass *klass;
  const GList *templates;
  snd_pcm_stream_t mode = -1;
  GValueArray *array;
  GValue value = { 0, };
  GList *l, *list;

  if (!g_str_equal (pspec->name, "device")) {
    G_OBJECT_WARN_INVALID_PROPERTY_ID (probe, prop_id, pspec);
    return NULL;
  }

  klass = GST_ELEMENT_GET_CLASS (GST_ELEMENT (probe));

  /* I'm pretty sure ALSA has a good way to do this. However, their cool
   * auto-generated documentation is pretty much useless if you try to
   * do function-wise look-ups. */
  /* we assume one pad template at max [zero=mixer] */
  templates = gst_element_class_get_pad_template_list (klass);
  if (templates) {
    if (GST_PAD_TEMPLATE_DIRECTION (templates->data) == GST_PAD_SRC)
      mode = SND_PCM_STREAM_CAPTURE;
    else
      mode = SND_PCM_STREAM_PLAYBACK;
  }

  list = gst_alsa_get_device_list (mode);

  if (list == NULL) {
    GST_LOG_OBJECT (probe, "No devices found");
    return NULL;
  }

  array = g_value_array_new (g_list_length (list));
  g_value_init (&value, G_TYPE_STRING);
  for (l = list; l != NULL; l = l->next) {
    GST_LOG_OBJECT (probe, "Found device: %s", (gchar *) l->data);
    g_value_take_string (&value, (gchar *) l->data);
    l->data = NULL;
    g_value_array_append (array, &value);
  }
  g_value_unset (&value);
  g_list_free (list);

  return array;
}

static void
gst_alsa_property_probe_interface_init (GstPropertyProbeInterface * iface)
{
  iface->get_properties = gst_alsa_device_property_probe_get_properties;
  iface->probe_property = gst_alsa_device_property_probe_probe_property;
  iface->needs_probe = gst_alsa_device_property_probe_needs_probe;
  iface->get_values = gst_alsa_device_property_probe_get_values;
}

static void
gst_alsa_type_add_device_property_probe_interface (GType type)
{
  static const GInterfaceInfo probe_iface_info = {
    (GInterfaceInitFunc) gst_alsa_property_probe_interface_init,
    NULL,
    NULL,
  };

  g_type_add_interface_static (type, GST_TYPE_PROPERTY_PROBE,
      &probe_iface_info);
}

static GstCaps *
gst_alsa_detect_rates (GstObject * obj, snd_pcm_hw_params_t * hw_params,
    GstCaps * in_caps)
{
  GstCaps *caps;
  guint min, max;
  gint err, dir, min_rate, max_rate;
  guint i;

  GST_LOG_OBJECT (obj, "probing sample rates ...");

  if ((err = snd_pcm_hw_params_get_rate_min (hw_params, &min, &dir)) < 0)
    goto min_rate_err;

  if ((err = snd_pcm_hw_params_get_rate_max (hw_params, &max, &dir)) < 0)
    goto max_rate_err;

  min_rate = min;
  max_rate = max;

  if (min_rate < 4000)
    min_rate = 4000;            /* random 'sensible minimum' */

  if (max_rate <= 0)
    max_rate = G_MAXINT;        /* or maybe just use 192400 or so? */
  else if (max_rate > 0 && max_rate < 4000)
    max_rate = MAX (4000, min_rate);

  GST_DEBUG_OBJECT (obj, "Min. rate = %u (%d)", min_rate, min);
  GST_DEBUG_OBJECT (obj, "Max. rate = %u (%d)", max_rate, max);

  caps = gst_caps_make_writable (in_caps);

  for (i = 0; i < gst_caps_get_size (caps); ++i) {
    GstStructure *s;

    s = gst_caps_get_structure (caps, i);
    if (min_rate == max_rate) {
      gst_structure_set (s, "rate", G_TYPE_INT, min_rate, NULL);
    } else {
      gst_structure_set (s, "rate", GST_TYPE_INT_RANGE,
          min_rate, max_rate, NULL);
    }
  }

  return caps;

  /* ERRORS */
min_rate_err:
  {
    GST_ERROR_OBJECT (obj, "failed to query minimum sample rate: %s",
        snd_strerror (err));
    gst_caps_unref (in_caps);
    return NULL;
  }
max_rate_err:
  {
    GST_ERROR_OBJECT (obj, "failed to query maximum sample rate: %s",
        snd_strerror (err));
    gst_caps_unref (in_caps);
    return NULL;
  }
}

static const struct
{
  const int width;
  const int depth;
  const int sformat;
  const int uformat;
} pcmformats[] = {
  {
  8, 8, SND_PCM_FORMAT_S8, SND_PCM_FORMAT_U8}, {
  16, 16, SND_PCM_FORMAT_S16, SND_PCM_FORMAT_U16}, {
  32, 24, SND_PCM_FORMAT_S24, SND_PCM_FORMAT_U24}, {
#if (G_BYTE_ORDER == G_LITTLE_ENDIAN)   /* no endian-unspecific enum available */
  24, 24, SND_PCM_FORMAT_S24_3LE, SND_PCM_FORMAT_U24_3LE}, {
#else
  24, 24, SND_PCM_FORMAT_S24_3BE, SND_PCM_FORMAT_U24_3BE}, {
#endif
  32, 32, SND_PCM_FORMAT_S32, SND_PCM_FORMAT_U32}
};

static GstCaps *
gst_alsa_detect_formats (GstObject * obj, snd_pcm_hw_params_t * hw_params,
    GstCaps * in_caps)
{
  snd_pcm_format_mask_t *mask;
  GstStructure *s;
  GstCaps *caps;
  guint i;

  snd_pcm_format_mask_malloc (&mask);
  snd_pcm_hw_params_get_format_mask (hw_params, mask);

  caps = gst_caps_new_empty ();

  for (i = 0; i < gst_caps_get_size (in_caps); ++i) {
    GstStructure *scopy;
    guint w;
    gint width = 0, depth = 0;

    s = gst_caps_get_structure (in_caps, i);
    if (!gst_structure_has_name (s, "audio/x-raw-int")) {
      GST_WARNING_OBJECT (obj, "skipping non-int format");
      continue;
    }
    if (!gst_structure_get_int (s, "width", &width) ||
        !gst_structure_get_int (s, "depth", &depth))
      continue;
    if (width == 0 || (width % 8) != 0)
      continue;                 /* Only full byte widths are valid */
    for (w = 0; w < G_N_ELEMENTS (pcmformats); w++)
      if (pcmformats[w].width == width && pcmformats[w].depth == depth)
        break;
    if (w == G_N_ELEMENTS (pcmformats))
      continue;                 /* Unknown format */

    if (snd_pcm_format_mask_test (mask, pcmformats[w].sformat) &&
        snd_pcm_format_mask_test (mask, pcmformats[w].uformat)) {
      /* template contains { true, false } or just one, leave it as it is */
      scopy = gst_structure_copy (s);
    } else if (snd_pcm_format_mask_test (mask, pcmformats[w].sformat)) {
      scopy = gst_structure_copy (s);
      gst_structure_set (scopy, "signed", G_TYPE_BOOLEAN, TRUE, NULL);
    } else if (snd_pcm_format_mask_test (mask, pcmformats[w].uformat)) {
      scopy = gst_structure_copy (s);
      gst_structure_set (scopy, "signed", G_TYPE_BOOLEAN, FALSE, NULL);
    } else {
      scopy = NULL;
    }
    if (scopy) {
      if (width > 8) {
        /* TODO: proper endianness detection, for now it's CPU endianness only */
        gst_structure_set (scopy, "endianness", G_TYPE_INT, G_BYTE_ORDER, NULL);
      }
      gst_caps_append_structure (caps, scopy);
    }
  }

  snd_pcm_format_mask_free (mask);
  gst_caps_unref (in_caps);
  return caps;
}

/* we don't have channel mappings for more than this many channels */
#define GST_ALSA_MAX_CHANNELS 8

static GstStructure *
get_channel_free_structure (const GstStructure * in_structure)
{
  GstStructure *s = gst_structure_copy (in_structure);

  gst_structure_remove_field (s, "channels");
  return s;
}

static void
caps_add_channel_configuration (GstCaps * caps,
    const GstStructure * in_structure, gint min_chans, gint max_chans)
{
  GstAudioChannelPosition pos[8] = {
    GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT,
    GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT,
    GST_AUDIO_CHANNEL_POSITION_REAR_LEFT,
    GST_AUDIO_CHANNEL_POSITION_REAR_RIGHT,
    GST_AUDIO_CHANNEL_POSITION_FRONT_CENTER,
    GST_AUDIO_CHANNEL_POSITION_LFE,
    GST_AUDIO_CHANNEL_POSITION_SIDE_LEFT,
    GST_AUDIO_CHANNEL_POSITION_SIDE_RIGHT
  };
  GstStructure *s = NULL;
  gint c;

  if (min_chans == max_chans && max_chans <= 2) {
    s = get_channel_free_structure (in_structure);
    gst_structure_set (s, "channels", G_TYPE_INT, max_chans, NULL);
    gst_caps_append_structure (caps, s);
    return;
  }

  g_assert (min_chans >= 1);

  /* mono and stereo don't need channel configurations */
  if (min_chans == 2) {
    s = get_channel_free_structure (in_structure);
    gst_structure_set (s, "channels", G_TYPE_INT, 2, NULL);
    gst_caps_append_structure (caps, s);
  } else if (min_chans == 1 && max_chans >= 2) {
    s = get_channel_free_structure (in_structure);
    gst_structure_set (s, "channels", GST_TYPE_INT_RANGE, 1, 2, NULL);
    gst_caps_append_structure (caps, s);
  }

  /* don't know whether to use 2.1 or 3.0 here - but I suspect
   * alsa might work around that/fix it somehow. Can we tell alsa
   * what our channel layout is like? */
  if (max_chans >= 3 && min_chans <= 3) {
    GstAudioChannelPosition pos_21[3] = {
      GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT,
      GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT,
      GST_AUDIO_CHANNEL_POSITION_LFE
    };

    s = get_channel_free_structure (in_structure);
    gst_structure_set (s, "channels", G_TYPE_INT, 3, NULL);
    gst_audio_set_channel_positions (s, pos_21);
    gst_caps_append_structure (caps, s);
  }

  /* everything else (4, 6, 8 channels) needs a channel layout */
  for (c = MAX (4, min_chans); c <= 8; c += 2) {
    if (max_chans >= c) {
      s = get_channel_free_structure (in_structure);
      gst_structure_set (s, "channels", G_TYPE_INT, c, NULL);
      gst_audio_set_channel_positions (s, pos);
      gst_caps_append_structure (caps, s);
    }
  }

  for (c = MAX (9, min_chans); c <= max_chans; ++c) {
    GstAudioChannelPosition *ch_layout;
    gint i;

    ch_layout = g_new (GstAudioChannelPosition, c);
    for (i = 0; i < c; ++i) {
      ch_layout[i] = GST_AUDIO_CHANNEL_POSITION_NONE;
    }
    s = get_channel_free_structure (in_structure);
    gst_structure_set (s, "channels", G_TYPE_INT, c, NULL);
    gst_audio_set_channel_positions (s, ch_layout);
    gst_caps_append_structure (caps, s);
    g_free (ch_layout);
  }
}

static GstCaps *
gst_alsa_detect_channels (GstObject * obj, snd_pcm_hw_params_t * hw_params,
    GstCaps * in_caps)
{
  GstCaps *caps;
  guint min, max;
  gint min_chans, max_chans;
  gint err;
  guint i;

  GST_LOG_OBJECT (obj, "probing channels ...");

  if ((err = snd_pcm_hw_params_get_channels_min (hw_params, &min)) < 0)
    goto min_chan_error;

  if ((err = snd_pcm_hw_params_get_channels_max (hw_params, &max)) < 0)
    goto max_chan_error;

  /* note: the above functions may return (guint) -1 */
  min_chans = min;
  max_chans = max;

  if (min_chans < 0) {
    min_chans = 1;
    max_chans = GST_ALSA_MAX_CHANNELS;
  } else if (max_chans < 0) {
    max_chans = GST_ALSA_MAX_CHANNELS;
  }

  if (min_chans > max_chans) {
    gint temp;

    GST_WARNING_OBJECT (obj, "minimum channels > maximum channels (%d > %d), "
        "please fix your soundcard drivers", min, max);
    temp = min_chans;
    min_chans = max_chans;
    max_chans = temp;
  }

  /* pro cards seem to return large numbers for min_channels */
  if (min_chans > GST_ALSA_MAX_CHANNELS) {
    GST_DEBUG_OBJECT (obj, "min_chans = %u, looks like a pro card", min_chans);
    if (max_chans < min_chans) {
      max_chans = min_chans;
    } else {
      /* only support [max_chans; max_chans] for these cards for now
       * to avoid inflating the source caps with loads of structures ... */
      min_chans = max_chans;
    }
  } else {
    min_chans = MAX (min_chans, 1);
    max_chans = MIN (GST_ALSA_MAX_CHANNELS, max_chans);
  }

  GST_DEBUG_OBJECT (obj, "Min. channels = %d (%d)", min_chans, min);
  GST_DEBUG_OBJECT (obj, "Max. channels = %d (%d)", max_chans, max);

  caps = gst_caps_new_empty ();

  for (i = 0; i < gst_caps_get_size (in_caps); ++i) {
    GstStructure *s;
    GType field_type;
    gint c_min = min_chans;
    gint c_max = max_chans;

    s = gst_caps_get_structure (in_caps, i);
    /* the template caps might limit the number of channels (like alsasrc),
     * in which case we don't want to return a superset, so hack around this
     * for the two common cases where the channels are either a fixed number
     * or a min/max range). Example: alsasrc template has channels = [1,2] and 
     * the detection will claim to support 8 channels for device 'plughw:0' */
    field_type = gst_structure_get_field_type (s, "channels");
    if (field_type == G_TYPE_INT) {
      gst_structure_get_int (s, "channels", &c_min);
      gst_structure_get_int (s, "channels", &c_max);
    } else if (field_type == GST_TYPE_INT_RANGE) {
      const GValue *val;

      val = gst_structure_get_value (s, "channels");
      c_min = CLAMP (gst_value_get_int_range_min (val), min_chans, max_chans);
      c_max = CLAMP (gst_value_get_int_range_max (val), min_chans, max_chans);
    } else {
      c_min = min_chans;
      c_max = max_chans;
    }

    caps_add_channel_configuration (caps, s, c_min, c_max);
  }

  gst_caps_unref (in_caps);

  return caps;

  /* ERRORS */
min_chan_error:
  {
    GST_ERROR_OBJECT (obj, "failed to query minimum channel count: %s",
        snd_strerror (err));
    return NULL;
  }
max_chan_error:
  {
    GST_ERROR_OBJECT (obj, "failed to query maximum channel count: %s",
        snd_strerror (err));
    return NULL;
  }
}

#ifndef GST_CHECK_VERSION
#define GST_CHECK_VERSION(major,minor,micro) \
    (GST_VERSION_MAJOR > (major) || \
     (GST_VERSION_MAJOR == (major) && GST_VERSION_MINOR > (minor)) || \
     (GST_VERSION_MAJOR == (major) && GST_VERSION_MINOR == (minor) && GST_VERSION_MICRO >= (micro)))
#endif

#if GST_CHECK_VERSION(0, 10, 18)
snd_pcm_t *
gst_alsa_open_iec958_pcm (GstObject * obj)
{
  char *iec958_pcm_name = NULL;
  snd_pcm_t *pcm = NULL;
  int res;
  char devstr[256];             /* Storage for local 'default' device string */

  /*
   * Try and open our default iec958 device. Fall back to searching on card x
   * if this fails, which should only happen on older alsa setups
   */

  /* The string will be one of these:
   * SPDIF_CON: Non-audio flag not set:
   *    spdif:{AES0 0x0 AES1 0x82 AES2 0x0 AES3 0x2}
   * SPDIF_CON: Non-audio flag set:
   *    spdif:{AES0 0x2 AES1 0x82 AES2 0x0 AES3 0x2}
   */
  sprintf (devstr,
      "iec958:{AES0 0x%02x AES1 0x%02x AES2 0x%02x AES3 0x%02x}",
      IEC958_AES0_CON_EMPHASIS_NONE | IEC958_AES0_NONAUDIO,
      IEC958_AES1_CON_ORIGINAL | IEC958_AES1_CON_PCM_CODER,
      0, IEC958_AES3_CON_FS_48000);

  GST_DEBUG_OBJECT (obj, "Generated device string \"%s\"", devstr);
  iec958_pcm_name = devstr;

  res = snd_pcm_open (&pcm, iec958_pcm_name, SND_PCM_STREAM_PLAYBACK, 0);
  if (G_UNLIKELY (res < 0)) {
    GST_DEBUG_OBJECT (obj, "failed opening IEC958 device: %s",
        snd_strerror (res));
    pcm = NULL;
  }

  return pcm;
}
#endif


/*
 * gst_alsa_probe_supported_formats:
 *
 * Takes the template caps and returns the subset which is actually
 * supported by this device.
 *
 */

GstCaps *
gst_alsa_probe_supported_formats (GstObject * obj, snd_pcm_t * handle,
    const GstCaps * template_caps)
{
  snd_pcm_hw_params_t *hw_params;
  snd_pcm_stream_t stream_type;
  GstCaps *caps;
  gint err;

  snd_pcm_hw_params_malloc (&hw_params);
  if ((err = snd_pcm_hw_params_any (handle, hw_params)) < 0)
    goto error;

  stream_type = snd_pcm_stream (handle);

  caps = gst_caps_copy (template_caps);

  if (!(caps = gst_alsa_detect_formats (obj, hw_params, caps)))
    goto subroutine_error;

  if (!(caps = gst_alsa_detect_rates (obj, hw_params, caps)))
    goto subroutine_error;

  if (!(caps = gst_alsa_detect_channels (obj, hw_params, caps)))
    goto subroutine_error;

#if GST_CHECK_VERSION(0, 10, 18)
  /* Try opening IEC958 device to see if we can support that format (playback
   * only for now but we could add SPDIF capture later) */
  if (stream_type == SND_PCM_STREAM_PLAYBACK) {
    snd_pcm_t *pcm = gst_alsa_open_iec958_pcm (obj);

    if (G_LIKELY (pcm)) {
      gst_caps_append (caps, gst_caps_new_simple ("audio/x-iec958", NULL));
      snd_pcm_close (pcm);
    }
  }
#endif

  snd_pcm_hw_params_free (hw_params);
  return caps;

  /* ERRORS */
error:
  {
    GST_ERROR_OBJECT (obj, "failed to query formats: %s", snd_strerror (err));
    snd_pcm_hw_params_free (hw_params);
    return NULL;
  }
subroutine_error:
  {
    GST_ERROR_OBJECT (obj, "failed to query formats");
    snd_pcm_hw_params_free (hw_params);
    return NULL;
  }
}

static gchar *
gst_alsa_find_device_name_no_handle (GstObject * obj, const gchar * devcard,
    gint device_num, snd_pcm_stream_t stream)
{
  snd_ctl_card_info_t *info = NULL;
  snd_ctl_t *ctl = NULL;
  gchar *ret = NULL;
  gint dev = -1;

  GST_LOG_OBJECT (obj, "[%s] device=%d", devcard, device_num);

  if (snd_ctl_open (&ctl, devcard, 0) < 0)
    return NULL;

  snd_ctl_card_info_malloc (&info);
  if (snd_ctl_card_info (ctl, info) < 0)
    goto done;

  while (snd_ctl_pcm_next_device (ctl, &dev) == 0 && dev >= 0) {
    if (dev == device_num) {
      snd_pcm_info_t *pcminfo;

      snd_pcm_info_malloc (&pcminfo);
      snd_pcm_info_set_device (pcminfo, dev);
      snd_pcm_info_set_subdevice (pcminfo, 0);
      snd_pcm_info_set_stream (pcminfo, stream);
      if (snd_ctl_pcm_info (ctl, pcminfo) < 0) {
        snd_pcm_info_free (pcminfo);
        break;
      }

      ret = g_strdup (snd_pcm_info_get_name (pcminfo));
      snd_pcm_info_free (pcminfo);
      GST_LOG_OBJECT (obj, "name from pcminfo: %s", GST_STR_NULL (ret));
    }
  }

  if (ret == NULL) {
    char *name = NULL;
    gint card;

    GST_LOG_OBJECT (obj, "no luck so far, trying backup");
    card = snd_ctl_card_info_get_card (info);
    snd_card_get_name (card, &name);
    ret = g_strdup (name);
    free (name);
  }

done:
  snd_ctl_card_info_free (info);
  snd_ctl_close (ctl);

  return ret;
}

gchar *
gst_alsa_find_device_name (GstObject * obj, const gchar * device,
    snd_pcm_t * handle, snd_pcm_stream_t stream)
{
  gchar *ret = NULL;

  if (device != NULL) {
    gchar *dev, *comma;
    gint devnum;

    GST_LOG_OBJECT (obj, "Trying to get device name from string '%s'", device);

    /* only want name:card bit, but not devices and subdevices */
    dev = g_strdup (device);
    if ((comma = strchr (dev, ','))) {
      *comma = '\0';
      devnum = atoi (comma + 1);
      ret = gst_alsa_find_device_name_no_handle (obj, dev, devnum, stream);
    }
    g_free (dev);
  }

  if (ret == NULL && handle != NULL) {
    snd_pcm_info_t *info;

    GST_LOG_OBJECT (obj, "Trying to get device name from open handle");
    snd_pcm_info_malloc (&info);
    snd_pcm_info (handle, info);
    ret = g_strdup (snd_pcm_info_get_name (info));
    snd_pcm_info_free (info);
  }

  GST_LOG_OBJECT (obj, "Device name for device '%s': %s",
      GST_STR_NULL (device), GST_STR_NULL (ret));

  return ret;
}

/* elementfactory information */
static const GstElementDetails gst_alsasink2_details =
GST_ELEMENT_DETAILS ("Audio sink (ALSA)",
    "Sink/Audio",
    "Output to a sound card via ALSA",
    "Wim Taymans <wim@fluendo.com>");

#define DEFAULT_DEVICE		"default"
#define DEFAULT_DEVICE_NAME	""
#define SPDIF_PERIOD_SIZE 1536
#define SPDIF_BUFFER_SIZE 15360

enum
{
  PROP_0,
  PROP_DEVICE,
  PROP_DEVICE_NAME
};

static void gst_alsasink2_init_interfaces (GType type);

GST_BOILERPLATE_FULL (_k_GstAlsaSink, gst_alsasink2, GstAudioSink,
    GST_TYPE_AUDIO_SINK, gst_alsasink2_init_interfaces);

static void gst_alsasink2_finalise (GObject * object);
static void gst_alsasink2_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_alsasink2_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);

static GstCaps *gst_alsasink2_getcaps (GstBaseSink * bsink);

static gboolean gst_alsasink2_open (GstAudioSink * asink);
static gboolean gst_alsasink2_prepare (GstAudioSink * asink,
    GstRingBufferSpec * spec);
static gboolean gst_alsasink2_unprepare (GstAudioSink * asink);
static gboolean gst_alsasink2_close (GstAudioSink * asink);
static guint gst_alsasink2_write (GstAudioSink * asink, gpointer data,
    guint length);
static guint gst_alsasink2_delay (GstAudioSink * asink);
static void gst_alsasink2_reset (GstAudioSink * asink);

static gint output_ref;         /* 0    */
static snd_output_t *output;    /* NULL */
static GStaticMutex output_mutex = G_STATIC_MUTEX_INIT;


#if (G_BYTE_ORDER == G_LITTLE_ENDIAN)
# define ALSA_SINK2_FACTORY_ENDIANNESS	"LITTLE_ENDIAN, BIG_ENDIAN"
#else
# define ALSA_SINK2_FACTORY_ENDIANNESS	"BIG_ENDIAN, LITTLE_ENDIAN"
#endif

static GstStaticPadTemplate alsasink2_sink_factory =
    GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-raw-int, "
        "endianness = (int) { " ALSA_SINK2_FACTORY_ENDIANNESS " }, "
        "signed = (boolean) { TRUE, FALSE }, "
        "width = (int) 32, "
        "depth = (int) 32, "
        "rate = (int) [ 1, MAX ], " "channels = (int) [ 1, MAX ]; "
        "audio/x-raw-int, "
        "endianness = (int) { " ALSA_SINK2_FACTORY_ENDIANNESS " }, "
        "signed = (boolean) { TRUE, FALSE }, "
        "width = (int) 24, "
        "depth = (int) 24, "
        "rate = (int) [ 1, MAX ], " "channels = (int) [ 1, MAX ]; "
        "audio/x-raw-int, "
        "endianness = (int) { " ALSA_SINK2_FACTORY_ENDIANNESS " }, "
        "signed = (boolean) { TRUE, FALSE }, "
        "width = (int) 32, "
        "depth = (int) 24, "
        "rate = (int) [ 1, MAX ], " "channels = (int) [ 1, MAX ]; "
        "audio/x-raw-int, "
        "endianness = (int) { " ALSA_SINK2_FACTORY_ENDIANNESS " }, "
        "signed = (boolean) { TRUE, FALSE }, "
        "width = (int) 16, "
        "depth = (int) 16, "
        "rate = (int) [ 1, MAX ], " "channels = (int) [ 1, MAX ]; "
        "audio/x-raw-int, "
        "signed = (boolean) { TRUE, FALSE }, "
        "width = (int) 8, "
        "depth = (int) 8, "
        "rate = (int) [ 1, MAX ], " "channels = (int) [ 1, MAX ];"
        "audio/x-iec958")
    );

static void
gst_alsasink2_finalise (GObject * object)
{
  _k_GstAlsaSink *sink = GST_ALSA_SINK2 (object);

  g_free (sink->device);
  g_mutex_free (sink->alsa_lock);

  g_static_mutex_lock (&output_mutex);
  --output_ref;
  if (output_ref == 0) {
    snd_output_close (output);
    output = NULL;
  }
  g_static_mutex_unlock (&output_mutex);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gst_alsasink2_init_interfaces (GType type)
{
  gst_alsa_type_add_device_property_probe_interface (type);
}

static void
gst_alsasink2_base_init (gpointer g_class)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (g_class);

  gst_element_class_set_details (element_class, &gst_alsasink2_details);

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&alsasink2_sink_factory));
}
static void
gst_alsasink2_class_init (_k_GstAlsaSinkClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseSinkClass *gstbasesink_class;
  GstBaseAudioSinkClass *gstbaseaudiosink_class;
  GstAudioSinkClass *gstaudiosink_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
  gstbasesink_class = (GstBaseSinkClass *) klass;
  gstbaseaudiosink_class = (GstBaseAudioSinkClass *) klass;
  gstaudiosink_class = (GstAudioSinkClass *) klass;

  parent_class = g_type_class_peek_parent (klass);

  gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_alsasink2_finalise);
  gobject_class->get_property = GST_DEBUG_FUNCPTR (gst_alsasink2_get_property);
  gobject_class->set_property = GST_DEBUG_FUNCPTR (gst_alsasink2_set_property);

  gstbasesink_class->get_caps = GST_DEBUG_FUNCPTR (gst_alsasink2_getcaps);

  gstaudiosink_class->open = GST_DEBUG_FUNCPTR (gst_alsasink2_open);
  gstaudiosink_class->prepare = GST_DEBUG_FUNCPTR (gst_alsasink2_prepare);
  gstaudiosink_class->unprepare = GST_DEBUG_FUNCPTR (gst_alsasink2_unprepare);
  gstaudiosink_class->close = GST_DEBUG_FUNCPTR (gst_alsasink2_close);
  gstaudiosink_class->write = GST_DEBUG_FUNCPTR (gst_alsasink2_write);
  gstaudiosink_class->delay = GST_DEBUG_FUNCPTR (gst_alsasink2_delay);
  gstaudiosink_class->reset = GST_DEBUG_FUNCPTR (gst_alsasink2_reset);

  g_object_class_install_property (gobject_class, PROP_DEVICE,
      g_param_spec_string ("device", "Device",
          "ALSA device, as defined in an asound configuration file",
          DEFAULT_DEVICE, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_DEVICE_NAME,
      g_param_spec_string ("device-name", "Device name",
          "Human-readable name of the sound device", DEFAULT_DEVICE_NAME,
          G_PARAM_READABLE));
}

static void
gst_alsasink2_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  _k_GstAlsaSink *sink;

  sink = GST_ALSA_SINK2 (object);

  switch (prop_id) {
    case PROP_DEVICE:
      g_free (sink->device);
      sink->device = g_value_dup_string (value);
      /* setting NULL restores the default device */
      if (sink->device == NULL) {
        sink->device = g_strdup (DEFAULT_DEVICE);
      }
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_alsasink2_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  _k_GstAlsaSink *sink;

  sink = GST_ALSA_SINK2 (object);

  switch (prop_id) {
    case PROP_DEVICE:
      g_value_set_string (value, sink->device);
      break;
    case PROP_DEVICE_NAME:
      g_value_take_string (value,
          gst_alsa_find_device_name (GST_OBJECT_CAST (sink),
              sink->device, sink->handle, SND_PCM_STREAM_PLAYBACK));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_alsasink2_init (_k_GstAlsaSink * alsasink2, _k_GstAlsaSinkClass * g_class)
{
  GST_DEBUG_OBJECT (alsasink2, "initializing alsasink2");

  alsasink2->device = g_strdup (DEFAULT_DEVICE);
  alsasink2->handle = NULL;
  alsasink2->cached_caps = NULL;
  alsasink2->alsa_lock = g_mutex_new ();

  g_static_mutex_lock (&output_mutex);
  if (output_ref == 0) {
    snd_output_stdio_attach (&output, stdout, 0);
    ++output_ref;
  }
  g_static_mutex_unlock (&output_mutex);
}

#define CHECK(call, error) \
G_STMT_START {                  \
if ((err = call) < 0)           \
  goto error;                   \
} G_STMT_END;

static GstCaps *
gst_alsasink2_getcaps (GstBaseSink * bsink)
{
  GstElementClass *element_class;
  GstPadTemplate *pad_template;
  _k_GstAlsaSink *sink = GST_ALSA_SINK2 (bsink);
  GstCaps *caps;

  if (sink->handle == NULL) {
    GST_DEBUG_OBJECT (sink, "device not open, using template caps");
    return NULL;                /* base class will get template caps for us */
  }

  if (sink->cached_caps) {
    GST_LOG_OBJECT (sink, "Returning cached caps");
    return gst_caps_ref (sink->cached_caps);
  }

  element_class = GST_ELEMENT_GET_CLASS (sink);
  pad_template = gst_element_class_get_pad_template (element_class, "sink");
  g_return_val_if_fail (pad_template != NULL, NULL);

  caps = gst_alsa_probe_supported_formats (GST_OBJECT (sink), sink->handle,
      gst_pad_template_get_caps (pad_template));

  if (caps) {
    sink->cached_caps = gst_caps_ref (caps);
  }

  GST_INFO_OBJECT (sink, "returning caps %" GST_PTR_FORMAT, caps);

  return caps;
}

static int
set_hwparams (_k_GstAlsaSink * alsa)
{
  guint rrate;
  gint err, dir;
  snd_pcm_hw_params_t *params;
  guint period_time, buffer_time;

  snd_pcm_hw_params_malloc (&params);

  GST_DEBUG_OBJECT (alsa, "Negotiating to %d channels @ %d Hz (format = %s) "
      "SPDIF (%d)", alsa->channels, alsa->rate,
      snd_pcm_format_name (alsa->format), alsa->iec958);

  /* start with requested values, if we cannot configure alsa for those values,
   * we set these values to -1, which will leave the default alsa values */
  buffer_time = alsa->buffer_time;
  period_time = alsa->period_time;

retry:
  /* choose all parameters */
  CHECK (snd_pcm_hw_params_any (alsa->handle, params), no_config);
  /* set the interleaved read/write format */
  CHECK (snd_pcm_hw_params_set_access (alsa->handle, params, alsa->access),
      wrong_access);
  /* set the sample format */
#if GST_CHECK_VERSION(0, 10, 18)
  if (alsa->iec958) {
    /* Try to use big endian first else fallback to le and swap bytes */
    if (snd_pcm_hw_params_set_format (alsa->handle, params, alsa->format) < 0) {
      alsa->format = SND_PCM_FORMAT_S16_LE;
      alsa->need_swap = TRUE;
      GST_DEBUG_OBJECT (alsa, "falling back to little endian with swapping");
    } else {
      alsa->need_swap = FALSE;
    }
  }
#endif
  CHECK (snd_pcm_hw_params_set_format (alsa->handle, params, alsa->format),
      no_sample_format);
  /* set the count of channels */
  CHECK (snd_pcm_hw_params_set_channels (alsa->handle, params, alsa->channels),
      no_channels);
  /* set the stream rate */
  rrate = alsa->rate;
  CHECK (snd_pcm_hw_params_set_rate_near (alsa->handle, params, &rrate, NULL),
      no_rate);
  if (rrate != alsa->rate)
    goto rate_match;

  /* get and dump some limits */
  {
    guint min, max;

    snd_pcm_hw_params_get_buffer_time_min (params, &min, &dir);
    snd_pcm_hw_params_get_buffer_time_max (params, &max, &dir);

    GST_DEBUG_OBJECT (alsa, "buffer time %u, min %u, max %u",
        alsa->buffer_time, min, max);

    snd_pcm_hw_params_get_period_time_min (params, &min, &dir);
    snd_pcm_hw_params_get_period_time_max (params, &max, &dir);

    GST_DEBUG_OBJECT (alsa, "period time %u, min %u, max %u",
        alsa->period_time, min, max);

    snd_pcm_hw_params_get_periods_min (params, &min, &dir);
    snd_pcm_hw_params_get_periods_max (params, &max, &dir);

    GST_DEBUG_OBJECT (alsa, "periods min %u, max %u", min, max);
  }

  /* now try to configure the buffer time and period time, if one
   * of those fail, we fall back to the defaults and emit a warning. */
  if (buffer_time != ~0u && !alsa->iec958) {
    /* set the buffer time */
    if ((err = snd_pcm_hw_params_set_buffer_time_near (alsa->handle, params,
                &buffer_time, &dir)) < 0) {
      GST_ELEMENT_WARNING (alsa, RESOURCE, SETTINGS, (NULL),
          ("Unable to set buffer time %i for playback: %s",
              buffer_time, snd_strerror (err)));
      /* disable buffer_time the next round */
      buffer_time = -1;
      goto retry;
    }
    GST_DEBUG_OBJECT (alsa, "buffer time %u", buffer_time);
  }
  if (period_time != ~0u && !alsa->iec958) {
    /* set the period time */
    if ((err = snd_pcm_hw_params_set_period_time_near (alsa->handle, params,
                &period_time, &dir)) < 0) {
      GST_ELEMENT_WARNING (alsa, RESOURCE, SETTINGS, (NULL),
          ("Unable to set period time %i for playback: %s",
              period_time, snd_strerror (err)));
      /* disable period_time the next round */
      period_time = -1;
      goto retry;
    }
    GST_DEBUG_OBJECT (alsa, "period time %u", period_time);
  }

  /* Set buffer size and period size manually for SPDIF */
  if (G_UNLIKELY (alsa->iec958)) {
    snd_pcm_uframes_t buffer_size = SPDIF_BUFFER_SIZE;
    snd_pcm_uframes_t period_size = SPDIF_PERIOD_SIZE;

    CHECK (snd_pcm_hw_params_set_buffer_size_near (alsa->handle, params,
            &buffer_size), buffer_size);
    CHECK (snd_pcm_hw_params_set_period_size_near (alsa->handle, params,
            &period_size, NULL), period_size);
  }

  /* write the parameters to device */
  CHECK (snd_pcm_hw_params (alsa->handle, params), set_hw_params);

  /* now get the configured values */
  CHECK (snd_pcm_hw_params_get_buffer_size (params, &alsa->buffer_size),
      buffer_size);
  CHECK (snd_pcm_hw_params_get_period_size (params, &alsa->period_size, &dir),
      period_size);

  GST_DEBUG_OBJECT (alsa, "buffer size %lu, period size %lu", alsa->buffer_size,
      alsa->period_size);

  snd_pcm_hw_params_free (params);
  return 0;

  /* ERRORS */
no_config:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (NULL),
        ("Broken configuration for playback: no configurations available: %s",
            snd_strerror (err)));
    snd_pcm_hw_params_free (params);
    return err;
  }
wrong_access:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (NULL),
        ("Access type not available for playback: %s", snd_strerror (err)));
    snd_pcm_hw_params_free (params);
    return err;
  }
no_sample_format:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (NULL),
        ("Sample format not available for playback: %s", snd_strerror (err)));
    snd_pcm_hw_params_free (params);
    return err;
  }
no_channels:
  {
    gchar *msg = NULL;

    if ((alsa->channels) == 1)
      msg = g_strdup (_("Could not open device for playback in mono mode."));
    if ((alsa->channels) == 2)
      msg = g_strdup (_("Could not open device for playback in stereo mode."));
    if ((alsa->channels) > 2)
      msg =
          g_strdup_printf (_
          ("Could not open device for playback in %d-channel mode."),
          alsa->channels);
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (msg), (snd_strerror (err)));
    g_free (msg);
    snd_pcm_hw_params_free (params);
    return err;
  }
no_rate:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (NULL),
        ("Rate %iHz not available for playback: %s",
            alsa->rate, snd_strerror (err)));
    return err;
  }
rate_match:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (NULL),
        ("Rate doesn't match (requested %iHz, get %iHz)", alsa->rate, err));
    snd_pcm_hw_params_free (params);
    return -EINVAL;
  }
buffer_size:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (NULL),
        ("Unable to get buffer size for playback: %s", snd_strerror (err)));
    snd_pcm_hw_params_free (params);
    return err;
  }
period_size:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (NULL),
        ("Unable to get period size for playback: %s", snd_strerror (err)));
    snd_pcm_hw_params_free (params);
    return err;
  }
set_hw_params:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (NULL),
        ("Unable to set hw params for playback: %s", snd_strerror (err)));
    snd_pcm_hw_params_free (params);
    return err;
  }
}

static int
set_swparams (_k_GstAlsaSink * alsa)
{
  int err;
  snd_pcm_sw_params_t *params;

  snd_pcm_sw_params_malloc (&params);

  /* get the current swparams */
  CHECK (snd_pcm_sw_params_current (alsa->handle, params), no_config);
  /* start the transfer when the buffer is almost full: */
  /* (buffer_size / avail_min) * avail_min */
  CHECK (snd_pcm_sw_params_set_start_threshold (alsa->handle, params,
          (alsa->buffer_size / alsa->period_size) * alsa->period_size),
      start_threshold);

  /* allow the transfer when at least period_size samples can be processed */
  CHECK (snd_pcm_sw_params_set_avail_min (alsa->handle, params,
          alsa->period_size), set_avail);

#if GST_CHECK_ALSA_VERSION(1,0,16)
  /* snd_pcm_sw_params_set_xfer_align() is deprecated, alignment is always 1 */
#else
  /* align all transfers to 1 sample */
  CHECK (snd_pcm_sw_params_set_xfer_align (alsa->handle, params, 1), set_align);
#endif

  /* write the parameters to the playback device */
  CHECK (snd_pcm_sw_params (alsa->handle, params), set_sw_params);

  snd_pcm_sw_params_free (params);
  return 0;

  /* ERRORS */
no_config:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (NULL),
        ("Unable to determine current swparams for playback: %s",
            snd_strerror (err)));
    snd_pcm_sw_params_free (params);
    return err;
  }
start_threshold:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (NULL),
        ("Unable to set start threshold mode for playback: %s",
            snd_strerror (err)));
    snd_pcm_sw_params_free (params);
    return err;
  }
set_avail:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (NULL),
        ("Unable to set avail min for playback: %s", snd_strerror (err)));
    snd_pcm_sw_params_free (params);
    return err;
  }
#if !GST_CHECK_ALSA_VERSION(1,0,16)
set_align:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (NULL),
        ("Unable to set transfer align for playback: %s", snd_strerror (err)));
    snd_pcm_sw_params_free (params);
    return err;
  }
#endif
set_sw_params:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (NULL),
        ("Unable to set sw params for playback: %s", snd_strerror (err)));
    snd_pcm_sw_params_free (params);
    return err;
  }
}

static gboolean
alsasink2_parse_spec (_k_GstAlsaSink * alsa, GstRingBufferSpec * spec)
{
  /* Initialize our boolean */
  alsa->iec958 = FALSE;

  switch (spec->type) {
    case GST_BUFTYPE_LINEAR:
      GST_DEBUG_OBJECT (alsa,
          "Linear format : depth=%d, width=%d, sign=%d, bigend=%d", spec->depth,
          spec->width, spec->sign, spec->bigend);

      alsa->format = snd_pcm_build_linear_format (spec->depth, spec->width,
          spec->sign ? 0 : 1, spec->bigend ? 1 : 0);
      break;
    case GST_BUFTYPE_FLOAT:
      switch (spec->format) {
        case GST_FLOAT32_LE:
          alsa->format = SND_PCM_FORMAT_FLOAT_LE;
          break;
        case GST_FLOAT32_BE:
          alsa->format = SND_PCM_FORMAT_FLOAT_BE;
          break;
        case GST_FLOAT64_LE:
          alsa->format = SND_PCM_FORMAT_FLOAT64_LE;
          break;
        case GST_FLOAT64_BE:
          alsa->format = SND_PCM_FORMAT_FLOAT64_BE;
          break;
        default:
          goto error;
      }
      break;
    case GST_BUFTYPE_A_LAW:
      alsa->format = SND_PCM_FORMAT_A_LAW;
      break;
    case GST_BUFTYPE_MU_LAW:
      alsa->format = SND_PCM_FORMAT_MU_LAW;
      break;
#if GST_CHECK_VERSION(0, 10, 18)
    case GST_BUFTYPE_IEC958:
      alsa->format = SND_PCM_FORMAT_S16_BE;
      alsa->iec958 = TRUE;
      break;
#endif
    default:
      goto error;

  }
  alsa->rate = spec->rate;
  alsa->channels = spec->channels;
  alsa->buffer_time = spec->buffer_time;
  alsa->period_time = spec->latency_time;
  alsa->access = SND_PCM_ACCESS_RW_INTERLEAVED;

  return TRUE;

  /* ERRORS */
error:
  {
    return FALSE;
  }
}

static gboolean
gst_alsasink2_open (GstAudioSink * asink)
{
  _k_GstAlsaSink *alsa;
  gint err;

  alsa = GST_ALSA_SINK2 (asink);

  CHECK (snd_pcm_open (&alsa->handle, alsa->device, SND_PCM_STREAM_PLAYBACK,
          SND_PCM_NONBLOCK), open_error);
  GST_LOG_OBJECT (alsa, "Opened device %s", alsa->device);

  return TRUE;

  /* ERRORS */
open_error:
  {
    if (err == -EBUSY) {
      GST_ELEMENT_ERROR (alsa, RESOURCE, BUSY,
          (_("Could not open audio device for playback. "
                  "Device is being used by another application.")),
          ("Device '%s' is busy", alsa->device));
    } else {
      GST_ELEMENT_ERROR (alsa, RESOURCE, OPEN_WRITE,
          (_("Could not open audio device for playback.")),
          ("Playback open error on device '%s': %s", alsa->device,
              snd_strerror (err)));
    }
    return FALSE;
  }
}

static gboolean
gst_alsasink2_prepare (GstAudioSink * asink, GstRingBufferSpec * spec)
{
  _k_GstAlsaSink *alsa;
  gint err;

  alsa = GST_ALSA_SINK2 (asink);

#if GST_CHECK_VERSION(0, 10, 18)
  if (spec->format == GST_IEC958) {
    snd_pcm_close (alsa->handle);
    alsa->handle = gst_alsa_open_iec958_pcm (GST_OBJECT (alsa));
    if (G_UNLIKELY (!alsa->handle)) {
      goto no_iec958;
    }
  }
#endif

  if (!alsasink2_parse_spec (alsa, spec))
    goto spec_parse;

  CHECK (set_hwparams (alsa), hw_params_failed);
  CHECK (set_swparams (alsa), sw_params_failed);

  alsa->bytes_per_sample = spec->bytes_per_sample;
  spec->segsize = alsa->period_size * spec->bytes_per_sample;
  spec->segtotal = alsa->buffer_size / alsa->period_size;

  {
    snd_output_t *out_buf = NULL;
    char *msg = NULL;

    snd_output_buffer_open (&out_buf);
    snd_pcm_dump_hw_setup (alsa->handle, out_buf);
    snd_output_buffer_string (out_buf, &msg);
    GST_DEBUG_OBJECT (alsa, "Hardware setup: \n%s", msg);
    snd_output_close (out_buf);
    snd_output_buffer_open (&out_buf);
    snd_pcm_dump_sw_setup (alsa->handle, out_buf);
    snd_output_buffer_string (out_buf, &msg);
    GST_DEBUG_OBJECT (alsa, "Software setup: \n%s", msg);
    snd_output_close (out_buf);
  }

  return TRUE;

  /* ERRORS */
#if GST_CHECK_VERSION(0, 10, 18)
no_iec958:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, OPEN_WRITE, (NULL),
        ("Could not open IEC958 (SPDIF) device for playback"));
    return FALSE;
  }
#endif
spec_parse:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (NULL),
        ("Error parsing spec"));
    return FALSE;
  }
hw_params_failed:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (NULL),
        ("Setting of hwparams failed: %s", snd_strerror (err)));
    return FALSE;
  }
sw_params_failed:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (NULL),
        ("Setting of swparams failed: %s", snd_strerror (err)));
    return FALSE;
  }
}

static gboolean
gst_alsasink2_unprepare (GstAudioSink * asink)
{
  _k_GstAlsaSink *alsa;
  gint err;

  alsa = GST_ALSA_SINK2 (asink);

  CHECK (snd_pcm_drop (alsa->handle), drop);

  CHECK (snd_pcm_hw_free (alsa->handle), hw_free);

  return TRUE;

  /* ERRORS */
drop:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (NULL),
        ("Could not drop samples: %s", snd_strerror (err)));
    return FALSE;
  }
hw_free:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, SETTINGS, (NULL),
        ("Could not free hw params: %s", snd_strerror (err)));
    return FALSE;
  }
}

static gboolean
gst_alsasink2_close (GstAudioSink * asink)
{
  _k_GstAlsaSink *alsa = GST_ALSA_SINK2 (asink);
  gint err;

  if (alsa->handle) {
    CHECK (snd_pcm_close (alsa->handle), close_error);
    alsa->handle = NULL;
  }
  gst_caps_replace (&alsa->cached_caps, NULL);

  return TRUE;

  /* ERRORS */
close_error:
  {
    GST_ELEMENT_ERROR (alsa, RESOURCE, CLOSE, (NULL),
        ("Playback close error: %s", snd_strerror (err)));
    return FALSE;
  }
}


/*
 *   Underrun and suspend recovery
 */
static gint
xrun_recovery (_k_GstAlsaSink * alsa, snd_pcm_t * handle, gint err)
{
  GST_DEBUG_OBJECT (alsa, "xrun recovery %d", err);

  if (err == -EPIPE) {          /* under-run */
    err = snd_pcm_prepare (handle);
    if (err < 0) {
      GST_WARNING_OBJECT (alsa,
          "Can't recovery from underrun, prepare failed: %s",
          snd_strerror (err));
    }
    return 0;
  } else if (err == -ESTRPIPE) {
    while ((err = snd_pcm_resume (handle)) == -EAGAIN)
      g_usleep (100);           /* wait until the suspend flag is released */

    if (err < 0) {
      err = snd_pcm_prepare (handle);
      if (err < 0) {
        GST_WARNING_OBJECT (alsa,
            "Can't recovery from suspend, prepare failed: %s",
            snd_strerror (err));
      }
    }
    return 0;
  }
  return err;
}

static guint
gst_alsasink2_write (GstAudioSink * asink, gpointer data, guint length)
{
  _k_GstAlsaSink *alsa;
  gint err;
  gint cptr;
  gint16 *ptr = data;

  alsa = GST_ALSA_SINK2 (asink);

  if (alsa->iec958 && alsa->need_swap) {
    guint i;

    GST_DEBUG_OBJECT (asink, "swapping bytes");
    for (i = 0; i < length / 2; i++) {
      ptr[i] = GUINT16_SWAP_LE_BE (ptr[i]);
    }
  }

  GST_LOG_OBJECT (asink, "received audio samples buffer of %u bytes", length);

  cptr = length / alsa->bytes_per_sample;

  GST_ALSA_SINK2_LOCK (asink);
  while (cptr > 0) {
    /* start by doing a blocking wait for free space. Set the timeout
     * to 4 times the period time */
    err = snd_pcm_wait (alsa->handle, (4 * alsa->period_time / 1000));
    if (err < 0) {
      GST_DEBUG_OBJECT (asink, "wait timeout, %d", err);
    } else {
      err = snd_pcm_writei (alsa->handle, ptr, cptr);
    }

    GST_DEBUG_OBJECT (asink, "written %d frames out of %d", err, cptr);
    if (err < 0) {
      GST_DEBUG_OBJECT (asink, "Write error: %s", snd_strerror (err));
      if (err == -EAGAIN) {
        continue;
      } else if (xrun_recovery (alsa, alsa->handle, err) < 0) {
        goto write_error;
      }
      continue;
    }

    ptr += snd_pcm_frames_to_bytes (alsa->handle, err);
    cptr -= err;
  }
  GST_ALSA_SINK2_UNLOCK (asink);

  return length - (cptr * alsa->bytes_per_sample);

write_error:
  {
    GST_ALSA_SINK2_UNLOCK (asink);
    return length;              /* skip one period */
  }
}

static guint
gst_alsasink2_delay (GstAudioSink * asink)
{
  _k_GstAlsaSink *alsa;
  snd_pcm_sframes_t delay;
  int res;

  alsa = GST_ALSA_SINK2 (asink);

  res = snd_pcm_delay (alsa->handle, &delay);
  if (G_UNLIKELY (res < 0)) {
    /* on errors, report 0 delay */
    GST_DEBUG_OBJECT (alsa, "snd_pcm_delay returned %d", res);
    delay = 0;
  }
  if (G_UNLIKELY (delay < 0)) {
    /* make sure we never return a negative delay */
    GST_WARNING_OBJECT (alsa, "snd_pcm_delay returned negative delay");
    delay = 0;
  }

  return delay;
}

static void
gst_alsasink2_reset (GstAudioSink * asink)
{
  _k_GstAlsaSink *alsa;
  gint err;

  alsa = GST_ALSA_SINK2 (asink);

  GST_ALSA_SINK2_LOCK (asink);
  GST_DEBUG_OBJECT (alsa, "drop");
  CHECK (snd_pcm_drop (alsa->handle), drop_error);
  GST_DEBUG_OBJECT (alsa, "prepare");
  CHECK (snd_pcm_prepare (alsa->handle), prepare_error);
  GST_DEBUG_OBJECT (alsa, "reset done");
  GST_ALSA_SINK2_UNLOCK (asink);

  return;

  /* ERRORS */
drop_error:
  {
    GST_ERROR_OBJECT (alsa, "alsa-reset: pcm drop error: %s",
        snd_strerror (err));
    GST_ALSA_SINK2_UNLOCK (asink);
    return;
  }
prepare_error:
  {
    GST_ERROR_OBJECT (alsa, "alsa-reset: pcm prepare error: %s",
        snd_strerror (err));
    GST_ALSA_SINK2_UNLOCK (asink);
    return;
  }
}

static void
gst_alsa_error_wrapper (const char *file, int line, const char *function,
    int err, const char *fmt, ...)
{
}

static gboolean
plugin_init (GstPlugin * plugin)
{
  int err;

  if (!gst_element_register (plugin, "_k_alsasink", GST_RANK_PRIMARY,
          GST_TYPE_ALSA_SINK2))
    return FALSE;

  err = snd_lib_error_set_handler (gst_alsa_error_wrapper);
  if (err != 0)
    GST_WARNING ("failed to set alsa error handler");

  return TRUE;
}

#define PACKAGE ""
GST_PLUGIN_DEFINE_STATIC (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "_k_alsa",
    "ALSA plugin library (hotfixed)",
    plugin_init, "0.1", "LGPL", "Phonon-GStreamer", "")
#undef PACKAGE
