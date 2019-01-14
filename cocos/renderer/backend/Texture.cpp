#include "Texture.h"

CC_BACKEND_BEGIN

namespace
{
    uint8_t computeBytesPerElement(TextureFormat textureFormat)
    {
        uint8_t ret = 0;
        switch (textureFormat)
        {
            case TextureFormat::R8G8B8A8:
                ret = 4;
                break;
            case TextureFormat::R8G8B8:
                ret = 3;
                break;
            case TextureFormat::RGBA4444:
                ret = 2;
                break;
            case TextureFormat::A8:
                ret = 1;
                break;
            case TextureFormat::I8:
                ret = 1;
                break;
            case TextureFormat::RGB565:
                ret = 2;
                break;
            case TextureFormat::RGB5A1:
                ret = 2;
                break;
            case TextureFormat::AI88:
                ret = 2;
                break;
            default:
                CC_ASSERT(false, "textureFormat pixel size in bytes not defined!");
                break;
        }
        return ret;
    }
}

Texture::Texture(const TextureDescriptor& descriptor)
: _width(descriptor.width)
, _height(descriptor.height)
, _textureType(descriptor.textureType)
, _textureFormat(descriptor.textureFormat)
, _bytesPerElement(computeBytesPerElement(descriptor.textureFormat))
, _isMipmapEnabled(descriptor.samplerDescriptor.mipmapEnabled)
, _textureUsage(descriptor.textureUsage)
, _isCompressed(descriptor.compressed)
{
}

Texture::~Texture()
{}

CC_BACKEND_END
