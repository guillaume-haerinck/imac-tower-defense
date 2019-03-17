const char Pos_VS[] =
{
    "#version 140\n"
    "in vec4 attr_pos;\n"
    "uniform mat4 projectionMtx;\n"
    "void main ()\n"
    "{\n"
    "  gl_Position = (attr_pos * projectionMtx);\n"
    "}"
};

const char PosColor_VS[] =
{
    "#version 140\n"
    "in vec4 attr_pos;\n"
    "in vec4 attr_color;\n"
    "out vec4 color;\n"
    "uniform mat4 projectionMtx;\n"
    "void main ()\n"
    "{\n"
    "  gl_Position = (attr_pos * projectionMtx);\n"
    "  color = attr_color;\n"
    "}"
};

const char PosTex0_VS[] =
{
    "#version 140\n"
    "in vec4 attr_pos;\n"
    "in vec2 attr_tex0;\n"
    "out vec2 uv0;\n"
    "uniform mat4 projectionMtx;\n"
    "void main ()\n"
    "{\n"
    "  gl_Position = (attr_pos * projectionMtx);\n"
    "  uv0 = attr_tex0;\n"
    "}"
};

const char PosColorCoverage_VS[] =
{
    "#version 140\n"
    "in vec4 attr_pos;\n"
    "in vec4 attr_color;\n"
    "out vec4 color;\n"
    "in float attr_coverage;\n"
    "out float coverage;\n"
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
    "#version 140\n"
    "in vec4 attr_pos;\n"
    "in vec2 attr_tex0;\n"
    "out vec2 uv0;\n"
    "in float attr_coverage;\n"
    "out float coverage;\n"
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
    "#version 140\n"
    "in vec4 attr_pos;\n"
    "in vec4 attr_color;\n"
    "out vec4 color;\n"
    "in vec2 attr_tex1;\n"
    "out vec2 uv1;\n"
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
    "#version 140\n"
    "in vec4 attr_pos;\n"
    "in vec2 attr_tex0;\n"
    "out vec2 uv0;\n"
    "in vec2 attr_tex1;\n"
    "out vec2 uv1;\n"
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
    "#version 140\n"
    "out vec4 fragColor;\n"
    "uniform vec4 rgba;\n"
    "void main ()\n"
    "{\n"
    "  fragColor = rgba;\n"
    "}"
};

const char Mask_FS[] =
{
    "#version 140\n"
    "out vec4 fragColor;\n"
    "void main ()\n"
    "{\n"
    "  fragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "}"
};

const char PathSolid_FS[] =
{
    "#version 140\n"
    "out vec4 fragColor;\n"
    "in vec4 color;\n"
    "void main ()\n"
    "{\n"
    "  fragColor = color;\n"
    "}"
};

const char PathLinear_FS[] =
{
    "#version 140\n"
    "out vec4 fragColor;\n"
    "uniform float opacity;\n"
    "uniform sampler2D ramps;\n"
    "in vec2 uv0;\n"
    "void main ()\n"
    "{\n"
    "  fragColor = (opacity * texture (ramps, uv0));\n"
    "}"
};

const char PathRadial_FS[] =
{
    "#version 140\n"
    "out vec4 fragColor;\n"
    "uniform vec4 radialGrad[2];\n"
    "uniform float opacity;\n"
    "uniform sampler2D ramps;\n"
    "in vec2 uv0;\n"
    "void main ()\n"
    "{\n"
    "  float tmpvar_1;\n"
    "  tmpvar_1 = ((radialGrad[1].y * uv0.x) - (radialGrad[1].z * uv0.y));\n"
    "  vec2 tmpvar_2;\n"
    "  tmpvar_2.x = (((radialGrad[0].x * uv0.x) + (radialGrad[0].y * uv0.y)) + (radialGrad[0].z * sqrt(\n"
    "    ((((radialGrad[0].w * uv0.x) * uv0.x) + ((radialGrad[1].x * uv0.y) * uv0.y)) - (tmpvar_1 * tmpvar_1))\n"
    "  )));\n"
    "  tmpvar_2.y = radialGrad[1].w;\n"
    "  fragColor = (opacity * texture (ramps, tmpvar_2));\n"
    "}"
};

const char PathPattern_FS[] =
{
    "#version 140\n"
    "out vec4 fragColor;\n"
    "uniform float opacity;\n"
    "uniform sampler2D pattern;\n"
    "in vec2 uv0;\n"
    "void main ()\n"
    "{\n"
    "  fragColor = (opacity * texture (pattern, uv0));\n"
    "}"
};

const char PathAASolid_FS[] =
{
    "#version 140\n"
    "out vec4 fragColor;\n"
    "in vec4 color;\n"
    "in float coverage;\n"
    "void main ()\n"
    "{\n"
    "  fragColor = (coverage * color);\n"
    "}"
};

const char PathAALinear_FS[] =
{
    "#version 140\n"
    "out vec4 fragColor;\n"
    "uniform float opacity;\n"
    "uniform sampler2D ramps;\n"
    "in vec2 uv0;\n"
    "in float coverage;\n"
    "void main ()\n"
    "{\n"
    "  fragColor = ((opacity * coverage) * texture (ramps, uv0));\n"
    "}"
};

const char PathAARadial_FS[] =
{
    "#version 140\n"
    "out vec4 fragColor;\n"
    "uniform vec4 radialGrad[2];\n"
    "uniform float opacity;\n"
    "uniform sampler2D ramps;\n"
    "in vec2 uv0;\n"
    "in float coverage;\n"
    "void main ()\n"
    "{\n"
    "  float tmpvar_1;\n"
    "  tmpvar_1 = ((radialGrad[1].y * uv0.x) - (radialGrad[1].z * uv0.y));\n"
    "  vec2 tmpvar_2;\n"
    "  tmpvar_2.x = (((radialGrad[0].x * uv0.x) + (radialGrad[0].y * uv0.y)) + (radialGrad[0].z * sqrt(\n"
    "    ((((radialGrad[0].w * uv0.x) * uv0.x) + ((radialGrad[1].x * uv0.y) * uv0.y)) - (tmpvar_1 * tmpvar_1))\n"
    "  )));\n"
    "  tmpvar_2.y = radialGrad[1].w;\n"
    "  fragColor = ((opacity * coverage) * texture (ramps, tmpvar_2));\n"
    "}"
};

const char PathAAPattern_FS[] =
{
    "#version 140\n"
    "out vec4 fragColor;\n"
    "uniform float opacity;\n"
    "uniform sampler2D pattern;\n"
    "in vec2 uv0;\n"
    "in float coverage;\n"
    "void main ()\n"
    "{\n"
    "  fragColor = ((opacity * coverage) * texture (pattern, uv0));\n"
    "}"
};

const char ImagePaintOpacitySolid_FS[] =
{
    "#version 140\n"
    "out vec4 fragColor;\n"
    "uniform sampler2D image;\n"
    "in vec4 color;\n"
    "in vec2 uv1;\n"
    "void main ()\n"
    "{\n"
    "  fragColor = (texture (image, uv1) * color.w);\n"
    "}"
};

const char ImagePaintOpacityLinear_FS[] =
{
    "#version 140\n"
    "out vec4 fragColor;\n"
    "uniform float opacity;\n"
    "uniform sampler2D ramps;\n"
    "uniform sampler2D image;\n"
    "in vec2 uv0;\n"
    "in vec2 uv1;\n"
    "void main ()\n"
    "{\n"
    "  fragColor = (texture (image, uv1) * (opacity * texture (ramps, uv0).w));\n"
    "}"
};

const char ImagePaintOpacityRadial_FS[] =
{
    "#version 140\n"
    "out vec4 fragColor;\n"
    "uniform vec4 radialGrad[2];\n"
    "uniform float opacity;\n"
    "uniform sampler2D ramps;\n"
    "uniform sampler2D image;\n"
    "in vec2 uv0;\n"
    "in vec2 uv1;\n"
    "void main ()\n"
    "{\n"
    "  float tmpvar_1;\n"
    "  tmpvar_1 = ((radialGrad[1].y * uv0.x) - (radialGrad[1].z * uv0.y));\n"
    "  vec2 tmpvar_2;\n"
    "  tmpvar_2.x = (((radialGrad[0].x * uv0.x) + (radialGrad[0].y * uv0.y)) + (radialGrad[0].z * sqrt(\n"
    "    ((((radialGrad[0].w * uv0.x) * uv0.x) + ((radialGrad[1].x * uv0.y) * uv0.y)) - (tmpvar_1 * tmpvar_1))\n"
    "  )));\n"
    "  tmpvar_2.y = radialGrad[1].w;\n"
    "  fragColor = (texture (image, uv1) * (opacity * texture (ramps, tmpvar_2).w));\n"
    "}"
};

const char ImagePaintOpacityPattern_FS[] =
{
    "#version 140\n"
    "out vec4 fragColor;\n"
    "uniform float opacity;\n"
    "uniform sampler2D pattern;\n"
    "uniform sampler2D image;\n"
    "in vec2 uv0;\n"
    "in vec2 uv1;\n"
    "void main ()\n"
    "{\n"
    "  fragColor = (texture (image, uv1) * (opacity * texture (pattern, uv0).w));\n"
    "}"
};

const char TextSolid_FS[] =
{
    "#version 140\n"
    "out vec4 fragColor;\n"
    "uniform sampler2D glyphs;\n"
    "in vec4 color;\n"
    "in vec2 uv1;\n"
    "void main ()\n"
    "{\n"
    "  fragColor = (texture (glyphs, uv1).x * color);\n"
    "}"
};

const char TextLinear_FS[] =
{
    "#version 140\n"
    "out vec4 fragColor;\n"
    "uniform float opacity;\n"
    "uniform sampler2D ramps;\n"
    "uniform sampler2D glyphs;\n"
    "in vec2 uv0;\n"
    "in vec2 uv1;\n"
    "void main ()\n"
    "{\n"
    "  fragColor = ((texture (glyphs, uv1).x * opacity) * texture (ramps, uv0));\n"
    "}"
};

const char TextRadial_FS[] =
{
    "#version 140\n"
    "out vec4 fragColor;\n"
    "uniform vec4 radialGrad[2];\n"
    "uniform float opacity;\n"
    "uniform sampler2D ramps;\n"
    "uniform sampler2D glyphs;\n"
    "in vec2 uv0;\n"
    "in vec2 uv1;\n"
    "void main ()\n"
    "{\n"
    "  float tmpvar_1;\n"
    "  tmpvar_1 = ((radialGrad[1].y * uv0.x) - (radialGrad[1].z * uv0.y));\n"
    "  vec2 tmpvar_2;\n"
    "  tmpvar_2.x = (((radialGrad[0].x * uv0.x) + (radialGrad[0].y * uv0.y)) + (radialGrad[0].z * sqrt(\n"
    "    ((((radialGrad[0].w * uv0.x) * uv0.x) + ((radialGrad[1].x * uv0.y) * uv0.y)) - (tmpvar_1 * tmpvar_1))\n"
    "  )));\n"
    "  tmpvar_2.y = radialGrad[1].w;\n"
    "  fragColor = ((texture (glyphs, uv1).x * opacity) * texture (ramps, tmpvar_2));\n"
    "}"
};

const char TextPattern_FS[] =
{
    "#version 140\n"
    "out vec4 fragColor;\n"
    "uniform float opacity;\n"
    "uniform sampler2D pattern;\n"
    "uniform sampler2D glyphs;\n"
    "in vec2 uv0;\n"
    "in vec2 uv1;\n"
    "void main ()\n"
    "{\n"
    "  fragColor = ((texture (glyphs, uv1).x * opacity) * texture (pattern, uv0));\n"
    "}"
};

