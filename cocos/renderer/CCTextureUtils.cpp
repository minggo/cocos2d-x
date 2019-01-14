#include "CCTextureUtils.h"


NS_CC_BEGIN

namespace backend { 

//////////////////////////////////////////////////////////////////////////
//convertor function

// IIIIIIII -> RRRRRRRRGGGGGGGGGBBBBBBBB
void PixelFormatUtils::convertI8ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    for (ssize_t i = 0; i < dataLen; ++i)
    {
        *outData++ = data[i];     //R
        *outData++ = data[i];     //G
        *outData++ = data[i];     //B
    }
}

// IIIIIIIIAAAAAAAA -> RRRRRRRRGGGGGGGGBBBBBBBB
void PixelFormatUtils::convertAI88ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
    {
        *outData++ = data[i];     //R
        *outData++ = data[i];     //G
        *outData++ = data[i];     //B
    }
}

// IIIIIIII -> RRRRRRRRGGGGGGGGGBBBBBBBBAAAAAAAA
void PixelFormatUtils::convertI8ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    for (ssize_t i = 0; i < dataLen; ++i)
    {
        *outData++ = data[i];     //R
        *outData++ = data[i];     //G
        *outData++ = data[i];     //B
        *outData++ = 0xFF;        //A
    }
}

// IIIIIIIIAAAAAAAA -> RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA
void PixelFormatUtils::convertAI88ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
    {
        *outData++ = data[i];     //R
        *outData++ = data[i];     //G
        *outData++ = data[i];     //B
        *outData++ = data[i + 1]; //A
    }
}

// IIIIIIII -> RRRRRGGGGGGBBBBB
void PixelFormatUtils::convertI8ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    unsigned short* out16 = (unsigned short*)outData;
    for (int i = 0; i < dataLen; ++i)
    {
        *out16++ = (data[i] & 0x00F8) << 8    //R
            | (data[i] & 0x00FC) << 3         //G
            | (data[i] & 0x00F8) >> 3;        //B
    }
}

// IIIIIIIIAAAAAAAA -> RRRRRGGGGGGBBBBB
void PixelFormatUtils::convertAI88ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    unsigned short* out16 = (unsigned short*)outData;
    for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
    {
        *out16++ = (data[i] & 0x00F8) << 8    //R
            | (data[i] & 0x00FC) << 3         //G
            | (data[i] & 0x00F8) >> 3;        //B
    }
}

// IIIIIIII -> RRRRGGGGBBBBAAAA
void PixelFormatUtils::convertI8ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    unsigned short* out16 = (unsigned short*)outData;
    for (ssize_t i = 0; i < dataLen; ++i)
    {
        *out16++ = (data[i] & 0x00F0) << 8    //R
            | (data[i] & 0x00F0) << 4             //G
            | (data[i] & 0x00F0)                  //B
            | 0x000F;                             //A
    }
}

// IIIIIIIIAAAAAAAA -> RRRRGGGGBBBBAAAA
void PixelFormatUtils::convertAI88ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    unsigned short* out16 = (unsigned short*)outData;
    for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
    {
        *out16++ = (data[i] & 0x00F0) << 8    //R
            | (data[i] & 0x00F0) << 4             //G
            | (data[i] & 0x00F0)                  //B
            | (data[i + 1] & 0x00F0) >> 4;          //A
    }
}

// IIIIIIII -> RRRRRGGGGGBBBBBA
void PixelFormatUtils::convertI8ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    unsigned short* out16 = (unsigned short*)outData;
    for (int i = 0; i < dataLen; ++i)
    {
        *out16++ = (data[i] & 0x00F8) << 8    //R
            | (data[i] & 0x00F8) << 3         //G
            | (data[i] & 0x00F8) >> 2         //B
            | 0x0001;                         //A
    }
}

// IIIIIIIIAAAAAAAA -> RRRRRGGGGGBBBBBA
void PixelFormatUtils::convertAI88ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    unsigned short* out16 = (unsigned short*)outData;
    for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
    {
        *out16++ = (data[i] & 0x00F8) << 8    //R
            | (data[i] & 0x00F8) << 3         //G
            | (data[i] & 0x00F8) >> 2         //B
            | (data[i + 1] & 0x0080) >> 7;    //A
    }
}

// IIIIIIII -> IIIIIIIIAAAAAAAA
void PixelFormatUtils::convertI8ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    unsigned short* out16 = (unsigned short*)outData;
    for (ssize_t i = 0; i < dataLen; ++i)
    {
        *out16++ = 0xFF00     //A
            | data[i];            //I
    }
}

// IIIIIIIIAAAAAAAA -> AAAAAAAA
void PixelFormatUtils::convertAI88ToA8(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    for (ssize_t i = 1; i < dataLen; i += 2)
    {
        *outData++ = data[i]; //A
    }
}

// IIIIIIIIAAAAAAAA -> IIIIIIII
void PixelFormatUtils::convertAI88ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
    {
        *outData++ = data[i]; //R
    }
}

// RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA
void PixelFormatUtils::convertRGB888ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
    {
        *outData++ = data[i];         //R
        *outData++ = data[i + 1];     //G
        *outData++ = data[i + 2];     //B
        *outData++ = 0xFF;            //A
    }
}

// RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> RRRRRRRRGGGGGGGGBBBBBBBB
void PixelFormatUtils::convertRGBA8888ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
    {
        *outData++ = data[i];         //R
        *outData++ = data[i + 1];     //G
        *outData++ = data[i + 2];     //B
    }
}

// RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRRGGGGGGBBBBB
void PixelFormatUtils::convertRGB888ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    unsigned short* out16 = (unsigned short*)outData;
    for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
    {
        *out16++ = (data[i] & 0x00F8) << 8    //R
            | (data[i + 1] & 0x00FC) << 3     //G
            | (data[i + 2] & 0x00F8) >> 3;    //B
    }
}

// RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> RRRRRGGGGGGBBBBB
void PixelFormatUtils::convertRGBA8888ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    unsigned short* out16 = (unsigned short*)outData;
    for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
    {
        *out16++ = (data[i] & 0x00F8) << 8    //R
            | (data[i + 1] & 0x00FC) << 3     //G
            | (data[i + 2] & 0x00F8) >> 3;    //B
    }
}

// RRRRRRRRGGGGGGGGBBBBBBBB -> AAAAAAAA
void PixelFormatUtils::convertRGB888ToA8(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
    {
        *outData++ = (data[i] * 299 + data[i + 1] * 587 + data[i + 2] * 114 + 500) / 1000;  //A =  (R*299 + G*587 + B*114 + 500) / 1000
    }
}

// RRRRRRRRGGGGGGGGBBBBBBBB -> IIIIIIII
void PixelFormatUtils::convertRGB888ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
    {
        *outData++ = (data[i] * 299 + data[i + 1] * 587 + data[i + 2] * 114 + 500) / 1000;  //I =  (R*299 + G*587 + B*114 + 500) / 1000
    }
}

// RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> IIIIIIII
void PixelFormatUtils::convertRGBA8888ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
    {
        *outData++ = (data[i] * 299 + data[i + 1] * 587 + data[i + 2] * 114 + 500) / 1000;  //I =  (R*299 + G*587 + B*114 + 500) / 1000
    }
}

// RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> AAAAAAAA
void PixelFormatUtils::convertRGBA8888ToA8(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
    {
        *outData++ = data[i + 3]; //A
    }
}

// RRRRRRRRGGGGGGGGBBBBBBBB -> IIIIIIIIAAAAAAAA
void PixelFormatUtils::convertRGB888ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
    {
        *outData++ = (data[i] * 299 + data[i + 1] * 587 + data[i + 2] * 114 + 500) / 1000;  //I =  (R*299 + G*587 + B*114 + 500) / 1000
        *outData++ = 0xFF;
    }
}


// RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> IIIIIIIIAAAAAAAA
void PixelFormatUtils::convertRGBA8888ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
    {
        *outData++ = (data[i] * 299 + data[i + 1] * 587 + data[i + 2] * 114 + 500) / 1000;  //I =  (R*299 + G*587 + B*114 + 500) / 1000
        *outData++ = data[i + 3];
    }
}

// RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRGGGGBBBBAAAA
void PixelFormatUtils::convertRGB888ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    unsigned short* out16 = (unsigned short*)outData;
    for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
    {
        *out16++ = ((data[i] & 0x00F0) << 8           //R
            | (data[i + 1] & 0x00F0) << 4     //G
            | (data[i + 2] & 0xF0)            //B
            | 0x0F);                         //A
    }
}

// RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> RRRRGGGGBBBBAAAA
void PixelFormatUtils::convertRGBA8888ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    unsigned short* out16 = (unsigned short*)outData;
    for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
    {
        *out16++ = (data[i] & 0x00F0) << 8    //R
            | (data[i + 1] & 0x00F0) << 4         //G
            | (data[i + 2] & 0xF0)                //B
            | (data[i + 3] & 0xF0) >> 4;         //A
    }
}

// RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRRGGGGGBBBBBA
void PixelFormatUtils::convertRGB888ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    unsigned short* out16 = (unsigned short*)outData;
    for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
    {
        *out16++ = (data[i] & 0x00F8) << 8    //R
            | (data[i + 1] & 0x00F8) << 3     //G
            | (data[i + 2] & 0x00F8) >> 2     //B
            | 0x01;                          //A
    }
}

// RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRRGGGGGBBBBBA
void PixelFormatUtils::convertRGBA8888ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    unsigned short* out16 = (unsigned short*)outData;
    for (ssize_t i = 0, l = dataLen - 2; i < l; i += 4)
    {
        *out16++ = (data[i] & 0x00F8) << 8    //R
            | (data[i + 1] & 0x00F8) << 3     //G
            | (data[i + 2] & 0x00F8) >> 2     //B
            | (data[i + 3] & 0x0080) >> 7;   //A
    }
}

void PixelFormatUtils::convertRGB5A1ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    uint16_t *inData = (uint16_t*)data;
    const uint16_t pixelLen = dataLen / 2;
    uint16_t pixel;
    for (uint32_t i = 0; i < pixelLen; i++)
    {
        pixel = inData[i];
        *outData++ = (pixel & (0x001F << 11)) >> 8;
        *outData++ = (pixel & (0x001F << 6)) >> 3;
        *outData++ = (pixel & (0x001F << 1)) << 2;
        *outData++ = (pixel & 0x0001) * 255;
    }
}

void PixelFormatUtils::convertRGB565ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    uint16_t *inData = (uint16_t*)data;
    const uint16_t pixelLen = dataLen / 2;
    uint16_t pixel;
    for (uint32_t i = 0; i < pixelLen; i++)
    {
        pixel = inData[i];
        *outData++ = (pixel & (0x001F << 11)) >> 8;
        *outData++ = (pixel & (0x003F << 5)) >> 3;
        *outData++ = (pixel & (0x001F)) << 3;
        *outData++ = 0xFF;
    }
}

void PixelFormatUtils::convertRGBA4444ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    uint16_t *inData = (uint16_t*)data;
    const uint16_t pixelLen = dataLen / 2;
    uint16_t pixel;
    for (uint32_t i = 0; i < pixelLen; i++)
    {
        pixel = inData[i];
        *outData++ = ((pixel & 0xF000) >> 12) * 17;
        *outData++ = ((pixel & 0x0F00) >> 8) * 17;
        *outData++ = ((pixel & 0x00F0) >> 4) * 17;
        *outData++ = ((pixel & 0x000F) * 17);
    }

}

void PixelFormatUtils::convertA8ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
    for (uint32_t i = 0; i < dataLen; i++)
    {
        *outData++ = 0;
        *outData++ = 0;
        *outData++ = 0;
        *outData++ = data[i];
    }
}


// converter function end
//////////////////////////////////////////////////////////////////////////



Texture2D::PixelFormat PixelFormatUtils::convertI8ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen)
{
    switch (format)
    {
    case PixelFormat::RGBA8888:
        *outDataLen = dataLen*4;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertI8ToRGBA8888(data, dataLen, *outData);
        break;
    case PixelFormat::RGB888:
        *outDataLen = dataLen*3;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertI8ToRGB888(data, dataLen, *outData);
        break;
    case PixelFormat::RGB565:
        *outDataLen = dataLen*2;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertI8ToRGB565(data, dataLen, *outData);
        break;
    case PixelFormat::AI88:
        *outDataLen = dataLen*2;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertI8ToAI88(data, dataLen, *outData);
        break;
    case PixelFormat::RGBA4444:
        *outDataLen = dataLen*2;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertI8ToRGBA4444(data, dataLen, *outData);
        break;
    case PixelFormat::RGB5A1:
        *outDataLen = dataLen*2;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertI8ToRGB5A1(data, dataLen, *outData);
        break;
    case PixelFormat::A8:
        *outData = (unsigned char*)data;
        *outDataLen = dataLen;
        break;
    default:
        // unsupported conversion or don't need to convert
        if (format != PixelFormat::AUTO && format != PixelFormat::I8)
        {
            CCLOG("Can not convert image format PixelFormat::I8 to format ID:%d, we will use it's origin format PixelFormat::I8", static_cast<int>(format));
        }

        *outData = (unsigned char*)data;
        *outDataLen = dataLen;
        return PixelFormat::I8;
    }

    return format;
}

Texture2D::PixelFormat PixelFormatUtils::convertAI88ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen)
{
    switch (format)
    {
    case PixelFormat::RGBA8888:
        *outDataLen = dataLen*2;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertAI88ToRGBA8888(data, dataLen, *outData);
        break;
    case PixelFormat::RGB888:
        *outDataLen = dataLen/2*3;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertAI88ToRGB888(data, dataLen, *outData);
        break;
    case PixelFormat::RGB565:
        *outDataLen = dataLen;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertAI88ToRGB565(data, dataLen, *outData);
        break;
    case PixelFormat::A8:
        *outDataLen = dataLen/2;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertAI88ToA8(data, dataLen, *outData);
        break;
    case PixelFormat::I8:
        *outDataLen = dataLen/2;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertAI88ToI8(data, dataLen, *outData);
        break;
    case PixelFormat::RGBA4444:
        *outDataLen = dataLen;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertAI88ToRGBA4444(data, dataLen, *outData);
        break;
    case PixelFormat::RGB5A1:
        *outDataLen = dataLen;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertAI88ToRGB5A1(data, dataLen, *outData);
        break;
    default:
        // unsupported conversion or don't need to convert
        if (format != PixelFormat::AUTO && format != PixelFormat::AI88)
        {
            CCLOG("Can not convert image format PixelFormat::AI88 to format ID:%d, we will use it's origin format PixelFormat::AI88", static_cast<int>(format));
        }

        *outData = (unsigned char*)data;
        *outDataLen = dataLen;
        return PixelFormat::AI88;
        break;
    }

    return format;
}

Texture2D::PixelFormat PixelFormatUtils::convertRGB888ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen)
{
    switch (format)
    {
    case PixelFormat::RGBA8888:
        *outDataLen = dataLen/3*4;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertRGB888ToRGBA8888(data, dataLen, *outData);
        break;
    case PixelFormat::RGB565:
        *outDataLen = dataLen/3*2;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertRGB888ToRGB565(data, dataLen, *outData);
        break;
    case PixelFormat::A8:
        *outDataLen = dataLen/3;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertRGB888ToA8(data, dataLen, *outData);
        break;
    case PixelFormat::I8:
        *outDataLen = dataLen/3;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertRGB888ToI8(data, dataLen, *outData);
        break;
    case PixelFormat::AI88:
        *outDataLen = dataLen/3*2;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertRGB888ToAI88(data, dataLen, *outData);
        break;
    case PixelFormat::RGBA4444:
        *outDataLen = dataLen/3*2;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertRGB888ToRGBA4444(data, dataLen, *outData);
        break;
    case PixelFormat::RGB5A1:
        *outDataLen = dataLen;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertRGB888ToRGB5A1(data, dataLen, *outData);
        break;
    default:
        // unsupported conversion or don't need to convert
        if (format != PixelFormat::AUTO && format != PixelFormat::RGB888)
        {
            CCLOG("Can not convert image format PixelFormat::RGB888 to format ID:%d, we will use it's origin format PixelFormat::RGB888", static_cast<int>(format));
        }

        *outData = (unsigned char*)data;
        *outDataLen = dataLen;
        return PixelFormat::RGB888;
    }
    return format;
}

Texture2D::PixelFormat PixelFormatUtils::convertRGBA8888ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen)
{

    switch (format)
    {
    case PixelFormat::RGB888:
        *outDataLen = dataLen/4*3;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertRGBA8888ToRGB888(data, dataLen, *outData);
        break;
    case PixelFormat::RGB565:
        *outDataLen = dataLen/2;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertRGBA8888ToRGB565(data, dataLen, *outData);
        break;
    case PixelFormat::A8:
        *outDataLen = dataLen/4;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertRGBA8888ToA8(data, dataLen, *outData);
        break;
    case PixelFormat::I8:
        *outDataLen = dataLen/4;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertRGBA8888ToI8(data, dataLen, *outData);
        break;
    case PixelFormat::AI88:
        *outDataLen = dataLen/2;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertRGBA8888ToAI88(data, dataLen, *outData);
        break;
    case PixelFormat::RGBA4444:
        *outDataLen = dataLen/2;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertRGBA8888ToRGBA4444(data, dataLen, *outData);
        break;
    case PixelFormat::RGB5A1:
        *outDataLen = dataLen/2;
        *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
        convertRGBA8888ToRGB5A1(data, dataLen, *outData);
        break;
    default:
        // unsupported conversion or don't need to convert
        if (format != PixelFormat::AUTO && format != PixelFormat::RGBA8888)
        {
            CCLOG("Can not convert image format PixelFormat::RGBA8888 to format ID:%d, we will use it's origin format PixelFormat::RGBA8888", static_cast<int>(format));
        }

        *outData = (unsigned char*)data;
        *outDataLen = dataLen;
        return PixelFormat::RGBA8888;
    }

    return format;
}

Texture2D::PixelFormat PixelFormatUtils::convertRGB5A1ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen)
{
    switch (format)
    {
        case PixelFormat::RGBA8888:
            *outDataLen = dataLen/2*4;
            *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
            convertRGB5A1ToRGBA8888(data, dataLen, *outData);
            break;
        default:
            // unsupported conversion or don't need to convert
            if (format != PixelFormat::AUTO && format != PixelFormat::RGBA8888)
            {
                CCLOG("Can not convert image format PixelFormat::RGB5A1 to format ID:%d, we will use it's origin format PixelFormat::RGB51A", static_cast<int>(format));
            }
            *outData = (unsigned char*)data;
            *outDataLen = dataLen;
            return PixelFormat::RGBA8888;
    }
    
    return format;
}

Texture2D::PixelFormat PixelFormatUtils::convertRGB565ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen)
{
    switch (format)
    {
        case PixelFormat::RGBA8888:
            *outDataLen = dataLen/2*4;
            *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
            convertRGB565ToRGBA8888(data, dataLen, *outData);
            break;
        default:
            // unsupported conversion or don't need to convert
            if (format != PixelFormat::AUTO && format != PixelFormat::RGBA8888)
            {
                CCLOG("Can not convert image format PixelFormat::RGB565 to format ID:%d, we will use it's origin format PixelFormat::RGB565", static_cast<int>(format));
            }
            *outData = (unsigned char*)data;
            *outDataLen = dataLen;
            return PixelFormat::RGBA8888;
    }
    
    return format;
}

Texture2D::PixelFormat PixelFormatUtils::convertA8ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen)
{
    switch (format)
    {
        case PixelFormat::RGBA8888:
            *outDataLen = dataLen*4;
            *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
            convertA8ToRGBA8888(data, dataLen, *outData);
            break;
        default:
            // unsupported conversion or don't need to convert
            if (format != PixelFormat::AUTO && format != PixelFormat::RGBA8888)
            {
                CCLOG("Can not convert image format PixelFormat::A8 to format ID:%d, we will use it's origin format PixelFormat::A8", static_cast<int>(format));
            }
            *outData = (unsigned char*)data;
            *outDataLen = dataLen;
            return PixelFormat::RGBA8888;
    }
    
    return format;
}

Texture2D::PixelFormat PixelFormatUtils::convertRGBA4444ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen)
{
    switch (format)
    {
        case PixelFormat::RGBA8888:
            *outDataLen = dataLen/ 2 * 4;
            *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
            convertRGBA4444ToRGBA8888(data, dataLen, *outData);
            break;
        default:
            // unsupported conversion or don't need to convert
            if (format != PixelFormat::AUTO && format != PixelFormat::RGBA8888)
            {
                CCLOG("Can not convert image format PixelFormat::RGBA444 to format ID:%d, we will use it's origin format PixelFormat::RGBA4444", static_cast<int>(format));
            }
            *outData = (unsigned char*)data;
            *outDataLen = dataLen;
            return PixelFormat::RGBA8888;
    }
    
    return format;
}
/*
convert map:
1.PixelFormat::RGBA8888
2.PixelFormat::RGB888
3.PixelFormat::RGB565
4.PixelFormat::A8
5.PixelFormat::I8
6.PixelFormat::AI88
7.PixelFormat::RGBA4444
8.PixelFormat::RGB5A1

gray(5) -> 1235678
gray alpha(6) -> 12345678
rgb(2) -> 1235678
rgba(1) -> 12345678

*/
Texture2D::PixelFormat PixelFormatUtils::convertDataToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat originFormat, PixelFormat format, unsigned char** outData, ssize_t* outDataLen)
{
    // don't need to convert
    if (format == originFormat || format == PixelFormat::AUTO)
    {
        *outData = (unsigned char*)data;
        *outDataLen = dataLen;
        return originFormat;
    }
    
    switch (originFormat)
    {
    case PixelFormat::I8:
        return convertI8ToFormat(data, dataLen, format, outData, outDataLen);
    case PixelFormat::AI88:
        return convertAI88ToFormat(data, dataLen, format, outData, outDataLen);
    case PixelFormat::RGB888:
        return convertRGB888ToFormat(data, dataLen, format, outData, outDataLen);
    case PixelFormat::RGBA8888:
        return convertRGBA8888ToFormat(data, dataLen, format, outData, outDataLen);
    case PixelFormat::RGB5A1:
        return convertRGB5A1ToFormat(data, dataLen, format, outData, outDataLen);
    case PixelFormat::RGB565:
        return convertRGB565ToFormat(data, dataLen, format, outData, outDataLen);
#ifdef CC_USE_METAL
    case PixelFormat::RGBA4444:
        return convertRGBA4444ToFormat(data, dataLen, format, outData, outDataLen);
    case PixelFormat::A8:
        return convertA8ToFormat(data, dataLen, format, outData, outDataLen);

#endif
    default:
        CCLOG("unsupported conversion from format %d to format %d", static_cast<int>(originFormat), static_cast<int>(format));
        *outData = (unsigned char*)data;
        *outDataLen = dataLen;
        return originFormat;
    }
}

}

NS_CC_END