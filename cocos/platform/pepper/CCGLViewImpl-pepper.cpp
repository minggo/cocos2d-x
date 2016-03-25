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

#include "platform/pepper/CCGLViewImpl-pepper.h"

#include <stdio.h>
#include <GLES2/gl2.h>

#include "platform/pepper/CCPepperModule.h"


namespace cocos2d
{


GLViewImpl* GLViewImpl::create(void* instance, int width, int height)
{
    auto ret = new GLViewImpl(instance);
    if (ret) 
    {
        ret->setFrameSize(width, height);
        ret->autorelease();
    }

    return ret;
}


GLViewImpl::GLViewImpl(const void* instance)
: _cocosInstance(const_cast<void*>(instance))
{
}

GLViewImpl::~GLViewImpl()
{
}
 
void GLViewImpl::end()
{

}

bool GLViewImpl::isOpenGLReady()
{
    return true;
}


void GLViewImpl::swapBuffers()
{
    static_cast<CocosInstance*>(_cocosInstance)->SwapBuffers();   
}

void GLViewImpl::setIMEKeyboardState(bool open)
{
}

}

