#include "ProgramMTL.h"
#include "ShaderModuleMTL.h"

CC_BACKEND_BEGIN

ProgramMTL::ProgramMTL(id<MTLDevice> mtlDevice, const std::string& vertexShader, const std::string& fragmentShader)
: Program(vertexShader, fragmentShader)
{
    _vertexShader = new (std::nothrow) ShaderModuleMTL(mtlDevice, backend::ShaderStage::VERTEX, vertexShader);
    if(_vertexShader)
        _vertexShader->autorelease();
    _fragmentShader = new (std::nothrow) ShaderModuleMTL(mtlDevice, backend::ShaderStage::FRAGMENT, fragmentShader);
    if(_fragmentShader)
        _fragmentShader->autorelease();
    CC_SAFE_RETAIN(_vertexShader);
    CC_SAFE_RETAIN(_fragmentShader);
}

ProgramMTL::~ProgramMTL()
{
    CC_SAFE_RELEASE(_vertexShader);
    CC_SAFE_RELEASE(_fragmentShader);
}

int ProgramMTL::getVertexUniformLocation(const std::string& uniform) const
{
    const auto& vsUniforms = _vertexShader->getUniforms();
    const auto& vsIter = vsUniforms.find(uniform);
    if(vsIter != vsUniforms.end())
        return vsIter->second.location;
  
    return -1;
}

int ProgramMTL::getFragmentUniformLocation(const std::string& uniform) const
{
    const auto& fsUniforms = _fragmentShader->getUniforms();
    const auto& fsIter = fsUniforms.find(uniform);
    if(fsIter != fsUniforms.end())
        return fsIter->second.location;
    
    return -1;
}

const std::unordered_map<std::string, UniformInfo>& ProgramMTL::getVertexUniformInfos() const
{
    return _vertexShader->getUniforms();
}

const std::unordered_map<std::string, UniformInfo>& ProgramMTL::getFragmentUniformInfos() const
{
    return _fragmentShader->getUniforms();
}

int ProgramMTL::getMaxVertexLocation() const
{
    return _vertexShader->getMaxLocation();
}

int ProgramMTL::getMaxFragmentLocation() const
{
    return _fragmentShader->getMaxLocation();
}

CC_BACKEND_END
