#pragma once

#include "../Macros.h"
#include "../CommandBuffer.h"

#include "platform/CCGL.h"

#include <vector>

CC_BACKEND_BEGIN

class BufferGL;
class RenderPipelineGL;
class Program;

class CommandBufferGL : public CommandBuffer
{
public:
    CommandBufferGL();
    ~CommandBufferGL();
    
    virtual void beginFrame() override;
    virtual void beginRenderPass(const RenderPassDescriptor& descriptor) override;
    virtual void setRenderPipeline(RenderPipeline* renderPipeline) override;
    virtual void setViewport(int x, int y, unsigned int w, unsigned int h) override;
    virtual void setCullMode(CullMode mode) override;
    virtual void setVertexBuffer(unsigned int index, Buffer* buffer) override;
    virtual void setBindGroup(BindGroup* bindGroup) override;
    virtual void setIndexBuffer(Buffer* buffer) override;
    virtual void drawArrays(PrimitiveType primitiveType, unsigned int start,  unsigned int count) override;
    virtual void drawElements(PrimitiveType primitiveType, IndexFormat indexType, unsigned int count, unsigned int offset) override;
    virtual void endRenderPass() override;
    virtual void endFrame() override;
    
    virtual void setLineWidth(float lineWidth) override;
    
    virtual bool isScissorEnable() override;
    virtual void setScissorEnable(bool enabled) override;
    virtual void getScissorRect(float rect[4]) override;
    virtual void setScissorRect(float x, float y, float width, float height) override;
    
private:
    struct Viewport
    {
        int x = 0;
        int y = 0;
        unsigned int w = 0;
        unsigned int h = 0;
    };
    
    void prepareDrawing() const;
    void bindVertexBuffer(Program* program) const;
    void setUniforms(Program* program) const;
    void setUniform(bool isArray, GLuint location, unsigned int size, GLenum uniformType, void* data) const;
    void cleanResources();
    void applyRenderPassDescriptor(const RenderPassDescriptor& descirptor);
    
    struct Viewport _viewport;
    GLint _defaultFBO = 0;
    GLuint _currentFBO = 0;
    std::vector<BufferGL*> _vertexBuffers;
    BindGroup* _bindGroup = nullptr;
    BufferGL* _indexBuffer = nullptr;
    RenderPipelineGL* _renderPipeline = nullptr;
    CullMode _cullMode = CullMode::NONE;
};

CC_BACKEND_END
