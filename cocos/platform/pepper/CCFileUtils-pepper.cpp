#include "platform/pepper/CCFileUtils-pepper.h"

namespace cocos2d 
{

FileUtils* FileUtils::getInstance()
{
    if (s_sharedFileUtils == nullptr)
    {
        s_sharedFileUtils = new FileUtilsAndroid();
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
	return true;
}

bool FileUtilsPepper::isAbsolutePath(const std::string& path) const
{

}

} // end of namespace