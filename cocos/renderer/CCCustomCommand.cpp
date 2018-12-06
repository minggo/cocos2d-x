/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include "renderer/CCCustomCommand.h"
#include "renderer/CCTextureAtlas.h"

NS_CC_BEGIN

CustomCommand::CustomCommand()
: func(nullptr)
, _defaultCapacity(29)
{
    _type = RenderCommand::Type::CUSTOM_COMMAND;
    _quads = (V3F_C4B_T2F_Quad*)malloc(_defaultCapacity * sizeof(V3F_C4B_T2F_Quad) );
    memset(_quads, 0, _defaultCapacity * sizeof(V3F_C4B_T2F_Quad) );
}

void CustomCommand::init(float depth, const cocos2d::Mat4 &modelViewTransform, uint32_t flags)
{
    RenderCommand::init(depth, modelViewTransform, flags);
}

void CustomCommand::init(float depth, TextureAtlas *textureAtlas, const cocos2d::Mat4 &modelViewTransform, uint32_t flags)
{
    RenderCommand::init(depth, modelViewTransform, flags);
    _textureAtlas = textureAtlas;

    auto dataLength = sizeof(V3F_C4B_T2F_Quad) * textureAtlas->getTotalQuads();
    memcpy(_quads, textureAtlas->getQuads(), dataLength);
}

void CustomCommand::updateColor(Color4B color4)
{
    auto count = _textureAtlas->getTotalQuads();
    for (int index = 0; index < count; ++index)
    {
        _quads[index].bl.colors = color4;
        _quads[index].br.colors = color4;
        _quads[index].tl.colors = color4;
        _quads[index].tr.colors = color4;
    }
}

void CustomCommand::init(float globalOrder)
{
    _globalOrder = globalOrder;
}

CustomCommand::~CustomCommand()
{
    CC_SAFE_FREE(_quads);
}

void CustomCommand::execute()
{
    if(func)
    {
        func();
    }
}

size_t CustomCommand::copyVertexData(void* out) const
{
    auto dataLength = sizeof(V3F_C4B_T2F_Quad) * getQuadCount();
//    memcpy(out, getQuad(), dataLength);
    memcpy(out, _quads, dataLength);
    return dataLength;
}

V3F_C4B_T2F_Quad* CustomCommand::getQuad() const
{
    return _textureAtlas->getQuads();
}

const unsigned short* CustomCommand::getIndices() const
{
    return _textureAtlas->getIndices();
}

size_t CustomCommand::getQuadCount() const
{
    return _textureAtlas->getTotalQuads();
}

size_t CustomCommand::getIndexCount() const
{
    return getQuadCount() * 6;
}

NS_CC_END
