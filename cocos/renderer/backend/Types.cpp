#include "Types.h"

#include "platform/CCGL.h"

CC_BACKEND_BEGIN

namespace BlendConst {
    //TODO: should be removed in the future
    static_assert(BLEND_ZERO == GL_ZERO, "BLEND value error: GL_ZERO");
    static_assert(BLEND_ONE == GL_ONE, "BLEND value error: GL_ONE");
    static_assert(BLEND_SRC_COLOR == GL_SRC_COLOR, "BLEND value error: GL_SRC_COLOR");
    static_assert(BLEND_ONE_MINUS_SRC_COLOR == GL_ONE_MINUS_SRC_COLOR, "BLEND value error: GL_ONE_MINUS_SRC_COLOR");
    static_assert(BLEND_DST_COLOR == GL_DST_COLOR, "BLEND value error: GL_DST_COLOR");
    static_assert(BLEND_ONE_MINUS_DST_COLOR == GL_ONE_MINUS_DST_COLOR, "BLEND value error: GL_ONE_MINUS_DST_COLOR");
    static_assert(BLEND_SRC_ALPHA == GL_SRC_ALPHA, "BLEND value error: GL_SRC_ALPHA");
    static_assert(BLEND_ONE_MINUS_SRC_ALPHA == GL_ONE_MINUS_SRC_ALPHA, "BLEND value error: GL_ONE_MINUS_SRC_ALPHA");
    static_assert(BLEND_DST_ALPHA == GL_DST_ALPHA, "BLEND value error: GL_DST_ALPHA");
    static_assert(BLEND_ONE_MINUS_DST_ALPHA == GL_ONE_MINUS_DST_ALPHA, "BLEND value error: GL_ONE_MINUS_DST_ALPHA");
    static_assert(BLEND_CONSTANT_ALPHA == GL_CONSTANT_ALPHA, "BLEND value error: GL_CONSTANT_ALPHA");
    static_assert(BLEND_ONE_MINUS_CONSTANT_ALPHA == GL_ONE_MINUS_CONSTANT_ALPHA, "BLEND value error: GL_ONE_MINUS_CONSTANT_ALPHA");
    static_assert(BLEND_SRC_ALPHA_SATURATE == GL_SRC_ALPHA_SATURATE, "BLEND value error: GL_SRC_ALPHA_SATURATE");
    static_assert(BLEND_BLEND_COLOR == GL_BLEND_COLOR, "BLEND value error: GL_SRC_ALPHA_SATURATE");
}

CC_BACKEND_END