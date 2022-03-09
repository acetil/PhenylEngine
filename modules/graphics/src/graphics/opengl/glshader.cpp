#include <fstream>
#include <sstream>

#include "glshader.h"
#include "logging/logging.h"

using namespace graphics;

static GLuint loadShader (ShaderType shaderType, const std::string& shaderPath);
static GLuint loadShader (GLuint shaderType, const std::string& shaderPath);

GLShaderProgram::GLShaderProgram (ShaderProgramBuilder& builder) {

    auto spec = builder.build();

    initShaders(spec.shaderPaths);

    for (auto [name, type] : spec.uniforms) {
        uniformMap[name] = {glGetUniformLocation(programId, name.c_str()), type};
    }
}

void GLShaderProgram::initShaders (util::Map<ShaderType, std::string>& shaders) {
    std::vector<GLuint> shaderIds;

    for (auto& [type, path] : shaders) {
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

void GLShaderProgram::applyUniform (const std::string& uniformName, UniformType uniformType, const unsigned char* uniformPtr) {
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
        case UniformType::VEC2F:
            glUniform2fv(uniform.uniformId, 1, (float*)uniformPtr);
            break;
        case UniformType::MAT2F:
            glUniformMatrix2fv(uniform.uniformId, 1, GL_FALSE, (float*)uniformPtr);
            break;
        case UniformType::MAT4F:
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