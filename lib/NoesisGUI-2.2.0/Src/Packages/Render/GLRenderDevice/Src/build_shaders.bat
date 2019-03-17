@echo off

:: OpenGL 2.1
Type NUL > ShadersGL120.h
CALL :build_vertex :build_120
CALL :build_fragment :build_120

:: OpenGL 3.1
Type NUL > ShadersGL140.h
CALL :build_vertex :build_140
CALL :build_fragment :build_140

:: OpenGL ES
Type NUL > ShadersGLES.h
CALL :build_vertex :build_es
CALL :build_fragment :build_es

goto :eof

:build_vertex
    CALL %* /vertex /Vn Pos_VS Shader.vert
    CALL %* /vertex /D HAS_COLOR /Vn PosColor_VS Shader.vert
    CALL %* /vertex /D HAS_UV0 /Vn PosTex0_VS Shader.vert
    CALL %* /vertex /D HAS_COLOR /D HAS_COVERAGE /Vn PosColorCoverage_VS Shader.vert
    CALL %* /vertex /D HAS_UV0 /D HAS_COVERAGE /Vn PosTex0Coverage_VS Shader.vert
    CALL %* /vertex /D HAS_COLOR /D HAS_UV1 /Vn PosColorTex1_VS Shader.vert
    CALL %* /vertex /D HAS_UV0 /D HAS_UV1 /Vn PosTex0Tex1_VS Shader.vert
    goto :eof

:build_fragment
    CALL %* /fragment /Vn RGBA_FS /D EFFECT_RGBA Shader.frag
    CALL %* /fragment /Vn Mask_FS /D EFFECT_MASK Shader.frag
    CALL %* /fragment /Vn PathSolid_FS /D EFFECT_PATH_SOLID Shader.frag
    CALL %* /fragment /Vn PathLinear_FS /D EFFECT_PATH_LINEAR Shader.frag
    CALL %* /fragment /Vn PathRadial_FS /D EFFECT_PATH_RADIAL Shader.frag
    CALL %* /fragment /Vn PathPattern_FS /D EFFECT_PATH_PATTERN Shader.frag
    CALL %* /fragment /Vn PathAASolid_FS /D EFFECT_PATH_AA_SOLID Shader.frag
    CALL %* /fragment /Vn PathAALinear_FS /D EFFECT_PATH_AA_LINEAR Shader.frag
    CALL %* /fragment /Vn PathAARadial_FS /D EFFECT_PATH_AA_RADIAL Shader.frag
    CALL %* /fragment /Vn PathAAPattern_FS /D EFFECT_PATH_AA_PATTERN Shader.frag
    CALL %* /fragment /Vn ImagePaintOpacitySolid_FS /D EFFECT_IMAGE_PAINT_OPACITY_SOLID Shader.frag
    CALL %* /fragment /Vn ImagePaintOpacityLinear_FS /D EFFECT_IMAGE_PAINT_OPACITY_LINEAR Shader.frag
    CALL %* /fragment /Vn ImagePaintOpacityRadial_FS /D EFFECT_IMAGE_PAINT_OPACITY_RADIAL Shader.frag
    CALL %* /fragment /Vn ImagePaintOpacityPattern_FS /D EFFECT_IMAGE_PAINT_OPACITY_PATTERN Shader.frag
    CALL %* /fragment /Vn TextSolid_FS /D EFFECT_TEXT_SOLID Shader.frag
    CALL %* /fragment /Vn TextLinear_FS /D EFFECT_TEXT_LINEAR Shader.frag
    CALL %* /fragment /Vn TextRadial_FS /D EFFECT_TEXT_RADIAL Shader.frag
    CALL %* /fragment /Vn TextPattern_FS /D EFFECT_TEXT_PATTERN Shader.frag
    goto :eof

:: -------------------------------------------------------------------------------------------------------
:build_120
    glsl_optimizer /header /V 120 %* >> ShadersGL120.h
    goto :eof

:: -------------------------------------------------------------------------------------------------------
:build_140
    glsl_optimizer /header /V 140 /D CORE_PROFILE %* >> ShadersGL140.h
    goto :eof

:: -------------------------------------------------------------------------------------------------------
:build_es
    glsl_optimizer /header /gl_es /D PRECISION_QUALIFIERS %* >> ShadersGLES.h
    goto :eof

