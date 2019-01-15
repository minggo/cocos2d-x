#include "CCProgramState.h"
#include "renderer/backend/ProgramCache.h"
#include "renderer/backend/Program.h"
#include "renderer/backend/Texture.h"


NS_CC_BEGIN

UniformBuffer::UniformBuffer(backend::UniformInfo _uniformInfo)
: uniformInfo(_uniformInfo)
, dirty(false)
{
    if(uniformInfo.bufferSize)
    {
        data = malloc(uniformInfo.bufferSize);
        if (data)
            memset(data, 0, uniformInfo.bufferSize);
    }
}

UniformBuffer::~UniformBuffer()
{
    if (data)
        free(data);
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& rhs)
{
    if (this != &rhs)
    {
        uniformInfo = rhs.uniformInfo;
        data = rhs.data;
        rhs.data = nullptr;
    }
    
    return *this;
}

TextureInfo::TextureInfo(const std::vector<uint32_t>& _slots, const std::vector<backend::Texture*> _textures)
: slot(_slots)
, textures(_textures)
{
    retainTextures();
}

TextureInfo::~TextureInfo()
{
    releaseTextures();
}

void TextureInfo::retainTextures()
{
    for (auto& texture : textures)
        CC_SAFE_RETAIN(texture);
}

void TextureInfo::releaseTextures()
{
    for (auto& texture : textures)
        CC_SAFE_RELEASE(texture);
}

TextureInfo& TextureInfo::operator=(TextureInfo&& rhs)
{
    if (this != &rhs)
    {
        slot = rhs.slot;
        
        rhs.retainTextures();
        releaseTextures();
        textures = rhs.textures;
        
        //release the textures before cleaning the vertor
        rhs.releaseTextures();
        rhs.textures.clear();
    }
    return *this;
}

ProgramState::ProgramState(const std::string& vertexShader, const std::string& fragmentShader)
{
    _program = backend::ProgramCache::getInstance()->newProgram(vertexShader, fragmentShader);
    CC_SAFE_RETAIN(_program);
    
    createVertexUniformBuffer();
    createFragmentUniformBuffer();
}

ProgramState::~ProgramState()
{
    CC_SAFE_RELEASE(_program);
    
    _vertexUniformInfos.clear();
    _fragmentUniformInfos.clear();
    _vertexTextureInfos.clear();
    _fragmentTextureInfos.clear();
}

void ProgramState::createVertexUniformBuffer()
{
    const auto& vertexUniformInfos = _program->getVertexUniformInfos();
    for(const auto& uniformInfo : vertexUniformInfos)
    {
        if(uniformInfo.second.bufferSize)
            _vertexUniformInfos[uniformInfo.second.location] = uniformInfo.second;
    }
}

void ProgramState::createFragmentUniformBuffer()
{
    const auto& fragmentUniformInfos = _program->getFragmentUniformInfos();
    for(const auto& uniformInfo : fragmentUniformInfos)
    {
        if(uniformInfo.second.bufferSize)
            _fragmentUniformInfos[uniformInfo.second.location] = uniformInfo.second;
    }
}

int ProgramState::getVertexUniformLocation(const std::string& uniform) const
{
    return _program->getVertexUniformLocation(uniform);
}

int ProgramState::getFragmentUniformLocation(const std::string& uniform) const
{
    return _program->getFragmentUniformLocation(uniform);
}

void ProgramState::setVertexUniform(int location, const void* data, uint32_t size)
{
    if(location < 0)
        return;
    
    assert(size <= _vertexUniformInfos[location].uniformInfo.bufferSize);
    memcpy(_vertexUniformInfos[location].data, data, size);
    _vertexUniformInfos[location].dirty = true;
}

void ProgramState::setFragmentUniform(int location, const void* data, uint32_t size)
{
    if(location < 0)
        return;
    
    assert(size <= _fragmentUniformInfos[location].uniformInfo.bufferSize);
    memcpy(_fragmentUniformInfos[location].data, data, size);
    _fragmentUniformInfos[location].dirty = true;
}

void ProgramState::setVertexTexture(int location, uint32_t slot, backend::Texture* texture)
{
    setTexture(location, slot, texture, _vertexTextureInfos);
}

void ProgramState::setFragmentTexture(int location, uint32_t slot, backend::Texture* texture)
{
    setTexture(location, slot, texture, _fragmentTextureInfos);
}

void ProgramState::setVertexTextureArray(int location, const std::vector<uint32_t>& slots, const std::vector<backend::Texture*> textures)
{
    setTextureArray(location, slots, textures, _vertexTextureInfos);
}

void ProgramState::setFragmentTextureArray(int location, const std::vector<uint32_t>& slots, const std::vector<backend::Texture*> textures)
{
    setTextureArray(location, slots, textures, _fragmentTextureInfos);
}

void ProgramState::setTexture(int location, uint32_t slot, backend::Texture* texture, std::unordered_map<int, TextureInfo>& textureInfo)
{
    if(location < 0)
        return;
    
    TextureInfo info;
    info.slot = {slot};
    info.textures = {texture};
    info.retainTextures();
    textureInfo[location] = std::move(info);
}

void ProgramState::setTextureArray(int location, const std::vector<uint32_t>& slots, const std::vector<backend::Texture*> textures, std::unordered_map<int, TextureInfo>& textureInfo)
{
    assert(slots.size() == textures.size());
    TextureInfo info;
    info.slot = slots;
    info.textures = textures;
    info.retainTextures();
    textureInfo[location] = std::move(info);
}

NS_CC_END

