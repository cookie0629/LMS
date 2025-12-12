
#pragma once

#define STBIR_DEFAULT_FILTER_DOWNSAMPLE STBIR_FILTER_MITCHELL
#define STBIR_DEFAULT_FILTER_UPSAMPLE STBIR_FILTER_CATMULLROM

#if STB_IMAGE_RESIZE_VERSION == 1
    #include <stb_image_resize.h>
#elif STB_IMAGE_RESIZE_VERSION == 2
    #include <stb_image_resize2.h>
#else
    #error "Unhandled STB image resize version"!
#endif
