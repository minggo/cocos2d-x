#include "BindGroup.h"
#include "Texture.h"
#include "Device.h"
#include "ProgramCache.h"

CC_BACKEND_BEGIN

//needed removed
void BindGroup::setTexture(const std::string& name, uint32_t index, Texture* texture)
{
    printf("-----------------");
}
void BindGroup::setUniform(const std::string& name, const void* data, uint32_t size)
{
    printf("-----------------");
}


BindGroup::UniformBuffer::UniformBuffer(UniformInfo _uniformInfo)
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

BindGroup::UniformBuffer::~UniformBuffer()
{
    if (data)
        free(data);
}

BindGroup::UniformBuffer& BindGroup::UniformBuffer::operator=(UniformBuffer&& rhs)
{
    if (this != &rhs)
    {
        uniformInfo = rhs.uniformInfo;
        data = rhs.data;
        rhs.data = nullptr;
    }
    
    return *this;
}

BindGroup::TextureInfo::~TextureInfo()
{
    releaseTextures();
}

void BindGroup::TextureInfo::retainTextures()
{
    for (auto& texture : textures)
        CC_SAFE_RETAIN(texture);
}

void BindGroup::TextureInfo::releaseTextures()
{
    for (auto& texture : textures)
        CC_SAFE_RELEASE(texture);
}

BindGroup::~BindGroup()
{
    _vertexUniformInfos.clear();
    _fragmentUniformInfos.clear();
    _vertexTextureInfos.clear();
    _fragmentTextureInfos.clear();
}

void BindGroup::newProgram(const std::string& vertexShader, const std::string& fragmentShader)
{
    _program = ProgramCache::getInstance()->newProgram(vertexShader, fragmentShader);
    createVertexUniformBuffer();
    createFragmentUniformBuffer();
}

void BindGroup::createVertexUniformBuffer()
{
    const auto& vertexUniformInfos = _program->getVertexUniformInfos();
    for(const auto& uniformInfo : vertexUniformInfos)
    {
        if(uniformInfo.second.bufferSize)
            _vertexUniformInfos[uniformInfo.second.location] = uniformInfo.second;
    }
}

void BindGroup::createFragmentUniformBuffer()
{
    const auto& fragmentUniformInfos = _program->getFragmentUniformInfos();
    for(const auto& uniformInfo : fragmentUniformInfos)
    {
        if(uniformInfo.second.bufferSize)
            _fragmentUniformInfos[uniformInfo.second.location] = uniformInfo.second;
    }
}

int BindGroup::getVertexUniformLocation(const std::string& uniform) const
{
    return _program->getVertexUniformLocation(uniform);
}

int BindGroup::getFragmentUniformLocation(const std::string& uniform) const
{
    return _program->getFragmentUniformLocation(uniform);
}

void BindGroup::setVertexUniform(int location, const void* data, uint32_t size)
{
    if(location < 0)
        return;
    
    assert(size <= _vertexUniformInfos[location].uniformInfo.bufferSize);
    memcpy(_vertexUniformInfos[location].data, data, size);
    _vertexUniformInfos[location].dirty = true;
}

void BindGroup::setFragmentUniform(int location, const void* data, uint32_t size)
{
    if(location < 0)
        return;
    
    assert(size <= _fragmentUniformInfos[location].uniformInfo.bufferSize);
    memcpy(_fragmentUniformInfos[location].data, data, size);
    _fragmentUniformInfos[location].dirty = true;
}

void BindGroup::setVertexTexture(int location, uint32_t slot, Texture* texture)
{
    setTexture(location, slot, texture, _vertexTextureInfos);
}

void BindGroup::setFragmentTexture(int location, uint32_t slot, Texture* texture)
{
    setTexture(location, slot, texture, _fragmentTextureInfos);
}

void BindGroup::setVertexTextureArray(int location, const std::vector<uint32_t>& slots, const std::vector<Texture*> textures)
{
    setTextureArray(location, slots, textures, _vertexTextureInfos);
}

void BindGroup::setFragmentTextureArray(int location, const std::vector<uint32_t>& slots, const std::vector<Texture*> textures)
{
    setTextureArray(location, slots, textures, _fragmentTextureInfos);
}

void BindGroup::setTexture(int location, uint32_t slot, Texture* texture, std::unordered_map<int, TextureInfo>& textureInfo)
{
    if(location < 0)
        return;
    
    TextureInfo info;
    info.slot = {slot};
    info.textures = {texture};
    info.retainTextures();
    textureInfo[location] = info;
}

void BindGroup::setTextureArray(int location, const std::vector<uint32_t>& slots, const std::vector<Texture*> textures, std::unordered_map<int, TextureInfo>& textureInfo)
{
    assert(slots.size() == textures.size());
    TextureInfo info;
    info.slot = slots;
    info.textures = textures;
    info.retainTextures();
    textureInfo[location] = info;
}

CC_BACKEND_END
