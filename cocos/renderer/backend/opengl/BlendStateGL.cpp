#include "BlendStateGL.h"
#include "base/ccUtils.h"

CC_BACKEND_BEGIN

void BlendStateGL::reset()
{
    glDisable(GL_BLEND);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

BlendStateGL::BlendStateGL(const BlendDescriptor& descriptor)
: _blendEnabled(descriptor.blendEnabled)
, _rgbBlendOperation(utils::toGLBlendOperation(descriptor.rgbBlendOperation))
, _alphaBlendOperation(utils::toGLBlendOperation(descriptor.alphaBlendOperation))
, _sourceRGBBlendFactor(utils::toGLBlendFactor(descriptor.sourceRGBBlendFactor))
, _destinationRGBBlendFactor(utils::toGLBlendFactor(descriptor.destinationRGBBlendFactor))
, _sourceAlphaBlendFactor(utils::toGLBlendFactor(descriptor.sourceAlphaBlendFactor))
, _destinationAlphaBlendFactor(utils::toGLBlendFactor(descriptor.destinationAlphaBlendFactor))
, _writeMaskRed((uint32_t)descriptor.writeMask & (uint32_t)ColorWriteMask::RED)
, _writeMaskGreen((uint32_t)descriptor.writeMask & (uint32_t)ColorWriteMask::GREEN)
, _writeMaskBlue((uint32_t)descriptor.writeMask & (uint32_t)ColorWriteMask::BLUE)
, _writeMaskAlpha((uint32_t)descriptor.writeMask & (uint32_t)ColorWriteMask::ALPHA)
{
}

void BlendStateGL::apply() const
{
    if (_blendEnabled)
    {
        glEnable(GL_BLEND);
        glBlendEquationSeparate(_rgbBlendOperation, _alphaBlendOperation);
        glBlendFuncSeparate(_sourceRGBBlendFactor,
                            _destinationRGBBlendFactor,
                            _sourceAlphaBlendFactor,
                            _destinationAlphaBlendFactor);
    }
    else
        glDisable(GL_BLEND);
    
    glColorMask(_writeMaskRed, _writeMaskGreen, _writeMaskBlue, _writeMaskAlpha);
}

CC_BACKEND_END
