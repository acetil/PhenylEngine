#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

#include "glshader.h"
#include "common/assets/assets.h"
#include "logging/logging.h"

using namespace phenyl::graphics;

static GLuint loadShader (ShaderType shaderType, const std::string& shaderPath);
static GLuint loadShader (GLuint shaderType, const std::string& shaderPath);

GLShaderProgram::GLShaderProgram (ShaderBuilder& builder) {

    auto spec = builder.build();

    initShaders(spec.shaderPaths);

    for (auto [name, type] : spec.uniforms.kv()) {
        uniformMap[name] = {glGetUniformLocation(programId, name.c_str()), type};
    }
}

void GLShaderProgram::initShaders (util::Map<ShaderType, std::string>& shaders) {
    std::vector<GLuint> shaderIds;

    for (auto [type, path] : shaders.kv()) {
        shaderIds.push_back(loadShader(type, path));
    }

    logging::log(LEVEL_INFO, "Linking shader program.");

    programId = glCreateProgram();

    for (auto& i : shaderIds) {
        glAttachShader(programId, i);
    }
    //glAttachShader(programId, vertexId);
    //glAttachShader(programId, fragmentId);
    glLinkProgram(programId);
    GLint result = GL_FALSE;
    int infoLength;
    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLength);
    if (infoLength > 0) {
        char* infoLog = new char[infoLength];
        glGetProgramInfoLog(programId, infoLength, nullptr, infoLog);
        logging::log(LEVEL_ERROR, infoLog);
        delete[] infoLog;
    }

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        logging::log(LEVEL_ERROR, "Shader error code {}", error);
    }

    //glDetachShader(programId, vertexId);
    //glDetachShader(programId, fragmentId);

    for (auto& i : shaderIds) {
        glDetachShader(programId, i);
        glDeleteShader(i);
    }
}

void GLShaderProgram::applyUniform (const std::string& uniformName, ShaderDataType uniformType, const unsigned char* uniformPtr) {
    if (!uniformMap.contains(uniformName) || uniformMap.at(uniformName).uniformType != uniformType) {
        logging::log(LEVEL_DEBUG, "Wrong uniform type for uniform \"{}\": expected {}, got {}", uniformName,
                     getUniformTypeName(uniformMap.at(uniformName).uniformType), getUniformTypeName(uniformType));
        return;
    }

    applyUniform(uniformMap.at(uniformName), uniformPtr);
}

void GLShaderProgram::applyUniform (GLUniform uniform, const unsigned char* uniformPtr) {
    bind();
    switch (uniform.uniformType) {
        case ShaderDataType::VEC2F:
            glUniform2fv(uniform.uniformId, 1, (float*)uniformPtr);
            break;
        case ShaderDataType::MAT2F:
            glUniformMatrix2fv(uniform.uniformId, 1, GL_FALSE, (float*)uniformPtr);
            break;
        case ShaderDataType::MAT4F:
            glUniformMatrix4fv(uniform.uniformId, 1, GL_FALSE, (float*)uniformPtr);
            break;
        default:
            logging::log(LEVEL_WARNING, "Unimplemented uniform type: {}", getUniformTypeName(uniform.uniformType));
    }
}

void GLShaderProgram::bind () {
    glUseProgram(programId);
}

GLShaderProgram::~GLShaderProgram () {
    glDeleteProgram(programId);
}

static GLuint loadShader (ShaderType shaderType, const std::string& shaderPath) {
    switch (shaderType) {
        case ShaderType::VERTEX:
            return loadShader(GL_VERTEX_SHADER, shaderPath);
        case ShaderType::FRAGMENT:
            return loadShader(GL_FRAGMENT_SHADER, shaderPath);
        default:
            logging::log(LEVEL_ERROR, "Unimplemented shader type!");
            return 0;
    }
}

static GLuint loadShader (GLuint shaderType, const std::string& shaderPath) {
    GLuint shader = glCreateShader(shaderType);
    std::string shaderCode;
    std::ifstream shaderStream(shaderPath);
    //shaderStream.open(shaderType, std::ios::in);
    if (shaderStream.is_open()) {
        std::stringstream sstr;
        sstr << shaderStream.rdbuf();
        shaderCode = sstr.str();
        shaderStream.close();
    } else {
        logging::log(LEVEL_ERROR, "Error reading shader file {}", shaderPath);
        return 0;
    }
    GLint result = GL_FALSE;
    int infolength;
    logging::log(LEVEL_DEBUG, "Compiling shader file at {}", shaderPath);
    const char* shaderSourcePtr = shaderCode.c_str();
    glShaderSource(shader, 1, &shaderSourcePtr, nullptr);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infolength);
    if (infolength > 0) {
        char* errorMessage = new char[infolength];
        glGetShaderInfoLog(shader, infolength, nullptr, errorMessage);
        logging::log(LEVEL_ERROR, errorMessage);
        delete[] errorMessage;
    }
    return shader;
}

GLShaderManager::GLShaderManager (GLRenderer* renderer) : renderer{renderer} {}

Shader* GLShaderManager::load (std::istream& data, std::size_t id) {
    nlohmann::json json;
    data >> json;
    if (!json.is_object()) {
        logging::log(LEVEL_ERROR, "Expected object for shader, got {}!", json.type_name());
        return nullptr;
    }

    if (!json.contains("shaders")) {
        logging::log(LEVEL_ERROR, "Failed to find member \"shaders\" of shader!");
        return nullptr;
    }
    const auto& shaderObj = json.at("shaders");
    if (!shaderObj.is_object()) {
        logging::log(LEVEL_ERROR, "Expected object for shaders member, got {}!", shaderObj.type_name());
        return nullptr;
    }

    const auto& obj = shaderObj.get<nlohmann::json::object_t>();

    std::string fragmentPath{};
    std::string vertexPath{};

    for (const auto& i : obj) {
        if (i.first == "fragment") {
            if (!i.second.is_string()) {
                logging::log(LEVEL_ERROR, "Expected string for fragment path, got {}!", i.second.type_name());
                return nullptr;
            }
            fragmentPath = i.second.get<std::string>();
        } else if (i.first == "vertex") {
            if (!i.second.is_string()) {
                logging::log(LEVEL_ERROR, "Expected string for vertex path, got {}!", i.second.type_name());
                return nullptr;
            }
            vertexPath = i.second.get<std::string>();
        }
    }

    ShaderBuilder builder{vertexPath, fragmentPath};
    if (json.contains("uniforms")) {
        if (!json["uniforms"].is_object()) {
            logging::log(LEVEL_ERROR, "Expected object for uniforms member, got {}!", json["uniforms"].type_name());
            return nullptr;
        }

        const auto& uniforms = json["uniforms"].get<nlohmann::json::object_t>();
        for (const auto& [key, val] : uniforms) {
            if (!val.is_string()) {
                logging::log(LEVEL_ERROR, "Expected string for uniform type, got {}!", val.type_name());
                return nullptr;
            }
            const auto& type = val.get<std::string>();
            if (type == "float") {
                builder.addUniform<float>(key);
            } else if (type == "int") {
                builder.addUniform<int>(key);
            } else if (type == "vec2f") {
                builder.addUniform<glm::vec2>(key);
            } else if (type == "vec3f") {
                builder.addUniform<glm::vec3>(key);
            } else if (type == "vec4f") {
                builder.addUniform<glm::vec4>(key);
            } else if (type == "mat2f") {
                builder.addUniform<glm::mat2>(key);
            } else if (type == "mat3f") {
                builder.addUniform<glm::mat3>(key);
            } else if (type == "mat4f") {
                builder.addUniform<glm::mat4>(key);
            } else {
                logging::log(LEVEL_ERROR, "Unknown uniform type: \"{}\"", type);
                return nullptr;
            }
        }
    }

    auto glshader = std::make_shared<GLShaderProgram>(builder);
    shaders[id] = std::make_unique<Shader>(std::move(glshader));

    return shaders[id].get();
}

const char* GLShaderManager::getFileType () const {
    return ".json";
}

void GLShaderManager::queueUnload (std::size_t id) {
    if (onUnload(id)) {
        shaders.remove(id);
    }
}

void GLShaderManager::selfRegister () {
    common::Assets::AddManager(this);
}
