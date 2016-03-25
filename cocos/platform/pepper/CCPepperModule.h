#pragma once

#include "ppapi/cpp/instance.h"
#include "ppapi/utility/completion_callback_factory.h"
#include "ppapi/cpp/graphics_3d.h"

#include "platform/pepper/CCGLViewImpl-pepper.h"

class CocosInstance : public pp::Instance
{
public:
	explicit CocosInstance(PP_Instance instance);

	virtual bool Init(uint32_t argc,  const char* argn[], const char* argv[]) override;
	virtual void HandleMessage(const pp::Var& message) override;
	virtual void DidChangeView(const pp::View& view) override;

	void SwapBuffers();

private:

	bool InitGL(int32_t width, int32_t height);

	void MainLoop(int32_t);

    cocos2d::GLViewImpl* _view;
    pp::Graphics3D _context;
    pp::CompletionCallbackFactory<CocosInstance> _callbackFactory;
};
