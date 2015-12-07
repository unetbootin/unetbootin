#include <gst/gst.h>
#include <gst/interfaces/propertyprobe.h>
#include <gst/interfaces/xoverlay.h>

#if !defined(GST_VERSION_MAJOR) \
    || !defined(GST_VERSION_MINOR)
#  error "No GST_VERSION_* macros"
#elif GST_VERION_MAJOR != 0 && GST_VERSION_MINOR != 10
#  error "Incompatible version of GStreamer found (Version 0.10.x is required)."
#endif

int main(int argc, char **argv)
{
}
