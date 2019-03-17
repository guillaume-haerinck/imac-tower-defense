////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef GL_IMPORT
    #error GL_IMPORT(_optional, _proto, _func) must be defined!
#endif

#if NS_RENDERER_USE_WGL
    // OpenGL1.1
    GL_IMPORT(false, PFNGLGETERRORPROC, glGetError);
    GL_IMPORT(false, PFNGLTEXIMAGE2DPROC, glTexImage2D);
    GL_IMPORT(false, PFNGLTEXSUBIMAGE2DPROC, glTexSubImage2D);
    GL_IMPORT(false, PFNGLPIXELSTOREI, glPixelStorei);
    GL_IMPORT(false, PFNGLTEXPARAMETERIPROC, glTexParameteri);
    GL_IMPORT(false, PFNGLGETTEXLEVELPARAMETERIVPROC, glGetTexLevelParameteriv);
    GL_IMPORT(false, PFNGLBINDTEXTUREPROC, glBindTexture);
    GL_IMPORT(false, PFNGLGENTEXTURESPROC, glGenTextures);
    GL_IMPORT(false, PFNGLISTEXTUREPROC, glIsTexture);
    GL_IMPORT(false, PFNGLDELETETEXTURESPROC, glDeleteTextures);
    GL_IMPORT(false, PFNGLCOLORMASKPROC, glColorMask);
    GL_IMPORT(false, PFNGLDEPTHFUNCPROC, glDepthFunc);
    GL_IMPORT(false, PFNGLDISABLEPROC, glDisable);
    GL_IMPORT(false, PFNGLVIEWPORTPROC, glViewport);
    GL_IMPORT(false, PFNGLDRAWELEMENTSPROC, glDrawElements);
    GL_IMPORT(false, PFNGLGETINTEGERVPROC, glGetIntegerv);
    GL_IMPORT(false, PFNGLGETBOOLEANVPROC, glGetBooleanv);
    GL_IMPORT(false, PFNGLGETFLOATVPROC, glGetFloatv);
    GL_IMPORT(false, PFNGLGETTEXPARAMETERIVPROC, glGetTexParameteriv);
    GL_IMPORT(false, PFNGLGETSTRINGPROC, glGetString);
    GL_IMPORT(false, PFNGLDRAWARRAYSPROC, glDrawArrays);
    GL_IMPORT(false, PFNGLBLENDFUNCPROC, glBlendFunc);
    GL_IMPORT(false, PFNGLBLENDEQUATIONPROC, glBlendEquation);
    GL_IMPORT(false, PFNGLPOINTSIZEPROC, glPointSize);
    GL_IMPORT(false, PFNGLCULLFACEPROC, glCullFace);
    GL_IMPORT(false, PFNGLCLEARPROC, glClear);
    GL_IMPORT(false, PFNGLSCISSORPROC, glScissor);
    GL_IMPORT(false, PFNGLENABLEPROC, glEnable);
    GL_IMPORT(false, PFNGLCLEARSTENCILPROC, glClearStencil);
    GL_IMPORT(false, PFNGLDEPTHMASKPROC, glDepthMask);
    GL_IMPORT(false, PFNGLCLEARDEPTHPROC, glClearDepth);
    GL_IMPORT(false, PFNGLCLEARCOLORPROC, glClearColor);
    GL_IMPORT(false, PFNGLSTENCILFUNCPROC, glStencilFunc);
    GL_IMPORT(false, PFNGLSTENCILMASKPROC, glStencilMask);
    GL_IMPORT(false, PFNGLSTENCILOPPROC, glStencilOp);
    GL_IMPORT(false, PFNGLFLUSHPROC, glFlush);
    GL_IMPORT(false, PFNGLISENABLEDPROC, glIsEnabled);
    GL_IMPORT(false, PFNGLDISABLECLIENTSTATEPROC, glDisableClientState);
#endif

#if NS_RENDERER_USE_WGL || NS_RENDERER_USE_GLX
    // OpenGL2.1
    GL_IMPORT(false, PFNGLISSHADERPROC, glIsShader);
    GL_IMPORT(false, PFNGLGENBUFFERSPROC, glGenBuffers);
    GL_IMPORT(false, PFNGLISBUFFERPROC, glIsBuffer);
    GL_IMPORT(false, PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
    GL_IMPORT(false, PFNGLBINDBUFFERPROC, glBindBuffer);
    GL_IMPORT(false, PFNGLBUFFERDATAPROC, glBufferData);
    GL_IMPORT(false, PFNGLBUFFERSUBDATAPROC, glBufferSubData);
    GL_IMPORT(false, PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
    GL_IMPORT(false, PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray);
    GL_IMPORT(false, PFNGLGETVERTEXATTRIBIVPROC, glGetVertexAttribiv);
    GL_IMPORT(false, PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
    GL_IMPORT(false, PFNGLCREATESHADERPROC, glCreateShader);
    GL_IMPORT(false, PFNGLDELETESHADERPROC, glDeleteShader);
    GL_IMPORT(false, PFNGLSHADERSOURCEPROC, glShaderSource);
    GL_IMPORT(false, PFNGLCOMPILESHADERPROC, glCompileShader);
    GL_IMPORT(false, PFNGLGETSHADERIVPROC, glGetShaderiv);
    GL_IMPORT(false, PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
    GL_IMPORT(false, PFNGLCREATEPROGRAMPROC, glCreateProgram);
    GL_IMPORT(false, PFNGLISPROGRAMPROC, glIsProgram);
    GL_IMPORT(false, PFNGLDELETEPROGRAMPROC, glDeleteProgram);
    GL_IMPORT(false, PFNGLATTACHSHADERPROC, glAttachShader);
    GL_IMPORT(false, PFNGLLINKPROGRAMPROC, glLinkProgram);
    GL_IMPORT(false, PFNGLUSEPROGRAMPROC, glUseProgram);
    GL_IMPORT(false, PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
    GL_IMPORT(false, PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation);
    GL_IMPORT(false, PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
    GL_IMPORT(false, PFNGLBINDATTRIBLOCATIONPROC, glBindAttribLocation);
    GL_IMPORT(false, PFNGLGETPROGRAMIVPROC, glGetProgramiv);
    GL_IMPORT(false, PFNGLGETACTIVEUNIFORMPROC, glGetActiveUniform);
    GL_IMPORT(false, PFNGLGETACTIVEATTRIBPROC, glGetActiveAttrib);
    GL_IMPORT(false, PFNGLUNIFORM1IPROC, glUniform1i);
    GL_IMPORT(false, PFNGLUNIFORM1FPROC, glUniform1f);
    GL_IMPORT(false, PFNGLUNIFORM2FPROC, glUniform2f);
    GL_IMPORT(false, PFNGLUNIFORM3FPROC, glUniform3f);
    GL_IMPORT(false, PFNGLUNIFORM4FPROC, glUniform4f);
    GL_IMPORT(false, PFNGLUNIFORM4FVPROC, glUniform4fv);
    GL_IMPORT(false, PFNGLUNIFORMMATRIX2FVPROC, glUniformMatrix2fv);
    GL_IMPORT(false, PFNGLUNIFORMMATRIX3FVPROC, glUniformMatrix3fv);
    GL_IMPORT(false, PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);
    GL_IMPORT(false, PFNGLACTIVETEXTUREPROC, glActiveTexture);
    GL_IMPORT(false, PFNGLCOMPRESSEDTEXIMAGE2DPROC, glCompressedTexImage2D);
    GL_IMPORT(false, PFNGLISRENDERBUFFERPROC, glIsRenderbuffer);
    GL_IMPORT(false, PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers);
    GL_IMPORT(false, PFNGLGENRENDERBUFFERSPROC, glGenRenderbuffers);
    GL_IMPORT(false, PFNGLBINDRENDERBUFFERPROC, glBindRenderbuffer);
    GL_IMPORT(false, PFNGLRENDERBUFFERSTORAGEPROC, glRenderbufferStorage);
    GL_IMPORT(false, PFNGLISFRAMEBUFFEREXTPROC, glIsFramebuffer);
    GL_IMPORT(false, PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer);
    GL_IMPORT(false, PFNGLDELETERENDERBUFFERSPROC, glDeleteRenderbuffers);
    GL_IMPORT(false, PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers);
    GL_IMPORT(false, PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus);
    GL_IMPORT(false, PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D);
    GL_IMPORT(false, PFNGLFRAMEBUFFERRENDERBUFFERPROC, glFramebufferRenderbuffer);
#endif

#if NS_RENDERER_USE_EGL || NS_RENDERER_USE_WGL || NS_RENDERER_USE_GLX || NS_RENDERER_USE_NSGL

    #if GL_VERSION_4_3 || GL_ES_VERSION_3_2 || (NS_RENDERER_OPENGL && GL_KHR_debug)
        GL_IMPORT(true, PFNGLPUSHDEBUGGROUPPROC, glPushDebugGroup);
        GL_IMPORT(true, PFNGLPOPDEBUGGROUPPROC, glPopDebugGroup);
        GL_IMPORT(true, PFNGLOBJECTLABELPROC, glObjectLabel);
    #endif

    #if NS_RENDERER_OPENGL_ES && GL_KHR_debug
        GL_IMPORT(true, PFNGLPUSHDEBUGGROUPKHRPROC, glPushDebugGroupKHR);
        GL_IMPORT(true, PFNGLPOPDEBUGGROUPKHRPROC, glPopDebugGroupKHR);
        GL_IMPORT(true, PFNGLOBJECTLABELKHRPROC, glObjectLabelKHR);
    #endif

    #if GL_VERSION_4_3 || GL_ES_VERSION_3_1 || GL_ARB_vertex_attrib_binding
        GL_IMPORT(true, PFNGLBINDVERTEXBUFFERPROC, glBindVertexBuffer);
        GL_IMPORT(true, PFNGLVERTEXATTRIBFORMATPROC, glVertexAttribFormat);
        GL_IMPORT(true, PFNGLVERTEXATTRIBBINDINGPROC, glVertexAttribBinding);
    #endif

    #if GL_VERSION_3_0 || GL_ES_VERSION_3_0 || GL_ARB_map_buffer_range
        GL_IMPORT(true, PFNGLMAPBUFFERRANGEPROC, glMapBufferRange);
        GL_IMPORT(true, PFNGLUNMAPBUFFERPROC, glUnmapBuffer);
    #endif

    #if GL_EXT_map_buffer_range
        GL_IMPORT(true, PFNGLMAPBUFFERRANGEPROC, glMapBufferRangeEXT);
        GL_IMPORT(true, PFNGLUNMAPBUFFEROESPROC, glUnmapBufferOES);
    #endif

    #if GL_VERSION_3_0 || GL_ES_VERSION_3_0 || GL_ARB_vertex_array_object
        GL_IMPORT(true, PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
        GL_IMPORT(true, PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);
        GL_IMPORT(true, PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
        GL_IMPORT(true, PFNGLISVERTEXARRAYPROC, glIsVertexArray);
    #endif

    #if GL_APPLE_vertex_array_object
        #if NS_RENDERER_USE_NSGL
            typedef void(*PFNGLBINDVERTEXARRAYAPPLEPROC)(GLuint id);
            typedef void(*PFNGLDELETEVERTEXARRAYSAPPLEPROC)(GLsizei n, const GLuint *ids);
            typedef void(*PFNGLGENVERTEXARRAYSAPPLEPROC)(GLsizei n, GLuint *ids);
            typedef GLboolean(*PFNGLISVERTEXARRAYAPPLEPROC)(GLuint id);
        #endif
        GL_IMPORT(true, PFNGLBINDVERTEXARRAYAPPLEPROC, glBindVertexArrayAPPLE);
        GL_IMPORT(true, PFNGLDELETEVERTEXARRAYSAPPLEPROC, glDeleteVertexArraysAPPLE);
        GL_IMPORT(true, PFNGLGENVERTEXARRAYSAPPLEPROC, glGenVertexArraysAPPLE);
        GL_IMPORT(true, PFNGLISVERTEXARRAYAPPLEPROC, glIsVertexArrayAPPLE);
    #endif

    #if GL_APPLE_framebuffer_multisample
        GL_IMPORT(true, PFNGLRENDERBUFFERSTORAGEMULTISAMPLEAPPLEPROC, glRenderbufferStorageMultisampleAPPLE);
    #endif

    #if GL_OES_vertex_array_object
        GL_IMPORT(true, PFNGLBINDVERTEXARRAYOESPROC, glBindVertexArrayOES);
        GL_IMPORT(true, PFNGLDELETEVERTEXARRAYSOESPROC, glDeleteVertexArraysOES);
        GL_IMPORT(true, PFNGLGENVERTEXARRAYSOESPROC, glGenVertexArraysOES);
        GL_IMPORT(true, PFNGLISVERTEXARRAYOESPROC, glIsVertexArrayOES);
    #endif

    #if GL_VERSION_3_0 || GL_ES_VERSION_3_0 || GL_ARB_framebuffer_object
        GL_IMPORT(true, PFNGLBLITFRAMEBUFFERPROC, glBlitFramebuffer);
        GL_IMPORT(true, PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC, glRenderbufferStorageMultisample);
    #endif

    #if GL_EXT_framebuffer_blit
        #if NS_RENDERER_USE_NSGL
            typedef void(*PFNGLBLITFRAMEBUFFEREXTPROC) (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
        #endif
        GL_IMPORT(true, PFNGLBLITFRAMEBUFFEREXTPROC, glBlitFramebufferEXT);
    #endif

    #if GL_ANGLE_framebuffer_blit
        GL_IMPORT(true, PFNGLBLITFRAMEBUFFERANGLEPROC, glBlitFramebufferANGLE);
    #endif

    #if GL_EXT_framebuffer_multisample
        #if NS_RENDERER_USE_NSGL
            typedef void(*PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
        #endif
        GL_IMPORT(true, PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC, glRenderbufferStorageMultisampleEXT);
    #endif

    #if GL_ANGLE_framebuffer_multisample
        GL_IMPORT(true, PFNGLRENDERBUFFERSTORAGEMULTISAMPLEANGLEPROC, glRenderbufferStorageMultisampleANGLE);
    #endif

    #if GL_EXT_multisampled_render_to_texture
        #if !GL_EXT_framebuffer_multisample
            GL_IMPORT(true, PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC, glRenderbufferStorageMultisampleEXT);
        #endif
        GL_IMPORT(true, PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC, glFramebufferTexture2DMultisampleEXT);
    #endif

    #if GL_IMG_multisampled_render_to_texture
        GL_IMPORT(true, PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC, glRenderbufferStorageMultisampleIMG);
        GL_IMPORT(true, PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC, glFramebufferTexture2DMultisampleIMG);
    #endif

    #if GL_EXT_discard_framebuffer
        GL_IMPORT(true, PFNGLDISCARDFRAMEBUFFEREXTPROC, glDiscardFramebufferEXT);
    #endif

    #if GL_EXT_debug_marker
        GL_IMPORT(true, PFNGLPUSHGROUPMARKEREXTPROC, glPushGroupMarkerEXT);
        GL_IMPORT(true, PFNGLPOPGROUPMARKEREXTPROC, glPopGroupMarkerEXT);
    #endif

    #if GL_VERSION_4_3 || GL_ES_VERSION_3_0 || GL_ARB_invalidate_subdata
        GL_IMPORT(true, PFNGLINVALIDATEFRAMEBUFFERPROC, glInvalidateFramebuffer);
    #endif

    #if GL_VERSION_4_2 || GL_ES_VERSION_3_0 || GL_ARB_texture_storage
        GL_IMPORT(true, PFNGLTEXSTORAGE2DPROC, glTexStorage2D);
    #endif

    #if GL_EXT_texture_storage
        GL_IMPORT(true, PFNGLTEXSTORAGE2DEXTPROC, glTexStorage2DEXT);
    #endif

    #if GL_VERSION_3_3 || GL_ES_VERSION_3_0 || GL_ARB_sampler_objects
        GL_IMPORT(true, PFNGLGENSAMPLERSPROC, glGenSamplers);
        GL_IMPORT(true, PFNGLDELETESAMPLERSPROC, glDeleteSamplers);
        GL_IMPORT(true, PFNGLISSAMPLERPROC, glIsSampler);
        GL_IMPORT(true, PFNGLBINDSAMPLERPROC, glBindSampler);
        GL_IMPORT(true, PFNGLSAMPLERPARAMETERIPROC, glSamplerParameteri);
    #endif

    #if GL_VERSION_3_0 || GL_ES_VERSION_3_0
        GL_IMPORT(true, PFNGLGETSTRINGIPROC, glGetStringi);
    #endif
#endif
