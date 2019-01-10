#pragma once

#include "Macros.h"
#include "base/CCRef.h"
#include "Program.h"
#include "Types.h"

#include <string>
#include <unordered_map>
#include <vector>

CC_BACKEND_BEGIN

class Texture;
class Sampler;
class Program;

class BindGroup : public cocos2d::Ref
{
public:
    struct UniformBuffer
    {
        UniformBuffer(UniformInfo _uniformInfo);
        UniformBuffer() = default;
        ~UniformBuffer();
        UniformBuffer& operator =(UniformBuffer&& rhs);

        UniformInfo uniformInfo;
        bool dirty = false;
        void* data = nullptr;
    };
    
    struct TextureInfo
    {
        ~TextureInfo();
        
        void retainTextures();
        void releaseTextures();

        std::vector<uint32_t> slot;
        std::vector<Texture*> textures;
    };
    
    ~BindGroup();
    
    //TODO coulsonwang
    void setTexture(const std::string& name, uint32_t index, Texture* texture);
    void setUniform(const std::string& name, const void* data, uint32_t size);

    
    //set program
    void newProgram(const std::string& vertexShader, const std::string& fragmentShader);
    Program* getProgram() const { return _program; }
    
    //set uniforms
    void setVertexUniform(int location, const void* data, uint32_t size);
    void setFragmentUniform(int location, const void* data, uint32_t size);
    
    int getVertexUniformLocation(const std::string& uniform) const;
    int getFragmentUniformLocation(const std::string& uniform) const;
    inline const std::unordered_map<int, UniformBuffer>& getVertexUniformInfos() const { return _vertexUniformInfos; }
    inline const std::unordered_map<int, UniformBuffer>& getFragmentUniformInfos() const { return _fragmentUniformInfos; }
    
    //set textures
    void setVertexTexture(int location, uint32_t slot, Texture* texture);
    void setFragmentTexture(int location, uint32_t slot, Texture* texture);
    void setVertexTextureArray(int location, const std::vector<uint32_t>& slots, const std::vector<Texture*> textures);
    void setFragmentTextureArray(int location, const std::vector<uint32_t>& slots, const std::vector<Texture*> textures);
    
    inline const std::unordered_map<int, TextureInfo>& getVertexTextureInfos() const { return _vertexTextureInfos; }
    inline const std::unordered_map<int, TextureInfo>& getFragmentTextureInfos() const { return _fragmentTextureInfos; }

private:
    void createVertexUniformBuffer();
    void createFragmentUniformBuffer();
    
    void setTexture(int location, uint32_t slot, Texture* texture, std::unordered_map<int, TextureInfo>& textureInfo);
    void setTextureArray(int location, const std::vector<uint32_t>& slots, const std::vector<Texture*> textures, std::unordered_map<int, TextureInfo>& textureInfo);

    Program* _program = nullptr;
    
    std::unordered_map<int, UniformBuffer> _vertexUniformInfos;
    std::unordered_map<int, UniformBuffer> _fragmentUniformInfos;
    
    std::unordered_map<int, TextureInfo> _vertexTextureInfos;
    std::unordered_map<int, TextureInfo> _fragmentTextureInfos;
};

CC_BACKEND_END
