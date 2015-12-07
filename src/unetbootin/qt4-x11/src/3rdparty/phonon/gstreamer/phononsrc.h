/*  This file is part of the KDE project.

Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 or 3 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __PHONON_SRC_H__
#define __PHONON_SRC_H__

#include <sys/types.h>
#include <gst/gst.h>
#include <gst/base/gstbasesrc.h>
#include "streamreader.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Gstreamer
{

G_BEGIN_DECLS

#define GST_TYPE_PHONON_SRC \
  (phonon_src_get_type())
#define GST_PHONON_SRC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_PHONON_SRC,PhononSrc))
#define GST_PHONON_SRC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_PHONON_SRC,PhononSrcClass))
#define GST_IS_PHONON_SRC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_PHONON_SRC))
#define GST_IS_PHONON_SRC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_PHONON_SRC))

typedef struct _PhononSrc PhononSrc;
typedef struct _PhononSrcClass PhononSrcClass;

// PhononSrc:
struct _PhononSrc {
    GstBaseSrc element;
    StreamReader *device;
};

struct _PhononSrcClass {
    GstBaseSrcClass parent_class;
};

GType phonon_src_get_type (void);

G_END_DECLS

}
} //namespace Phonon::Gstreamer

QT_END_NAMESPACE


#endif // __PHONON_SRC_H__
