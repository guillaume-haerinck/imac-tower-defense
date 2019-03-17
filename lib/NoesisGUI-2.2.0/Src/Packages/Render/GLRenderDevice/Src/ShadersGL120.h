const char Pos_VS[] =
{
    "#version 120\n"
    "attribute vec4 attr_pos;\n"
    "uniform mat4 projectionMtx;\n"
    "void main ()\n"
    "{\n"
    "  gl_Position = (attr_pos * projectionMtx);\n"
    "}"
};

const char PosColor_VS[] =
{
    "#version 120\n"
    "attribute vec4 attr_pos;\n"
    "attribute vec4 attr_color;\n"
    "varying vec4 color;\n"
    "uniform mat4 projectionMtx;\n"
    "void main ()\n"
    "{\n"
    "  gl_Position = (attr_pos * projectionMtx);\n"
    "  color = attr_color;\n"
    "}"
};

const char PosTex0_VS[] =
{
    "#version 120\n"
    "attribute vec4 attr_pos;\n"
    "attribute vec2 attr_tex0;\n"
    "varying vec2 uv0;\n"
    "uniform mat4 projectionMtx;\n"
    "void main ()\n"
    "{\n"
    "  gl_Position = (attr_pos * projectionMtx);\n"
    "  uv0 = attr_tex0;\n"
    "}"
};

const char PosColorCoverage_VS[] =
{
    "#version 120\n"
    "attribute vec4 attr_pos;\n"
    "attribute vec4 attr_color;\n"
    "varying vec4 color;\n"
    "attribute float attr_coverage;\n"
    "varying float coverage;\n"
    "uniform mat4 projectionMtx;\n"
    "void main ()\n"
    "{\n"
    "  gl_Position = (attr_pos * projectionMtx);\n"
    "  color = attr_color;\n"
    "  coverage = attr_coverage;\n"
    "}"
};

const char PosTex0Coverage_VS[] =
{
    "#version 120\n"
    "attribute vec4 attr_pos;\n"
    "attribute vec2 attr_tex0;\n"
    "varying vec2 uv0;\n"
    "attribute float attr_coverage;\n"
    "varying float coverage;\n"
    "uniform mat4 projectionMtx;\n"
    "void main ()\n"
    "{\n"
    "  gl_Position = (attr_pos * projectionMtx);\n"
    "  uv0 = attr_tex0;\n"
    "  coverage = attr_coverage;\n"
    "}"
};

const char PosColorTex1_VS[] =
{
    "#version 120\n"
    "attribute vec4 attr_pos;\n"
    "attribute vec4 attr_color;\n"
    "varying vec4 color;\n"
    "attribute vec2 attr_tex1;\n"
    "varying vec2 uv1;\n"
    "uniform mat4 projectionMtx;\n"
    "void main ()\n"
    "{\n"
    "  gl_Position = (attr_pos * projectionMtx);\n"
    "  color = attr_color;\n"
    "  uv1 = attr_tex1;\n"
    "}"
};

const char PosTex0Tex1_VS[] =
{
    "#version 120\n"
    "attribute vec4 attr_pos;\n"
    "attribute vec2 attr_tex0;\n"
    "varying vec2 uv0;\n"
    "attribute vec2 attr_tex1;\n"
    "varying vec2 uv1;\n"
    "uniform mat4 projectionMtx;\n"
    "void main ()\n"
    "{\n"
    "  gl_Position = (attr_pos * projectionMtx);\n"
    "  uv0 = attr_tex0;\n"
    "  uv1 = attr_tex1;\n"
    "}"
};

const char RGBA_FS[] =
{
    "#version 120\n"
    "uniform vec4 rgba;\n"
    "void main ()\n"
    "{\n"
    "  gl_FragData[0] = rgba;\n"
    "}"
};

const char Mask_FS[] =
{
    "#version 120\n"
    "void main ()\n"
    "{\n"
    "  gl_FragData[0] = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "}"
};

const char PathSolid_FS[] =
{
    "#version 120\n"
    "varying vec4 color;\n"
    "void main ()\n"
    "{\n"
    "  gl_FragData[0] = color;\n"
    "}"
};

const char PathLinear_FS[] =
{
    "#version 120\n"
    "uniform float opacity;\n"
    "uniform sampler2D ramps;\n"
    "varying vec2 uv0;\n"
    "void main ()\n"
    "{\n"
    "  gl_FragData[0] = (opacity * texture2D (ramps, uv0));\n"
    "}"
};

const char PathRadial_FS[] =
{
    "#version 120\n"
    "uniform vec4 radialGrad[2];\n"
    "uniform float opacity;\n"
    "uniform sampler2D ramps;\n"
    "varying vec2 uv0;\n"
    "void main ()\n"
    "{\n"
    "  float tmpvar_1;\n"
    "  tmpvar_1 = ((radialGrad[1].y * uv0.x) - (radialGrad[1].z * uv0.y));\n"
    "  vec2 tmpvar_2;\n"
    "  tmpvar_2.x = (((radialGrad[0].x * uv0.x) + (radialGrad[0].y * uv0.y)) + (radialGrad[0].z * sqrt(\n"
    "    ((((radialGrad[0].w * uv0.x) * uv0.x) + ((radialGrad[1].x * uv0.y) * uv0.y)) - (tmpvar_1 * tmpvar_1))\n"
    "  )));\n"
    "  tmpvar_2.y = radialGrad[1].w;\n"
    "  gl_FragData[0] = (opacity * texture2D (ramps, tmpvar_2));\n"
    "}"
};

const char PathPattern_FS[] =
{
    "#version 120\n"
    "uniform float opacity;\n"
    "uniform sampler2D pattern;\n"
    "varying vec2 uv0;\n"
    "void main ()\n"
    "{\n"
    "  gl_FragData[0] = (opacity * texture2D (pattern, uv0));\n"
    "}"
};

const char PathAASolid_FS[] =
{
    "#version 120\n"
    "varying vec4 color;\n"
    "varying float coverage;\n"
    "void main ()\n"
    "{\n"
    "  gl_FragData[0] = (coverage * color);\n"
    "}"
};

const char PathAALinear_FS[] =
{
    "#version 120\n"
    "uniform float opacity;\n"
    "uniform sampler2D ramps;\n"
    "varying vec2 uv0;\n"
    "varying float coverage;\n"
    "void main ()\n"
    "{\n"
    "  gl_FragData[0] = ((opacity * coverage) * texture2D (ramps, uv0));\n"
    "}"
};

const char PathAARadial_FS[] =
{
    "#version 120\n"
    "uniform vec4 radialGrad[2];\n"
    "uniform float opacity;\n"
    "uniform sampler2D ramps;\n"
    "varying vec2 uv0;\n"
    "varying float coverage;\n"
    "void main ()\n"
    "{\n"
    "  float tmpvar_1;\n"
    "  tmpvar_1 = ((radialGrad[1].y * uv0.x) - (radialGrad[1].z * uv0.y));\n"
    "  vec2 tmpvar_2;\n"
    "  tmpvar_2.x = (((radialGrad[0].x * uv0.x) + (radialGrad[0].y * uv0.y)) + (radialGrad[0].z * sqrt(\n"
    "    ((((radialGrad[0].w * uv0.x) * uv0.x) + ((radialGrad[1].x * uv0.y) * uv0.y)) - (tmpvar_1 * tmpvar_1))\n"
    "  )));\n"
    "  tmpvar_2.y = radialGrad[1].w;\n"
    "  gl_FragData[0] = ((opacity * coverage) * texture2D (ramps, tmpvar_2));\n"
    "}"
};

const char PathAAPattern_FS[] =
{
    "#version 120\n"
    "uniform float opacity;\n"
    "uniform sampler2D pattern;\n"
    "varying vec2 uv0;\n"
    "varying float coverage;\n"
    "void main ()\n"
    "{\n"
    "  gl_FragData[0] = ((opacity * coverage) * texture2D (pattern, uv0));\n"
    "}"
};

const char ImagePaintOpacitySolid_FS[] =
{
    "#version 120\n"
    "uniform sampler2D image;\n"
    "varying vec4 color;\n"
    "varying vec2 uv1;\n"
    "void main ()\n"
    "{\n"
    "  gl_FragData[0] = (texture2D (image, uv1) * color.w);\n"
    "}"
};

const char ImagePaintOpacityLinear_FS[] =
{
    "#version 120\n"
    "uniform float opacity;\n"
    "uniform sampler2D ramps;\n"
    "uniform sampler2D image;\n"
    "varying vec2 uv0;\n"
    "varying vec2 uv1;\n"
    "void main ()\n"
    "{\n"
    "  gl_FragData[0] = (texture2D (image, uv1) * (opacity * texture2D (ramps, uv0).w));\n"
    "}"
};

const char ImagePaintOpacityRadial_FS[] =
{
    "#version 120\n"
    "uniform vec4 radialGrad[2];\n"
    "uniform float opacity;\n"
    "uniform sampler2D ramps;\n"
    "uniform sampler2D image;\n"
    "varying vec2 uv0;\n"
    "varying vec2 uv1;\n"
    "void main ()\n"
    "{\n"
    "  float tmpvar_1;\n"
    "  tmpvar_1 = ((radialGrad[1].y * uv0.x) - (radialGrad[1].z * uv0.y));\n"
    "  vec2 tmpvar_2;\n"
    "  tmpvar_2.x = (((radialGrad[0].x * uv0.x) + (radialGrad[0].y * uv0.y)) + (radialGrad[0].z * sqrt(\n"
    "    ((((radialGrad[0].w * uv0.x) * uv0.x) + ((radialGrad[1].x * uv0.y) * uv0.y)) - (tmpvar_1 * tmpvar_1))\n"
    "  )));\n"
    "  tmpvar_2.y = radialGrad[1].w;\n"
    "  gl_FragData[0] = (texture2D (image, uv1) * (opacity * texture2D (ramps, tmpvar_2).w));\n"
    "}"
};

const char ImagePaintOpacityPattern_FS[] =
{
    "#version 120\n"
    "uniform float opacity;\n"
    "uniform sampler2D pattern;\n"
    "uniform sampler2D image;\n"
    "varying vec2 uv0;\n"
    "varying vec2 uv1;\n"
    "void main ()\n"
    "{\n"
    "  gl_FragData[0] = (texture2D (image, uv1) * (opacity * texture2D (pattern, uv0).w));\n"
    "}"
};

const char TextSolid_FS[] =
{
    "#version 120\n"
    "uniform sampler2D glyphs;\n"
    "varying vec4 color;\n"
    "varying vec2 uv1;\n"
    "void main ()\n"
    "{\n"
    "  gl_FragData[0] = (texture2D (glyphs, uv1).x * color);\n"
    "}"
};

const char TextLinear_FS[] =
{
    "#version 120\n"
    "uniform float opacity;\n"
    "uniform sampler2D ramps;\n"
    "uniform sampler2D glyphs;\n"
    "varying vec2 uv0;\n"
    "varying vec2 uv1;\n"
    "void main ()\n"
    "{\n"
    "  gl_FragData[0] = ((texture2D (glyphs, uv1).x * opacity) * texture2D (ramps, uv0));\n"
    "}"
};

const char TextRadial_FS[] =
{
    "#version 120\n"
    "uniform vec4 radialGrad[2];\n"
    "uniform float opacity;\n"
    "uniform sampler2D ramps;\n"
    "uniform sampler2D glyphs;\n"
    "varying vec2 uv0;\n"
    "varying vec2 uv1;\n"
    "void main ()\n"
    "{\n"
    "  float tmpvar_1;\n"
    "  tmpvar_1 = ((radialGrad[1].y * uv0.x) - (radialGrad[1].z * uv0.y));\n"
    "  vec2 tmpvar_2;\n"
    "  tmpvar_2.x = (((radialGrad[0].x * uv0.x) + (radialGrad[0].y * uv0.y)) + (radialGrad[0].z * sqrt(\n"
    "    ((((radialGrad[0].w * uv0.x) * uv0.x) + ((radialGrad[1].x * uv0.y) * uv0.y)) - (tmpvar_1 * tmpvar_1))\n"
    "  )));\n"
    "  tmpvar_2.y = radialGrad[1].w;\n"
    "  gl_FragData[0] = ((texture2D (glyphs, uv1).x * opacity) * texture2D (ramps, tmpvar_2));\n"
    "}"
};

const char TextPattern_FS[] =
{
    "#version 120\n"
    "uniform float opacity;\n"
    "uniform sampler2D pattern;\n"
    "uniform sampler2D glyphs;\n"
    "varying vec2 uv0;\n"
    "varying vec2 uv1;\n"
    "void main ()\n"
    "{\n"
    "  gl_FragData[0] = ((texture2D (glyphs, uv1).x * opacity) * texture2D (pattern, uv0));\n"
    "}"
};

