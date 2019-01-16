/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
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

#include "2d/CCAtlasNode.h"
#include "renderer/CCTextureAtlas.h"
#include "base/CCDirector.h"
#include "renderer/CCTextureCache.h"
#include "base/ccUtils.h"
#include "renderer/ccShaders.h"
#include "renderer/CCRenderer.h"
#include "renderer/backend/ProgramState.h"

NS_CC_BEGIN

// implementation AtlasNode

// AtlasNode - Creation & Init

AtlasNode::AtlasNode()
{
    auto& pipelineDescriptor = _quadCommand.getPipelineDescriptor();
    _programState = new (std::nothrow) ProgramState(positionTextureColor_vert, positionTextureColor_frag);
    pipelineDescriptor.programState = _programState;
    _mvpMatrixLocation = pipelineDescriptor.programState->getUniformLocation("u_MVPMatrix");
    _textureLocation = pipelineDescriptor.programState->getUniformLocation("u_texture");
  
#define VERTEX_POSITION_SIZE 3
#define VERTEX_TEXCOORD_SIZE 2
#define VERTEX_COLOR_SIZE 4
    uint32_t colorOffset = (VERTEX_POSITION_SIZE)*sizeof(float);
    uint32_t texcoordOffset = VERTEX_POSITION_SIZE*sizeof(float) + VERTEX_COLOR_SIZE*sizeof(unsigned char);
    uint32_t totalSize = (VERTEX_POSITION_SIZE+VERTEX_TEXCOORD_SIZE)*sizeof(float) + VERTEX_COLOR_SIZE*sizeof(unsigned char);
    
    //set vertexLayout according to V3F_C4B_T2F structure
    backend::VertexLayout vertexLayout;
    vertexLayout.setAtrribute("a_position", 0, backend::VertexFormat::FLOAT_R32G32B32, 0, false);
    vertexLayout.setAtrribute("a_texCoord", 1, backend::VertexFormat::FLOAT_R32G32, texcoordOffset, false);
    vertexLayout.setAtrribute("a_color", 2, backend::VertexFormat::UBYTE_R8G8B8A8, colorOffset, true);
    
    vertexLayout.setLayout(totalSize, backend::VertexStepMode::VERTEX);
    pipelineDescriptor.vertexLayout = vertexLayout;
}

AtlasNode::~AtlasNode()
{
    CC_SAFE_RELEASE(_textureAtlas);
    CC_SAFE_RELEASE(_programState);
}

AtlasNode * AtlasNode::create(const std::string& tile, int tileWidth, int tileHeight, int itemsToRender)
{
    AtlasNode * ret = new (std::nothrow) AtlasNode();
    if (ret->initWithTileFile(tile, tileWidth, tileHeight, itemsToRender))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool AtlasNode::initWithTileFile(const std::string& tile, int tileWidth, int tileHeight, int itemsToRender)
{
    CCASSERT(tile.size() > 0, "file size should not be empty");
    Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(tile);
    return initWithTexture(texture, tileWidth, tileHeight, itemsToRender);
}

bool AtlasNode::initWithTexture(Texture2D* texture, int tileWidth, int tileHeight, int itemsToRender)
{
    _itemWidth  = tileWidth;
    _itemHeight = tileHeight;

    _colorUnmodified = Color3B::WHITE;
    _isOpacityModifyRGB = true;

    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;

    _textureAtlas = new (std::nothrow) TextureAtlas();

    if (! _textureAtlas)
    {
        CCLOG("cocos2d: Could not initialize AtlasNode. Invalid Texture.");
        return false;
    }
    
    _textureAtlas->initWithTexture(texture, itemsToRender);

    this->updateBlendFunc();
    this->updateOpacityModifyRGB();

    this->calculateMaxItems();

    _quadsToDraw = itemsToRender;

    return true;
}


// AtlasNode - Atlas generation

void AtlasNode::calculateMaxItems()
{
    Size s = _textureAtlas->getTexture()->getContentSize();
    
    if (_ignoreContentScaleFactor)
    {
        s = _textureAtlas->getTexture()->getContentSizeInPixels();
    }
    
    _itemsPerColumn = (int)(s.height / _itemHeight);
    _itemsPerRow = (int)(s.width / _itemWidth);
}

void AtlasNode::updateAtlasValues()
{
    CCASSERT(false, "CCAtlasNode:Abstract updateAtlasValue not overridden");
}

// AtlasNode - draw
void AtlasNode::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{    
    if( _textureAtlas->getTotalQuads() == 0 )
        return;
    
    auto programState = _quadCommand.getPipelineDescriptor().programState;
    programState->setTexture(_textureLocation, 0, _textureAtlas->getTexture()->getBackendTexture());
    
    const auto& projectionMat = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    programState->setUniform(_mvpMatrixLocation, projectionMat.m, sizeof(projectionMat.m));
    
    _quadCommand.init(_globalZOrder, _textureAtlas->getTexture(), _blendFunc, _textureAtlas->getQuads(), _quadsToDraw, transform, flags);
    renderer->addCommand(&_quadCommand);
}

// AtlasNode - RGBA protocol

const Color3B& AtlasNode::getColor() const
{
    if(_isOpacityModifyRGB)
    {
        return _colorUnmodified;
    }
    return Node::getColor();
}

void AtlasNode::setColor(const Color3B& color3)
{
    Color3B tmp = color3;
    _colorUnmodified = color3;

    if( _isOpacityModifyRGB )
    {
        tmp.r = tmp.r * _displayedOpacity/255;
        tmp.g = tmp.g * _displayedOpacity/255;
        tmp.b = tmp.b * _displayedOpacity/255;
    }
    Node::setColor(tmp);
}

void AtlasNode::setOpacity(GLubyte opacity)
{
    Node::setOpacity(opacity);

    // special opacity for premultiplied textures
    if( _isOpacityModifyRGB )
        this->setColor(_colorUnmodified);
}

void AtlasNode::setOpacityModifyRGB(bool value)
{
    Color3B oldColor = this->getColor();
    _isOpacityModifyRGB = value;
    this->setColor(oldColor);
}

bool AtlasNode::isOpacityModifyRGB() const
{
    return _isOpacityModifyRGB;
}

void AtlasNode::updateOpacityModifyRGB()
{
    _isOpacityModifyRGB = _textureAtlas->getTexture()->hasPremultipliedAlpha();
}

void AtlasNode::setIgnoreContentScaleFactor(bool ignoreContentScaleFactor)
{
    _ignoreContentScaleFactor = ignoreContentScaleFactor;
}

// AtlasNode - CocosNodeTexture protocol

const BlendFunc& AtlasNode::getBlendFunc() const
{
    return _blendFunc;
}

void AtlasNode::setBlendFunc(const BlendFunc &blendFunc)
{
    _blendFunc = blendFunc;
}

void AtlasNode::updateBlendFunc()
{
    if( ! _textureAtlas->getTexture()->hasPremultipliedAlpha() )
    {
        _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
        setOpacityModifyRGB(false);
    }
    else
    {
        _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
        setOpacityModifyRGB(true);
    }
}

void AtlasNode::setTexture(Texture2D *texture)
{
    _textureAtlas->setTexture(texture);
    this->updateBlendFunc();
    this->updateOpacityModifyRGB();
}

Texture2D * AtlasNode::getTexture() const
{
    return _textureAtlas->getTexture();
}

void AtlasNode::setTextureAtlas(TextureAtlas* textureAtlas)
{
    CC_SAFE_RETAIN(textureAtlas);
    CC_SAFE_RELEASE(_textureAtlas);
    _textureAtlas = textureAtlas;
}

TextureAtlas * AtlasNode::getTextureAtlas() const
{
    return _textureAtlas;
}

size_t AtlasNode::getQuadsToDraw() const
{
    return _quadsToDraw;
}

void AtlasNode::setQuadsToDraw(ssize_t quadsToDraw)
{
    _quadsToDraw = quadsToDraw;
}

NS_CC_END
