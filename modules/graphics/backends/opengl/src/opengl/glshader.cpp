#include <fstream>

#include <nlohmann/json.hpp>

#include "core/assets/assets.h"

#include "glshader.h"

using namespace phenyl::graphics;
using namespace phenyl::opengl;

static phenyl::Logger LOGGER{"GL_SHADER", detail::OPENGL_LOGGER};

static GLenum GetGlShaderType (ShaderSourceType type);
static std::optional<GLuint> LoadShader (GLenum type, const std::string& source);
static std::optional<GLuint> LinkShader (const std::unordered_map<ShaderSourceType, GLuint>& shaders);
static std::optional<std::string> ReadFromPath (const std::string& path);

GlShader::Builder& GlShader::Builder::withSource (ShaderSourceType type, std::string source) {
    PHENYL_DASSERT_MSG(!shaderSources.contains(type), "Attempted to add source to shader of type {} twice!", (unsigned int)type);

    auto shader = LoadShader(GetGlShaderType(type), source);
    if (shader) {
        shaderSources.emplace(type, *shader);
    }
    return *this;
}

GlShader::Builder& GlShader::Builder::withAttrib (ShaderDataType type, std::string attrib) {
    PHENYL_DASSERT_MSG(!attribs.contains(attrib), "Attempted to add vertex attrib \"{}\" to shader twice!", attrib);

    attribs.emplace(std::move(attrib), type);
    return *this;
}

GlShader::Builder& GlShader::Builder::withUniformBlock (std::string uniformName) {
    PHENYL_DASSERT_MSG(!uniformBlocks.contains(uniformName), "Attempted to add uniform \"{}\" to shader twice!", uniformName);

    uniformBlocks.emplace(std::move(uniformName));
    return *this;
}

GlShader::Builder& GlShader::Builder::withSampler (std::string samplerName) {
    PHENYL_DASSERT_MSG(!samplers.contains(samplerName), "Attempted to add sampler \"{}\" to shader twice!", samplerName);

    samplers.emplace(samplerName);
    return *this;
}

std::unique_ptr<GlShader> GlShader::Builder::build () {
    auto programId = LinkShader(shaderSources);
    if (!programId) {
        PHENYL_LOGE(LOGGER, "Failed to build shader: link error");
        return nullptr;
    }

    auto shader = std::make_unique<GlShader>(*programId);
    for (const auto& [attrib, type] : attribs) {
        if (!shader->addAttrib(type, attrib)) {
            PHENYL_LOGE(LOGGER, "Failed to build shader: missing attrib \"{}\"", attrib);
            return nullptr;
        }
    }

    for (const auto& uniform : uniformBlocks) {
        if (!shader->addUniformBlock(uniform)) {
            PHENYL_LOGE(LOGGER, "Failed to build shader: missing uniform \"{}\"", uniform);
            return nullptr;
        }
    }

    for (const auto& sampler : samplers) {
        if (!shader->addSampler(sampler)) {
            PHENYL_LOGE(LOGGER, "Failed to build shader: missing sampler \"{}\"", sampler);
            return nullptr;
        }
    }

    PHENYL_TRACE(LOGGER, "Successfully built shader");
    return shader;
}

GlShader::GlShader (GLuint programId) : programId{programId} {
    PHENYL_DASSERT(programId);
}

GlShader::GlShader (GlShader&& other) noexcept : programId{other.programId}, uniformBlocks{std::move(other.uniformBlocks)}, samplers{std::move(other.samplers)} {
    other.programId = 0;
}

GlShader& GlShader::operator= (GlShader&& other) noexcept {
    if (programId) {
        glDeleteProgram(programId);
    }

    programId = other.programId;
    uniformBlocks = std::move(other.uniformBlocks);
    samplers = std::move(other.samplers);

    other.programId = 0;

    return *this;
}

GlShader::~GlShader () {
    if (programId) {
        glDeleteProgram(programId);
    }
}

bool GlShader::addAttrib (ShaderDataType type, const std::string& attrib) {
    PHENYL_ASSERT(!attribs.contains(attrib));

    auto location = glGetAttribLocation(programId, attrib.c_str());
    if (location == -1) {
        PHENYL_LOGW(LOGGER, "Failed to find vertex attrib \"{}\"", attrib);
        return true;
    }

    PHENYL_TRACE(LOGGER, "Located vertex attrib \"{}\" at location={}", attrib, location);
    attribs[attrib] = location;
    return true;
}

bool GlShader::addUniformBlock (const std::string& uniform) {
    PHENYL_ASSERT(!uniformBlocks.contains(uniform));

    unsigned int location = uniformBlocks.size();
    PHENYL_ASSERT_MSG(location < GL_MAX_UNIFORM_BUFFER_BINDINGS, "Attempted to add too many uniform blocks to shader!");

    PHENYL_TRACE(LOGGER, "Adding uniform block \"{}\" at location={}", uniform, location);
    auto blockIndex = glGetUniformBlockIndex(programId, uniform.c_str());
    if (blockIndex == GL_INVALID_INDEX) {
        PHENYL_LOGE(LOGGER, "Failed to find uniform \"{}\"", uniform);
        return false;
    }

    bind();
    glUniformBlockBinding(programId, blockIndex, location);
    uniformBlocks[uniform] = location;
    return true;
}

bool GlShader::addSampler (const std::string& sampler) {
    PHENYL_ASSERT(!samplers.contains(sampler));

    unsigned int samplerId = samplers.size();

    PHENYL_TRACE(LOGGER, "Adding sampler \"{}\" at id={}", sampler, samplerId);
    auto samplerUniform = glGetUniformLocation(programId, sampler.c_str());
    if (samplerUniform == -1) {
        PHENYL_LOGE(LOGGER, "Failed to find sampler \"{}\"", sampler);
        return false;
    }

    bind();
    glUniform1i(samplerUniform, static_cast<GLint>(samplerId));
    samplers[sampler] = samplerId;
    return true;
}

std::size_t GlShader::hash () const noexcept {
    return static_cast<std::size_t>(programId);
}

std::optional<unsigned int> GlShader::getAttribLocation (const std::string& attrib) const noexcept {
    auto it = attribs.find(attrib);
    return it != attribs.end() ? std::optional{it->second} : std::nullopt;
}

std::optional<unsigned int> GlShader::getUniformLocation (const std::string& uniform) const noexcept {
    auto uniformIt = uniformBlocks.find(uniform);
    return uniformIt != uniformBlocks.end() ? std::optional{uniformIt->second} : std::nullopt;
}

std::optional<unsigned int> GlShader::getSamplerLocation (const std::string& sampler) const noexcept {
    auto samplerIt = samplers.find(sampler);
    return samplerIt != samplers.end() ? std::optional{samplerIt->second} : std::nullopt;
}

std::optional<std::size_t> GlShader::getUniformOffset (const std::string& uniformBlock,
    const std::string& uniform) const noexcept {
    auto index = glGetProgramResourceIndex(programId, GL_UNIFORM, uniform.c_str());
    if (index == GL_INVALID_INDEX) {
        return std::nullopt;
    }

    GLenum prop = GL_OFFSET;
    GLint offset = 0;
    glGetProgramResourceiv(programId, GL_UNIFORM, index, 1, &prop, 1, nullptr, &offset);

    return static_cast<std::size_t>(offset);
}

std::optional<std::size_t> GlShader::getUniformBlockSize (const std::string& uniformBlock) const noexcept {
    auto index = glGetProgramResourceIndex(programId, GL_UNIFORM_BLOCK, uniformBlock.c_str());
    if (index == GL_INVALID_INDEX) {
        return std::nullopt;
    }

    GLenum prop = GL_BUFFER_DATA_SIZE;
    GLint size = 0;
    glGetProgramResourceiv(programId, GL_UNIFORM_BLOCK, index, 1, &prop, 1, nullptr, &size);
    return static_cast<std::size_t>(size);
}

void GlShader::bind () {
    glUseProgram(programId);
}

const char* GlShaderManager::getFileType () const {
    return ".json";
}

Shader* GlShaderManager::load (std::ifstream& data, std::size_t id) {
    PHENYL_TRACE(LOGGER, "Loading shader from file");
    nlohmann::json json;
    data >> json;
    if (!json.is_object()) {
        PHENYL_LOGE(LOGGER, "Expected object for shader, got {}", json.type_name());
        return nullptr;
    }

    if (!json.contains("shaders")) {
        PHENYL_LOGE(LOGGER, "Failed to find member \"shaders\" of shader!");
        return nullptr;
    }
    const auto& jsonShaders = json.at("shaders");
    if (!jsonShaders.is_object()) {
        PHENYL_LOGE(LOGGER, "Expected object for shaders member, got {}!", jsonShaders.type_name());
        return nullptr;
    }

    GlShader::Builder builder;
    for (const auto& [type, path] : jsonShaders.get<nlohmann::json::object_t>()) {
        if (!path.is_string()) {
            PHENYL_LOGE(LOGGER, "Expected string for \"{}\" shader path, got {}", type, path.type_name());
            return nullptr;
        }
        auto source = ReadFromPath(path.get<std::string>());
        if (!source) {
            PHENYL_LOGE(LOGGER, "Failed to read source for \"{}\" shader", type);
            return nullptr;
        }

        if (type == "fragment") {
            PHENYL_TRACE(LOGGER, "Added fragment shader");
            builder.withSource(ShaderSourceType::FRAGMENT, std::move(*source));
        } else if (type == "vertex") {
            PHENYL_TRACE(LOGGER, "Added vertex shader");
            builder.withSource(ShaderSourceType::VERTEX, std::move(*source));
        } else {
            PHENYL_LOGW(LOGGER, "Unsupported shader type \"{}\", ignoring", type);
        }
    }

    if (json.contains("uniforms")) {
        if (!json["uniforms"].is_array()) {
            PHENYL_LOGE(LOGGER, "Expected array for uniforms, got {}!", json["uniforms"].type_name());
            return nullptr;
        }
        for (const auto& key : json["uniforms"].get<nlohmann::json::array_t>()) {
            if (!key.is_string()) {
                PHENYL_LOGE(LOGGER, "Expected string for uniform name, got {}!", key.type_name());
                return nullptr;
            }

            PHENYL_TRACE(LOGGER, "Added uniform block \"{}\"", key.get<std::string>());
            builder.withUniformBlock(key.get<std::string>());
        }
    }

    if (json.contains("samplers")) {
        if (!json["samplers"].is_array()) {
            PHENYL_LOGE(LOGGER, "Expected array for samplers, got {}!", json["samplers"].type_name());
            return nullptr;
        }
        for (const auto& key : json["samplers"].get<nlohmann::json::array_t>()) {
            if (!key.is_string()) {
                PHENYL_LOGE(LOGGER, "Expected string for sampler name, got {}!", key.type_name());
                return nullptr;
            }
            PHENYL_TRACE(LOGGER, "Added sampler \"{}\"", key.get<std::string>());
            builder.withSampler(key.get<std::string>());
        }
    }

    auto shader = builder.build();
    if (!shader) {
        PHENYL_LOGE(LOGGER, "Failed to build shader!");
        return nullptr;
    }

    PHENYL_TRACE(LOGGER, "Successfully built shader");
    PHENYL_DASSERT(!shaders.contains(id));
    shaders[id] = Shader{std::move(shader)};
    return &shaders[id];
}

Shader* GlShaderManager::load (Shader&& obj, std::size_t id) {
    PHENYL_DASSERT(!shaders.contains(id));
    shaders[id] = std::move(obj);
    return &shaders[id];
}

void GlShaderManager::queueUnload (std::size_t id) {
    if (onUnload(id)) {
        shaders.erase(id);
    }
}

void GlShaderManager::selfRegister () {
    core::Assets::AddManager(this);
}

static std::optional<GLuint> LoadShader (GLenum type, const std::string& source) {
    auto shaderId = glCreateShader(type);

    PHENYL_TRACE(LOGGER, "Compiling shader:\n{}", source);
    const auto* sourcePtr = source.c_str();
    glShaderSource(shaderId, 1, &sourcePtr, nullptr);
    glCompileShader(shaderId);

    GLint result = GL_FALSE;
    GLint infoLength;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLength);
    if (!result) {
        auto info = std::make_unique<char[]>(infoLength);
        glGetShaderInfoLog(shaderId, infoLength, nullptr, info.get());
        PHENYL_LOGE(LOGGER, "Shader compile error: \"{}\"", info.get());

        return std::nullopt;
    }

    return shaderId;
}

static std::optional<GLuint> LinkShader (const std::unordered_map<ShaderSourceType, GLuint>& shaders) {
    if (!shaders.contains(ShaderSourceType::VERTEX)) {
        PHENYL_LOGE(LOGGER, "Attempted to link program with missing vertex shader!");
        return std::nullopt;
    }
    // if (!shaders.contains(ShaderSourceType::FRAGMENT)) {
    //     PHENYL_LOGE(LOGGER, "Attempted to link program with missing fragment shader!");
    //     return std::nullopt;
    // }

    auto programId = glCreateProgram();
    PHENYL_TRACE(LOGGER, "Building shader with programId={}", programId);

    PHENYL_TRACE(LOGGER, "Attaching shaders");
    for (auto [_, shader] : shaders) {
        glAttachShader(programId, shader);
    }

    PHENYL_TRACE(LOGGER, "Linking shader program");
    glLinkProgram(programId);

    GLint result = GL_FALSE;
    GLint infoLength;
    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLength);
    if (!result) {
        auto info = std::make_unique<char[]>(infoLength);
        glGetProgramInfoLog(programId, infoLength, nullptr, info.get());
        PHENYL_LOGE(LOGGER, "Shader link error: \"{}\"", info.get());
    }

    for (auto [_, shader] : shaders) {
        glDetachShader(programId, shader);
        glDeleteShader(shader);
    }

    if (result) {
        PHENYL_TRACE(LOGGER, "Successfully created shader with id={}", programId);
        return programId;
    } else {
        glDeleteProgram(programId);
        return std::nullopt;
    }
}

static std::optional<std::string> ReadFromPath (const std::string& path) {
    std::ifstream file{path};
    if (!file) {
        PHENYL_LOGE(LOGGER, "Failed to open shader source at \"{}\"", path);
        return std::nullopt;
    }

    return std::string{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}

static GLenum GetGlShaderType (ShaderSourceType type) {
    switch (type) {
        case ShaderSourceType::FRAGMENT:
            return GL_FRAGMENT_SHADER;
        case ShaderSourceType::VERTEX:
            return GL_VERTEX_SHADER;
    }

    PHENYL_ABORT("Invalid shader type: {}", static_cast<unsigned int>(type));
}