#include "Texture.h"
#include <cassert>
CC_BACKEND_BEGIN

#define byte(n) ((n) * 8)
#define bit(n) (n)
namespace
{
    uint8_t computeBitsPerElement(TextureFormat textureFormat)
    {
        uint8_t ret = 0;
        switch (textureFormat)
        {
            case TextureFormat::R8G8B8A8:
                ret = byte(4);
                break;
            case TextureFormat::R8G8B8:
                ret = byte(3);
                break;
            case TextureFormat::RGBA4444:
                ret = byte(2);
                break;
            case TextureFormat::A8:
                ret = byte(1);
                break;
            case TextureFormat::I8:
                ret = byte(1);
                break;
            case TextureFormat::RGB565:
                ret = byte(2);
                break;
            case TextureFormat::RGB5A1:
                ret = byte(2);
                break;
            case TextureFormat::AI88:
                ret = byte(2);
                break;
            case TextureFormat::ETC1:
                ret = bit(4);
                break;
            case TextureFormat::ATC_RGB:
                ret = bit(4);
                break;
            case TextureFormat::ATC_EXPLICIT_ALPHA:
                ret = byte(1);
                break;
            case TextureFormat::ATC_INTERPOLATED_ALPHA:
                ret = byte(1);
                break;
            case TextureFormat::PVRTC2:
                ret = bit(2);
                break;
            case TextureFormat::PVRTC2A:
                ret = bit(2);
                break;
            case TextureFormat::PVRTC4:
                ret = bit(4);
                break;
            case TextureFormat::PVRTC4A:
                ret = bit(4);
                break;
            case TextureFormat::S3TC_DXT1:
                ret = bit(4);
                break;
            case TextureFormat::S3TC_DXT3:
                ret = byte(1);
                break;
            case TextureFormat::S3TC_DXT5:
                ret = byte(1);
                break;
            default:
                assert(false); //"textureFormat pixel size in bytes not defined!";
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
, _bitsPerElement(computeBitsPerElement(descriptor.textureFormat))
, _isMipmapEnabled(descriptor.samplerDescriptor.mipmapEnabled)
, _textureUsage(descriptor.textureUsage)
, _isCompressed(descriptor.compressed)
{
}

Texture::~Texture()
{}

CC_BACKEND_END
