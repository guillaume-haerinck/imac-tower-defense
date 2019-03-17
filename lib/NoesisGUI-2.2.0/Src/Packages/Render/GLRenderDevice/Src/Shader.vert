#ifndef PRECISION_QUALIFIERS
    #define highp
    #define mediump
    #define lowp
#endif

#ifdef CORE_PROFILE
    #define IN in
    #define OUT out
#else
    #define IN attribute
    #define OUT varying
#endif

IN highp vec4 attr_pos;

#ifdef HAS_COLOR
    IN lowp vec4 attr_color;
    OUT lowp vec4 color;
#endif

#ifdef HAS_UV0
    IN mediump vec2 attr_tex0;
    OUT mediump vec2 uv0;
#endif

#ifdef HAS_UV1
    IN mediump vec2 attr_tex1;
    OUT mediump vec2 uv1;
#endif

#ifdef HAS_COVERAGE
    IN lowp float attr_coverage;
    OUT lowp float coverage;
#endif

uniform mediump mat4 projectionMtx;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
    gl_Position = (attr_pos * projectionMtx);

#ifdef HAS_COLOR
    color = attr_color;
#endif

#ifdef HAS_UV0
    uv0 = attr_tex0;
#endif

#ifdef HAS_UV1
    uv1 = attr_tex1;
#endif

#ifdef HAS_COVERAGE
    coverage = attr_coverage;
#endif
}