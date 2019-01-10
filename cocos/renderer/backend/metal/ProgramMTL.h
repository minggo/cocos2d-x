#pragma once

#include "../Program.h"
#import <Metal/Metal.h>

CC_BACKEND_BEGIN

class ShaderModuleMTL;

class ProgramMTL : public Program
{
public:
    ProgramMTL(id<MTLDevice> mtlDevice, const std::string& vertexShader, const std::string& fragmentShader);
    virtual ~ProgramMTL();

    virtual const std::unordered_map<std::string, UniformInfo>& getVertexUniformInfos() const override;
    virtual const std::unordered_map<std::string, UniformInfo>& getFragmentUniformInfos() const override;
    
    virtual int getVertexUniformLocation(const std::string& uniform) const override;
    virtual int getFragmentUniformLocation(const std::string& uniform) const override;
    
    virtual ShaderModuleMTL* getVertexShader() const { return _vertexShader; }
    virtual ShaderModuleMTL* getFragmentShader() const { return _fragmentShader; }

private:
    ShaderModuleMTL* _vertexShader = nullptr;
    ShaderModuleMTL* _fragmentShader = nullptr;
};

CC_BACKEND_END
