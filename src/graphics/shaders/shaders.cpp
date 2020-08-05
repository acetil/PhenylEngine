#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <utility>

#include "graphics/graphics_headers.h"
#include "graphics/maths_headers.h"
#include "shaders.h"
#include "logging/logging.h"

using namespace graphics;

GLuint graphics::loadShader (const char* filepath, GLuint shaderType) {
    GLuint shader = glCreateShader(shaderType);
    std::string shaderCode;
    std::ifstream shaderStream;
    shaderStream.open(filepath, std::ios::in);
    if (shaderStream.is_open()) {
        std::stringstream sstr;
        sstr << shaderStream.rdbuf();
        shaderCode = sstr.str();
        shaderStream.close();
    } else {
        logging::logf(LEVEL_ERROR, "Error reading shader file %s", filepath);
        return 0;
    }
    GLint result = GL_FALSE;
    int infolength;
    logging::logf(LEVEL_INFO, "Compiling shader file at %s", filepath);
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

ShaderProgram* graphics::loadShaderProgram (const char* vertexPath, const char* fragmentPath, std::string name) {
    GLuint vertexId = loadShader(vertexPath, GL_VERTEX_SHADER);
    GLuint fragmentId = loadShader(fragmentPath, GL_FRAGMENT_SHADER);

    logging::log(LEVEL_INFO, "Linking shader program.");
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexId);
    glAttachShader(programId, fragmentId);
    glLinkProgram(programId);
    GLint result = GL_FALSE;
    int infoLength;
    glGetProgramiv(programId, GL_COMPILE_STATUS, &result);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLength);
    if (infoLength > 0) {
        char* infoLog = new char[infoLength];
        glGetProgramInfoLog(programId, infoLength, nullptr, infoLog);
        logging::log(LEVEL_ERROR, infoLog);
        delete[] infoLog;
    }

    glDetachShader(programId, vertexId);
    glDetachShader(programId, fragmentId);

    glDeleteShader(vertexId);
    glDeleteShader(fragmentId);
    return new ShaderProgram(programId, std::move(name));
}

graphics::ShaderProgram::ShaderProgram (GLuint program, std::string name) {
    this->programId = program;
    this->name = std::move(name);
    this->uniformMap = std::unordered_map<std::string, GLuint>();
}
void graphics::ShaderProgram::useProgram () const {
    glUseProgram(programId);
}
void graphics::ShaderProgram::registerUniform (const std::string& _name) {
    uniformMap[_name] = glGetUniformLocation(programId, _name.c_str());
}
// TODO: do for different uniform types
void graphics::ShaderProgram::appplyUniform (const std::string& _name, glm::mat4 matrix) {
    GLuint uniformId = uniformMap[_name];
    glUniformMatrix4fv(uniformId, 1, GL_FALSE, &matrix[0][0]);
}