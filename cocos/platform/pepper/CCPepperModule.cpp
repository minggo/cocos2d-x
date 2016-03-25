#include "platform/pepper/CCPepperModule.h"

#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"

#include "ppapi/lib/gl/gles2/gl2ext_ppapi.h"

#include "2d/CCSprite.h"

#include "base/CCDirector.h"


CocosInstance::CocosInstance(PP_Instance instance)
: pp::Instance(instance)
, _view(nullptr)
, _callbackFactory(this)
{}

bool CocosInstance::Init(uint32_t argc,  const char* argn[], const char* argv[])
{
	return true;
}

void CocosInstance::DidChangeView(const pp::View& view) 
{
	// Pepper specifies dimensions in DIPs (device-independent pixels). To
    // generate a context that is at device-pixel resolution on HiDPI devices,
    // scale the dimensions by view.GetDeviceScale().
    // int32_t newWidth = view.GetRect().width() * view.GetDeviceScale();
    // int32_t newHeight = view.GetRect().height() * view.GetDeviceScale();

    int32_t newWidth = view.GetRect().width();
    int32_t newHeight = view.GetRect().height();

    if (_context.is_null())
    {
    	if (!InitGL(newWidth, newHeight))
    		return;

    	_view = cocos2d::GLViewImpl::create(this, newWidth, newHeight);
    	auto director = cocos2d::Director::getInstance();
	    director->setOpenGLView(_view);

        // create a scene. it's an autorelease object
        auto scene = cocos2d::Scene::create();
        auto sprite = cocos2d::Sprite::create("/Users/minggo/SourceCode/test/electron/electron-pepper/HelloWorld.png");
        sprite->setPosition(newWidth/2, newHeight/2);
        scene->addChild(sprite);
        director->runWithScene(scene);
        director->mainLoop();
    }
    else 
    {
    	// Resize the buffers to the new size of the module.
		int32_t result = _context.ResizeBuffers(newWidth, newHeight);
		if (result < 0) {
			fprintf(stderr,
			        "Unable to resize buffers to %d x %d!\n",
			        newWidth,
			        newHeight);
			return;
	    }
    }

    glViewport(0, 0, newWidth, newHeight);
}

// handle messages from JS
// TODO
void CocosInstance::HandleMessage(const pp::Var& message) 
{

}

void CocosInstance::SwapBuffers()
{
	_context.SwapBuffers(_callbackFactory.NewCallback(&CocosInstance::MainLoop));
}


bool CocosInstance::InitGL(int32_t width, int32_t height) 
{
    if (!glInitializePPAPI(pp::Module::Get()->get_browser_interface())) 
    {
        fprintf(stderr, "Unable to initialize GL PPAPI!\n");
        return false;
    }

    const int32_t attribList[] = 
    {
        PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 8,
        PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 24,
        PP_GRAPHICS3DATTRIB_WIDTH, width,
        PP_GRAPHICS3DATTRIB_HEIGHT, height,
        PP_GRAPHICS3DATTRIB_NONE
    };

    _context = pp::Graphics3D(this, attribList);
    if (!BindGraphics(_context)) 
    {
        fprintf(stderr, "Unable to bind 3d context!\n");
        _context = pp::Graphics3D();
        glSetCurrentContextPPAPI(0);
        return false;
    }

    glSetCurrentContextPPAPI(_context.pp_resource());

    return true;
}

void CocosInstance::MainLoop(int32_t) 
{
	cocos2d::Director::getInstance()->mainLoop();
}


class CocosModule : public pp::Module 
{
public:
	CocosModule() :  pp::Module() {}
	virtual ~CocosModule() {}

	virtual pp::Instance* CreateInstance(PP_Instance instance) 
	{
		return new CocosInstance(instance);
	}
};

namespace pp 
{

Module* CreateModule() 
{
    return new CocosModule();	

}

} // end of pp