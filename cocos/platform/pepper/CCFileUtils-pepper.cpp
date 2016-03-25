#include "platform/pepper/CCFileUtils-pepper.h"

#include <stdio.h>

namespace cocos2d 
{

FileUtils* FileUtils::getInstance()
{
    if (s_sharedFileUtils == nullptr)
    {
        s_sharedFileUtils = new FileUtilsPepper();
        if(!s_sharedFileUtils->init())
        {
          delete s_sharedFileUtils;
          s_sharedFileUtils = nullptr;
        }
    }
    return s_sharedFileUtils;
}

FileUtilsPepper::FileUtilsPepper() {}
FileUtilsPepper::~FileUtilsPepper() {}

bool FileUtilsPepper::isFileExistInternal(const std::string& path) const
{
    auto fd = fopen(path.c_str(), "r");
    if (fd) 
    {
        fclose(fd);
        return true;
    }
    else 
    {
        return false;
    }
}

bool FileUtilsPepper::isAbsolutePath(const std::string& path) const
{
    // pepper can run on mac or windows
#ifdef PEPPER_MAC
    return (path[0] == '/');
#else // windows
    if ((path.length() > 2
        && ( (path[0] >= 'a' && path[0] <= 'z') || (path[0] >= 'A' && path[0] <= 'Z') )
        && path[1] == ':') || (path[0] == '/' && path[1] == '/'))
    {
        return true;
    }
    else 
    {
        return false;
    }
    
#endif // end of PEPPER_MAC
}

// Fixme: is it needed?
std::string FileUtilsPepper::getWritablePath() const
{
    return "";
}

} // end of namespace