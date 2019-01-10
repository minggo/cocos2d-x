#pragma once

#include "Macros.h"
#include "base/CCRef.h"
#include "platform/CCPlatformMacros.h"
#include "Types.h"

#include <string>
#include <unordered_map>

CC_BACKEND_BEGIN

class Program : public Ref
{
public:
    virtual const std::unordered_map<std::string, UniformInfo>& getVertexUniformInfos() const = 0;
    virtual const std::unordered_map<std::string, UniformInfo>& getFragmentUniformInfos() const = 0;
    
    virtual int getVertexUniformLocation(const std::string& uniform) const = 0;
    virtual int getFragmentUniformLocation(const std::string& uniform) const = 0;
};

CC_BACKEND_END
