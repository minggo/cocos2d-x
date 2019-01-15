#pragma once

#include <vector>
#include <string>

#include "platform/CCPlatformMacros.h"
#include "base/CCRef.h"
#include "renderer/backend/Types.h"

NS_CC_BEGIN

namespace backend {
    class Program;
    class Texture;
}

struct UniformBuffer
{
    UniformBuffer(backend::UniformInfo _uniformInfo);
    UniformBuffer() = default;
    ~UniformBuffer();
    UniformBuffer& operator =(UniformBuffer&& rhs);
    
    backend::UniformInfo uniformInfo;
    bool dirty = false;
    void* data = nullptr;
};

struct TextureInfo
{
    TextureInfo(const std::vector<uint32_t>& _slots, const std::vector<backend::Texture*> _textures);
    TextureInfo() = default;
    ~TextureInfo();
    TextureInfo& operator=(TextureInfo&& rhs);
    
    void retainTextures();
    void releaseTextures();
    
    std::vector<uint32_t> slot;
    std::vector<backend::Texture*> textures;
};

class ProgramState : public Ref
{
public:
    ProgramState(const std::string& vertexShader, const std::string& fragmentShader);
    virtual ~ProgramState();
    
    //get program
    backend::Program* getProgram() const { return _program; }
    
    //get or set uniforms
    int getVertexUniformLocation(const std::string& uniform) const;
    int getFragmentUniformLocation(const std::string& uniform) const;
    
    void setVertexUniform(int location, const void* data, uint32_t size);
    void setFragmentUniform(int location, const void* data, uint32_t size);
    
    inline const std::unordered_map<int, UniformBuffer>& getVertexUniformInfos() const { return _vertexUniformInfos; }
    inline const std::unordered_map<int, UniformBuffer>& getFragmentUniformInfos() const { return _fragmentUniformInfos; }
    
    //set textures
    void setVertexTexture(int location, uint32_t slot, backend::Texture* texture);
    void setFragmentTexture(int location, uint32_t slot, backend::Texture* texture);
    void setVertexTextureArray(int location, const std::vector<uint32_t>& slots, const std::vector<backend::Texture*> textures);
    void setFragmentTextureArray(int location, const std::vector<uint32_t>& slots, const std::vector<backend::Texture*> textures);
    
    inline const std::unordered_map<int, TextureInfo>& getVertexTextureInfos() const { return _vertexTextureInfos; }
    inline const std::unordered_map<int, TextureInfo>& getFragmentTextureInfos() const { return _fragmentTextureInfos; }
    
protected:
    void createVertexUniformBuffer();
    void createFragmentUniformBuffer();
    void setTexture(int location, uint32_t slot, backend::Texture* texture, std::unordered_map<int, TextureInfo>& textureInfo);
    void setTextureArray(int location, const std::vector<uint32_t>& slots, const std::vector<backend::Texture*> textures, std::unordered_map<int, TextureInfo>& textureInfo);
    
    backend::Program* _program = nullptr;
    std::unordered_map<int, UniformBuffer> _vertexUniformInfos;
    std::unordered_map<int, UniformBuffer> _fragmentUniformInfos;
    
    std::unordered_map<int, TextureInfo> _vertexTextureInfos;
    std::unordered_map<int, TextureInfo> _fragmentTextureInfos;
    
};

NS_CC_END
