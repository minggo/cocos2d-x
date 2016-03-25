/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#pragma once

#define GL_GLEXT_PROTOTYPES

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "ppapi/lib/gl/gles2/gl2ext_ppapi.h"


#define glGenVertexArrays glGenVertexArraysOES
#define glDeleteVertexArrays glDeleteVertexArraysOES
#define glBindVertexArray glBindVertexArrayOES
#define glIsVertexArray glIsVertexArrayOES
#define glClearDepth glClearDepthf
#define glMapBuffer glMapBufferImpl
#define glUnmapBuffer glUnmapBufferImpl

#define GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
#define GL_BGRA GL_BGRA_EXT

// just make compiler happy, because pepper doesn't support these two functions
// and they are not invoked

GLvoid* glMapBufferImpl(GLenum target, GLenum access) 
{
	return nullptr; 
}
GLboolean glUnmapBufferImpl(GLenum target) { return false; }
