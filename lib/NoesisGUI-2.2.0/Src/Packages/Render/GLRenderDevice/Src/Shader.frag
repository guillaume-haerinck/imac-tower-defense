#if defined(EFFECT_RGBA)

#elif defined(EFFECT_MASK)

#elif defined(EFFECT_PATH_SOLID)
    #define EFFECT_PATH 1
    #define PAINT_SOLID 1

#elif defined(EFFECT_PATH_LINEAR)
    #define EFFECT_PATH 1
    #define PAINT_LINEAR 1

#elif defined(EFFECT_PATH_RADIAL)
    #define EFFECT_PATH 1
    #define PAINT_RADIAL 1

#elif defined(EFFECT_PATH_PATTERN)
    #define EFFECT_PATH 1
    #define PAINT_PATTERN 1

#elif defined(EFFECT_PATH_AA_SOLID)
    #define EFFECT_PATH_AA 1
    #define PAINT_SOLID 1

#elif defined(EFFECT_PATH_AA_LINEAR)
    #define EFFECT_PATH_AA 1
    #define PAINT_LINEAR 1

#elif defined(EFFECT_PATH_AA_RADIAL)
    #define EFFECT_PATH_AA 1
    #define PAINT_RADIAL 1

#elif defined(EFFECT_PATH_AA_PATTERN)
    #define EFFECT_PATH_AA 1
    #define PAINT_PATTERN 1

#elif defined(EFFECT_IMAGE_PAINT_OPACITY_SOLID)
    #define EFFECT_IMAGE_PAINT_OPACITY 1
    #define PAINT_SOLID 1

#elif defined(EFFECT_IMAGE_PAINT_OPACITY_LINEAR)
    #define EFFECT_IMAGE_PAINT_OPACITY 1
    #define PAINT_LINEAR 1

#elif defined(EFFECT_IMAGE_PAINT_OPACITY_RADIAL)
    #define EFFECT_IMAGE_PAINT_OPACITY 1
    #define PAINT_RADIAL 1

#elif defined(EFFECT_IMAGE_PAINT_OPACITY_PATTERN)
    #define EFFECT_IMAGE_PAINT_OPACITY 1
    #define PAINT_PATTERN 1

#elif defined(EFFECT_TEXT_SOLID)
    #define EFFECT_TEXT 1
    #define PAINT_SOLID 1

#elif defined(EFFECT_TEXT_LINEAR)
    #define EFFECT_TEXT 1
    #define PAINT_LINEAR 1

#elif defined(EFFECT_TEXT_RADIAL)
    #define EFFECT_TEXT 1
    #define PAINT_RADIAL 1

#elif defined(EFFECT_TEXT_PATTERN)
    #define EFFECT_TEXT 1
    #define PAINT_PATTERN 1
#else
    #error EFFECT not defined
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(EFFECT_PATH_AA)
    #define HAS_COVERAGE 1
#endif

#if defined(PAINT_SOLID)
    #define HAS_COLOR 1
#endif

#if defined(PAINT_LINEAR) || defined(PAINT_RADIAL) || defined(PAINT_PATTERN)
    #define HAS_UV0 1
#endif

#if defined(EFFECT_IMAGE_PAINT_OPACITY) || defined(EFFECT_TEXT)
    #define HAS_UV1 1
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef PRECISION_QUALIFIERS
    #define highp
    #define mediump
    #define lowp
#endif

#ifdef CORE_PROFILE
    out vec4 fragColor;
    #define FRAG_COLOR fragColor
#else
    #define FRAG_COLOR gl_FragData[0]
#endif

uniform lowp vec4 rgba;
uniform mediump vec4 radialGrad[2];
uniform lowp float opacity;

uniform lowp sampler2D pattern;
uniform lowp sampler2D ramps;
uniform lowp sampler2D image;
uniform lowp sampler2D glyphs;

#ifdef HAS_COLOR
    varying lowp vec4 color;
#endif

#ifdef HAS_UV0
    varying mediump vec2 uv0;
#endif

#ifdef HAS_UV1
    varying mediump vec2 uv1;
#endif

#ifdef HAS_COVERAGE
    varying lowp float coverage;
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
    /////////////////////////////////////////////////////
    // Fetch paint color and opacity
    /////////////////////////////////////////////////////
    #if defined(PAINT_SOLID)
        lowp vec4 paint = color;
        lowp float opacity_ = 1.0f;

    #elif defined(PAINT_LINEAR)
        lowp vec4 paint = texture2D(ramps, uv0);
        lowp float opacity_ = opacity;

    #elif defined(PAINT_RADIAL)
        mediump float dd = radialGrad[1].y * uv0.x - radialGrad[1].z * uv0.y;
        mediump float u = radialGrad[0].x * uv0.x + radialGrad[0].y * uv0.y + radialGrad[0].z * 
            sqrt(radialGrad[0].w * uv0.x * uv0.x + radialGrad[1].x * uv0.y * uv0.y - dd * dd);
        lowp vec4 paint = texture2D(ramps, vec2(u, radialGrad[1].w));
        lowp float opacity_ = opacity;

    #elif defined(PAINT_PATTERN)
        lowp vec4 paint = texture2D(pattern, uv0);
        lowp float opacity_ = opacity;
    #endif

    /////////////////////////////////////////////////////
    // Apply selected effect
    /////////////////////////////////////////////////////
    #if defined(EFFECT_RGBA)
        FRAG_COLOR = rgba;

    #elif defined(EFFECT_MASK)
        FRAG_COLOR = vec4(1.0f, 1.0f, 1.0f, 1.0f);

    #elif defined(EFFECT_PATH)
        FRAG_COLOR = opacity_ * paint;

    #elif defined(EFFECT_PATH_AA)
        FRAG_COLOR = (opacity_ * coverage) * paint;

    #elif defined(EFFECT_IMAGE_PAINT_OPACITY)
        FRAG_COLOR = texture2D(image, uv1) * (opacity_ * paint.a);

    #elif defined(EFFECT_TEXT)
        FRAG_COLOR = (texture2D(glyphs, uv1).r * opacity_) * paint;

    #endif
}
