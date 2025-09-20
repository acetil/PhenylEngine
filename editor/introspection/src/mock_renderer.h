#pragma once

#include "core/assets/asset_manager.h"
#include "graphics/backend/renderer.h"
#include "phenyl/asset.h"

namespace phenyl {
class MockViewport : public graphics::Viewport {
public:
    [[nodiscard]] std::string_view getName () const noexcept override {
        return "phenyl::MockViewport";
    }

    [[nodiscard]] bool shouldClose () const override {
        return false;
    }

    void poll () override {}

    [[nodiscard]] glm::ivec2 getResolution () const override {
        return {1, 1};
    }

    [[nodiscard]] glm::vec2 getContentScale () const override {
        return {1, 1};
    }

    void addInputDevices (core::GameInput& manager) override {}

    void addUpdateHandler (graphics::IViewportUpdateHandler* handler) override {}
};

class MockBuffer : public graphics::IBuffer {
public:
    void upload (std::span<const std::byte> data) override {}
};

class MockUniformBuffer : public graphics::IUniformBuffer {
public:
    std::span<std::byte> allocate (std::size_t size) override {
        m_data.resize(size);
        return m_data;
    }

    void upload () override {}

    bool isReadable () const override {
        return false;
    }

    std::size_t getMinAlignment () const noexcept override {
        return 8;
    }

private:
    std::vector<std::byte> m_data;
};

class MockSampler : public graphics::ISampler {
public:
    [[nodiscard]] std::size_t hash () const noexcept override {
        return 0;
    }
};

class MockImageTexture : public graphics::IImageTexture {
public:
    [[nodiscard]] std::uint32_t width () const noexcept override {
        return 1;
    }

    [[nodiscard]] std::uint32_t height () const noexcept override {
        return 1;
    }

    void upload (const graphics::Image& image) override {}

    [[nodiscard]] graphics::ISampler& sampler () noexcept override {
        return m_sampler;
    }

private:
    MockSampler m_sampler;
};

class MockImageArrayTexture : public graphics::IImageArrayTexture {
public:
    [[nodiscard]] std::uint32_t width () const noexcept override {
        return 1;
    }

    [[nodiscard]] std::uint32_t height () const noexcept override {
        return 1;
    }

    [[nodiscard]] std::uint32_t size () const noexcept override {
        return 1;
    }

    void reserve (std::uint32_t capacity) override {}

    std::uint32_t append () override {
        return 1;
    }

    void upload (std::uint32_t index, const graphics::Image& image) override {}

    [[nodiscard]] graphics::ISampler& sampler () noexcept override {
        return m_sampler;
    }

private:
    MockSampler m_sampler;
};

class MockPipeline : public graphics::IPipeline {
public:
    void bindBuffer (meta::TypeIndex type, graphics::BufferBinding binding, const graphics::IBuffer& buffer,
        std::size_t offset) override {}

    void bindIndexBuffer (graphics::ShaderIndexType type, const graphics::IBuffer& buffer) override {}

    void bindUniform (meta::TypeIndex type, graphics::UniformBinding binding, const graphics::IUniformBuffer& buffer,
        std::size_t offset, std::size_t size) override {}

    void bindSampler (graphics::SamplerBinding binding, graphics::ISampler& sampler) override {}

    void unbindIndexBuffer () override {}

    void render (graphics::ICommandList& list, graphics::IFrameBuffer* fb, std::size_t vertices,
        std::size_t offset) override {}

    void renderInstanced (graphics::ICommandList& list, graphics::IFrameBuffer* fb, std::size_t numInstances,
        std::size_t vertices, std::size_t offset) override {}
};

class MockPipelineBuilder : public graphics::IPipelineBuilder {
public:
    void withBlendMode (graphics::BlendMode mode) override {}

    void withDepthTesting (bool doDepthWrite) override {}

    void withCullMode (graphics::CullMode mode) override {}

    void withGeometryType (graphics::GeometryType type) override {}

    void withShader (const std::shared_ptr<graphics::Shader>& shader) override {}

    graphics::BufferBinding withBuffer (meta::TypeIndex type, std::size_t size,
        graphics::BufferInputRate inputRate) override {
        return 0;
    }

    void withAttrib (graphics::ShaderDataType type, unsigned location, graphics::BufferBinding binding,
        std::size_t offset) override {}

    graphics::UniformBinding withUniform (meta::TypeIndex type, unsigned location) override {
        return 0;
    }

    graphics::SamplerBinding withSampler (unsigned location) override {
        return 0;
    }

    std::unique_ptr<graphics::IPipeline> build () override {
        return std::make_unique<MockPipeline>();
    }
};

class MockFrameBuffer : public graphics::IFrameBuffer {
public:
    void clear (glm::vec4 clearColor) override {}

    graphics::ISampler* getSampler () noexcept override {
        return &m_sampler;
    }

    graphics::ISampler* getDepthSampler () noexcept override {
        return &m_sampler;
    }

    glm::ivec2 getDimensions () const noexcept override {
        return {1, 1};
    }

private:
    MockSampler m_sampler;
};

class MockCommandList : public graphics::ICommandList {};

class MockRenderer : public graphics::Renderer {
public:
    [[nodiscard]] std::string_view getName () const noexcept override {
        return "phenyl::MockRenderer";
    }

    double getCurrentTime () override {
        return 0;
    }

    void clearWindow () override {}

    void finishRender () override {}

    graphics::PipelineBuilder buildPipeline () override {
        return graphics::PipelineBuilder{std::make_unique<MockPipelineBuilder>()};
    }

    void loadDefaultShaders () override {}

    graphics::Viewport& getViewport () override {
        return m_viewport;
    }

    const graphics::Viewport& getViewport () const override {
        return m_viewport;
    }

    void render () override {}

protected:
    std::unique_ptr<graphics::IBuffer> makeRendererBuffer (std::size_t startCapacity, std::size_t elementSize,
        graphics::BufferStorageHint storageHint, bool isIndex) override {
        return std::make_unique<MockBuffer>();
    }

    std::unique_ptr<graphics::IUniformBuffer> makeRendererUniformBuffer (bool readable) override {
        return std::make_unique<MockUniformBuffer>();
    }

    std::unique_ptr<graphics::IImageTexture> makeRendererImageTexture (
        const graphics::TextureProperties& properties) override {
        return std::make_unique<MockImageTexture>();
    }

    std::unique_ptr<graphics::IImageArrayTexture> makeRendererArrayTexture (
        const graphics::TextureProperties& properties, std::uint32_t width, std::uint32_t height) override {
        return std::make_unique<MockImageArrayTexture>();
    }

    std::unique_ptr<graphics::IFrameBuffer> makeRendererFrameBuffer (const graphics::FrameBufferProperties& properties,
        std::uint32_t width, std::uint32_t height) override {
        return std::make_unique<MockFrameBuffer>();
    }

    graphics::ICommandList* makeCommandList () override {
        return &m_cmdList;
    }

private:
    MockViewport m_viewport{};
    MockCommandList m_cmdList{};
};

class MockShader : public graphics::IShader {
public:
    [[nodiscard]] std::size_t hash () const noexcept override {
        return 0;
    }

    [[nodiscard]] std::optional<unsigned int> getAttribLocation (std::string_view attrib) const noexcept override {
        return 0;
    }

    [[nodiscard]] std::optional<unsigned int> getUniformLocation (const std::string& uniform) const noexcept override {
        return 0;
    }

    [[nodiscard]] std::optional<unsigned int> getSamplerLocation (const std::string& sampler) const noexcept override {
        return 0;
    }

    std::optional<std::size_t> getUniformOffset (const std::string& uniformBlock,
        const std::string& uniform) const noexcept override {
        return 0;
    }

    std::optional<std::size_t> getUniformBlockSize (const std::string& uniformBlock) const noexcept override {
        return 0;
    }
};

// TODO: remove need for this
class MockShaderManager : public core::AssetManager<graphics::Shader> {
public:
    std::shared_ptr<graphics::Shader> load (core::AssetLoadContext& ctx) override {
        return std::make_shared<graphics::Shader>(std::make_unique<MockShader>());
    }

    void selfRegister () {
        Assets::AddManager(this);

        auto mockShader = std::make_shared<graphics::Shader>(std::make_unique<MockShader>());
        Assets::LoadVirtual("phenyl/shaders/box", mockShader);
        Assets::LoadVirtual("phenyl/shaders/debug", mockShader);
        Assets::LoadVirtual("phenyl/shaders/sprite", mockShader);
        Assets::LoadVirtual("phenyl/shaders/canvas", mockShader);
        Assets::LoadVirtual("phenyl/shaders/particle", mockShader);
        Assets::LoadVirtual("phenyl/shaders/blinn_phong", mockShader);
        Assets::LoadVirtual("phenyl/shaders/shadow_map", mockShader);
        Assets::LoadVirtual("phenyl/shaders/mesh_prepass", mockShader);
        Assets::LoadVirtual("phenyl/shaders/postprocess/noop", mockShader);
        Assets::LoadVirtual("phenyl/shaders/test", mockShader);
    }
};
} // namespace phenyl
