#include "platform/pepper/CCApplication-pepper.h"

namespace cocos2d
{

Application * Application::_sharedApplication = 0;

//---------------------------------------------- static functions -----------------------------------------------

Application* Application::getInstance()
{
    if (!Application::_sharedApplication)
    {
        Application::_sharedApplication = new Application();
    }

    return Application::_sharedApplication;
}

Application* Application::sharedApplication()
{
    return Application::getInstance();
}

//----------------------------------------------- member functions -------------------------------------------------


Application::Application() {}
Application::~Application() {}
void Application::setAnimationInterval(float interval) {}
LanguageType Application::getCurrentLanguage() { return LanguageType::ENGLISH; }
const char * Application::getCurrentLanguageCode() { return ""; }  
Application::Platform Application::getTargetPlatform() { return Platform::OS_PEPPER; }  
std::string Application::getVersion() { return ""; }
bool Application::openURL(const std::string &url) { return false; }
void Application::applicationScreenSizeChanged(int newWidth, int newHeight) {}
bool Application::applicationDidFinishLaunching() { return true; }
void Application::applicationDidEnterBackground() {}
void Application::applicationWillEnterForeground() {}

} // end of namespace cocos2d
