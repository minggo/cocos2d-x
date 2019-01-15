#include "CCPipelineDescriptor.h"

NS_CC_BEGIN

PipelineDescriptor::~PipelineDescriptor()
{
    if(programState)
        programState->release();
}

void PipelineDescriptor::createProgramState(const std::string& vertexShader, const std::string& fragmentShader)
{
     if(programState)
         programState->release();
    
     programState = new (std::nothrow) ProgramState(vertexShader, fragmentShader);
}

NS_CC_END
