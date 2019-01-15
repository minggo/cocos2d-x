#pragma once

#include "../Macros.h"
#include "../Types.h"
#include "../RenderPipelineDescriptor.h"
#include "base/CCRef.h"
#include "platform/CCGL.h"
#include "../Program.h"

#include <string>
#include <unordered_map>

CC_BACKEND_BEGIN

class ShaderModuleGL;

struct AttributeInfo
{
    uint32_t location = 0;
    uint32_t size = 0;
    GLenum type = GL_BYTE;
    GLsizei stride = 0;
    uint32_t offset = 0;
    GLboolean needToBeNormallized = GL_FALSE;
};

class ProgramGL : public Program
{
public:
    typedef std::vector<AttributeInfo> VertexAttributeArray;
    
    ProgramGL(const std::string& vertexShader, const std::string& fragmentShader);
    ~ProgramGL();
    
    inline const std::vector<VertexAttributeArray>& getAttributeInfos() const { return _attributeInfos; }
    inline GLuint getHandler() const { return _program; }
    void computeAttributeInfos(const RenderPipelineDescriptor& descriptor);

    virtual const std::unordered_map<std::string, UniformInfo>& getVertexUniformInfos() const override;
    virtual const std::unordered_map<std::string, UniformInfo>& getFragmentUniformInfos() const override;

    virtual int getVertexUniformLocation(const std::string& uniform) const override;
    virtual int getFragmentUniformLocation(const std::string& uniform) const override;
    
    virtual int getMaxVertexLocation() const override;
    virtual int getMaxFragmentLocation() const override;

private:
    void compileProgram();
    bool getAttributeLocation(const std::string& attributeName, uint32_t& location) const;
    void computeUniformInfos();
    
    GLuint _program = 0;
    ShaderModuleGL* _vertexShaderModule = nullptr;
    ShaderModuleGL* _fragmentShaderModule = nullptr;
    
    std::vector<VertexAttributeArray> _attributeInfos;
    std::unordered_map<std::string, UniformInfo> _uniformInfos;
    
};

CC_BACKEND_END
