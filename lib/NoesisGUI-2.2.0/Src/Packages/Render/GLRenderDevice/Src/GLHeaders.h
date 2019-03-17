////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __RENDER_GLHEADERS_H__
#define __RENDER_GLHEADERS_H__


#define NS_RENDERER_USE_WGL 0
#define NS_RENDERER_USE_EGL 0
#define NS_RENDERER_USE_EMS 0
#define NS_RENDERER_USE_EAGL 0
#define NS_RENDERER_USE_NSGL 0
#define NS_RENDERER_USE_GLX 0


#include <NsCore/Noesis.h>
#include <NsCore/Error.h>


#if defined(NS_PLATFORM_IPHONE) || defined(NS_PLATFORM_ANDROID) || defined(NS_PLATFORM_EMSCRIPTEN)
    #define NS_RENDERER_OPENGL_ES 1
#else
    #define NS_RENDERER_OPENGL 1
#endif


#if NS_RENDERER_OPENGL

    #define GL_APIENTRY APIENTRY
    #define GL_APIENTRYP APIENTRYP

    #if defined(NS_PLATFORM_WINDOWS)
        #undef NS_RENDERER_USE_WGL
        #define NS_RENDERER_USE_WGL 1
        #include <windows.h>
        #include <gl/GL.h>
        #include "glext.h"

    #elif defined(NS_PLATFORM_OSX)
        #undef NS_RENDERER_USE_NSGL
        #define NS_RENDERER_USE_NSGL 1
        #define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
        #include <OpenGL/gl3.h>
        #include <OpenGL/gl3ext.h>
        #include <OpenGL/gl.h>
        #include <OpenGL/glext.h>
        
    #elif defined(NS_PLATFORM_LINUX)
        #undef NS_RENDERER_USE_GLX
        #define NS_RENDERER_USE_GLX 1
        #include <GL/gl.h>
        #include <GL/glext.h>

    #else
        #error Platform not supported
    #endif

#elif NS_RENDERER_OPENGL_ES

    #if defined(NS_PLATFORM_IPHONE)
        #include <OpenGLES/ES3/gl.h>
        #include <OpenGLES/ES2/glext.h>
        #include <OpenGLES/ES3/glext.h>
        #undef NS_RENDERER_USE_EAGL
        #define NS_RENDERER_USE_EAGL 1

    #elif defined(NS_PLATFORM_ANDROID)
        #include <GLES3/gl32.h>
        #include <GLES3/gl3ext.h>
        #include <GLES2/gl2ext.h>
        #undef NS_RENDERER_USE_EGL
        #define NS_RENDERER_USE_EGL 1

    #elif defined(NS_PLATFORM_EMSCRIPTEN)
        #include <GLES3/gl32.h>
        #undef NS_RENDERER_USE_EMS
        #define NS_RENDERER_USE_EMS 1
    #else
        #error Platform not supported
    #endif
#endif

#if NS_RENDERER_USE_WGL
    typedef PROC (GL_APIENTRYP PFNWGLGETPROCADDRESSPROC)(LPCSTR lpszProc);
    typedef HGLRC (GL_APIENTRYP PFNWGLGETCURRENTCONTEXT)(VOID);

    typedef GLenum (GL_APIENTRYP PFNGLGETERRORPROC)(void);
    typedef void (GL_APIENTRYP PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
    typedef void (GL_APIENTRYP PFNGLTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
    typedef void (GL_APIENTRYP PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
    typedef void (GL_APIENTRYP PFNGLGETTEXLEVELPARAMETERIVPROC)(GLenum target, GLint level, GLenum pname, GLint *params);
    typedef void (GL_APIENTRYP PFNGLPIXELSTOREI)(GLenum pname, GLint param);
    typedef void (GL_APIENTRYP PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
    typedef void (GL_APIENTRYP PFNGLGENTEXTURESPROC)(GLsizei n, GLuint *textures);
    typedef GLboolean (GL_APIENTRYP PFNGLISTEXTUREPROC)(GLuint texture);
    typedef void (GL_APIENTRYP PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint *textures);
    typedef void (GL_APIENTRYP PFNGLCOLORMASKPROC)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
    typedef void (GL_APIENTRYP PFNGLDEPTHFUNCPROC)(GLenum func);
    typedef void (GL_APIENTRYP PFNGLDISABLEPROC)(GLenum cap);
    typedef void (GL_APIENTRYP PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
    typedef void (GL_APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
    typedef void (GL_APIENTRYP PFNGLGETINTEGERVPROC)(GLenum pname, GLint *params);
    typedef void (GL_APIENTRYP PFNGLGETBOOLEANVPROC)(GLenum pname, GLboolean *params);
    typedef void (GL_APIENTRYP PFNGLGETFLOATVPROC)(GLenum pname, GLfloat *params);
    typedef void (GL_APIENTRYP PFNGLGETTEXPARAMETERIVPROC)(GLenum target, GLenum pname, GLint *params);
    typedef const GLubyte * (GL_APIENTRYP PFNGLGETSTRINGPROC) (GLenum name);
    typedef void (GL_APIENTRYP PFNGLDRAWARRAYSPROC)(GLenum mode, GLint first, GLsizei count);
    typedef void (GL_APIENTRYP PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
    typedef void (GL_APIENTRYP PFNGLPOINTSIZEPROC)(GLfloat size);
    typedef void (GL_APIENTRYP PFNGLCULLFACEPROC)(GLenum mode);
    typedef void (GL_APIENTRYP PFNGLCLEARPROC)(GLbitfield mask);
    typedef void (GL_APIENTRYP PFNGLSCISSORPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
    typedef void (GL_APIENTRYP PFNGLENABLEPROC)(GLenum cap);
    typedef void (GL_APIENTRYP PFNGLCLEARSTENCILPROC)(GLint s);
    typedef void (GL_APIENTRYP PFNGLDEPTHMASKPROC) (GLboolean flag);
    typedef void (GL_APIENTRYP PFNGLCLEARDEPTHPROC)(GLdouble depth);
    typedef void (GL_APIENTRYP PFNGLCLEARCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    typedef void (GL_APIENTRYP PFNGLSTENCILFUNCPROC)(GLenum func, GLint ref, GLuint mask);
    typedef void (GL_APIENTRYP PFNGLSTENCILMASKPROC)(GLuint mask);
    typedef void (GL_APIENTRYP PFNGLSTENCILOPPROC)(GLenum fail, GLenum zfail, GLenum zpass);
    typedef void (GL_APIENTRYP PFNGLFLUSHPROC)(void);
    typedef GLboolean (GL_APIENTRYP PFNGLISENABLEDPROC)(GLenum);
    typedef void (GL_APIENTRYP PFNGLDISABLECLIENTSTATEPROC)(GLenum array);
#endif

#if NS_RENDERER_USE_EGL
    #include <EGL/egl.h>
#endif

#if NS_RENDERER_USE_GLX
    #include <GL/glx.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef GL_RGB8
    #define GL_RGB8 0x8051
#endif

#ifndef GL_RGBA8
    #define GL_RGBA8 0x8058
#endif

#ifndef GL_BGRA
    #define GL_BGRA  0x80E1
#endif

#ifndef GL_BGRA_EXT
    #define GL_BGRA_EXT 0x80E1
#endif

#ifndef GL_BGRA8_EXT
    #define GL_BGRA8_EXT 0x93A1
#endif

#ifndef GL_R8
    #define GL_R8 0x8229
#endif

#ifndef GL_RED
    #define GL_RED 0x1903
#endif

#ifndef GL_SAMPLER_BINDING
    #define GL_SAMPLER_BINDING 0x8919
#endif

#ifndef GL_MAX_SAMPLES
    #define GL_MAX_SAMPLES 0x8D57
#endif

#ifndef GL_MAX_SAMPLES_EXT
    #define GL_MAX_SAMPLES_EXT 0x8D57
#endif

#ifndef GL_MAX_SAMPLES_ANGLE
    #define GL_MAX_SAMPLES_ANGLE 0x8D57
#endif

#ifndef GL_MAX_SAMPLES_APPLE
    #define GL_MAX_SAMPLES_APPLE 0x8D57
#endif

#ifndef GL_MAX_SAMPLES_IMG
    #define GL_MAX_SAMPLES_IMG 0x9135
#endif

#ifndef GL_READ_FRAMEBUFFER
    #define GL_READ_FRAMEBUFFER 0x8CA8
#endif

#ifndef GL_DRAW_FRAMEBUFFER
    #define GL_DRAW_FRAMEBUFFER 0x8CA9
#endif

#ifdef GL_MAP_READ_BIT
    #define GL_MAP_READ_BIT 0x0001
#endif

#ifndef GL_MAP_WRITE_BIT
    #define GL_MAP_WRITE_BIT 0x0002
#endif

#ifndef GL_MAP_INVALIDATE_RANGE_BIT
    #define GL_MAP_INVALIDATE_RANGE_BIT 0x0004
#endif

#ifndef GL_MAP_INVALIDATE_BUFFER_BIT
    #define GL_MAP_INVALIDATE_BUFFER_BIT 0x0008
#endif

#ifndef GL_MAP_FLUSH_EXPLICIT_BIT
    #define GL_MAP_FLUSH_EXPLICIT_BIT 0x0010
#endif

#ifndef GL_MAP_UNSYNCHRONIZED_BIT
    #define GL_MAP_UNSYNCHRONIZED_BIT 0x0020
#endif

#ifndef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
    #define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#endif

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
    #define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#endif

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
    #define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#endif

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
    #define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#endif

#ifndef GL_TEXTURE_MAX_LEVEL
    #define GL_TEXTURE_MAX_LEVEL 0x813D
#endif

#ifndef GL_HALF_FLOAT
    #define GL_HALF_FLOAT 0x140B
#endif

#ifndef GL_HALF_FLOAT_OES
    #define GL_HALF_FLOAT_OES 0x8D61
#endif

#ifndef GL_VERTEX_ARRAY_BINDING
    #define GL_VERTEX_ARRAY_BINDING 0x85B5
#endif

#ifndef GL_NUM_EXTENSIONS
    #define GL_NUM_EXTENSIONS 0x821D
#endif

#ifndef GL_DEBUG_OUTPUT
    #define GL_DEBUG_OUTPUT 0x92E0
#endif

#ifndef GL_DEBUG_OUTPUT_SYNCHRONOUS
    #define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242
#endif

#ifndef GL_DEBUG_SOURCE_API
    #define GL_DEBUG_SOURCE_API 0x8246
#endif

#ifndef GL_DEBUG_SOURCE_WINDOW_SYSTEM
    #define GL_DEBUG_SOURCE_WINDOW_SYSTEM 0x8247
#endif

#ifndef GL_DEBUG_SOURCE_SHADER_COMPILER
    #define GL_DEBUG_SOURCE_SHADER_COMPILER 0x8248
#endif

#ifndef GL_DEBUG_SOURCE_THIRD_PARTY
    #define GL_DEBUG_SOURCE_THIRD_PARTY 0x8249
#endif

#ifndef GL_DEBUG_SOURCE_APPLICATION
    #define GL_DEBUG_SOURCE_APPLICATION 0x824A
#endif

#ifndef GL_DEBUG_SOURCE_OTHER
    #define GL_DEBUG_SOURCE_OTHER 0x824B
#endif

#ifndef GL_DEBUG_TYPE_ERROR
    #define GL_DEBUG_TYPE_ERROR 0x824C
#endif

#ifndef GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR
    #define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#endif

#ifndef GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR
    #define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
#endif

#ifndef GL_DEBUG_TYPE_PORTABILITY
    #define GL_DEBUG_TYPE_PORTABILITY 0x824F
#endif

#ifndef GL_DEBUG_TYPE_PERFORMANCE
    #define GL_DEBUG_TYPE_PERFORMANCE 0x8250
#endif

#ifndef GL_DEBUG_TYPE_OTHER
    #define GL_DEBUG_TYPE_OTHER 0x8251
#endif

#ifndef GL_DEBUG_SEVERITY_HIGH
    #define GL_DEBUG_SEVERITY_HIGH 0x9146
#endif

#ifndef GL_DEBUG_SEVERITY_MEDIUM
    #define GL_DEBUG_SEVERITY_MEDIUM 0x9147
#endif

#ifndef GL_DEBUG_SEVERITY_LOW
    #define GL_DEBUG_SEVERITY_LOW 0x9148
#endif

#ifndef GL_CLAMP_TO_BORDER
    #define GL_CLAMP_TO_BORDER 0x812D
#endif

#ifndef GL_LUMINANCE8
    #define GL_LUMINANCE8 0x8040
#endif

#ifndef GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
    #define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#endif

#ifndef GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
    #define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS 0x8CD9
#endif

#ifndef GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
    #define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#endif

#ifndef GL_FRAMEBUFFER_UNSUPPORTED
    #define GL_FRAMEBUFFER_UNSUPPORTED 0x8CDD
#endif

#ifndef GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
    #define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB
#endif

#ifndef GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
    #define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC
#endif

#ifndef GL_FRAMEBUFFER_UNDEFINED
    #define GL_FRAMEBUFFER_UNDEFINED 0x8219
#endif

#ifndef GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
    #define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE 0x8D56
#endif

#endif
