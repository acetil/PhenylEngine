#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <string.h>
#include <unordered_map>

#include "graphics/graphics_headers.h"
#include "graphics/math_headers.h"
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
    glShaderSource(shader, 1, &shaderSourcePtr, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infolength);
    if (infolength > 0) {
        char* errorMessage = (char*) malloc(sizeof(char) * infolength);
        glGetShaderInfoLog(shader, infolength, NULL, errorMessage);
        logging::log(LEVEL_ERROR, errorMessage);
        free(errorMessage);
    }
    return shader;
}

ShaderProgram graphics::loadShaders (const char* vertexPath, const char* fragmentPath, std::string name) {
    GLuint vertexId = loadShader(vertexPath, GL_VERTEX_SHADER);
    GLuint fragmentId = loadShader(fragmentPath, GL_FRAGMENT_SHADER);

    logging::log(LEVEL_INFO, "Linking shader program.");
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexId);
    glAttachShader(programId, fragmentId);

    GLint result = GL_FALSE;
    int infoLength;
    glGetProgramiv(programId, GL_COMPILE_STATUS, &result);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLength);
    if (infoLength > 0) {
        char* infoLog = (char*) malloc(infoLength * sizeof(char));
        glGetProgramInfoLog(programId, infoLength, NULL, infoLog);
        logging::log(LEVEL_ERROR, infoLog);
        free(infoLog);
    }

    glDetachShader(programId, vertexId);
    glDetachShader(programId, fragmentId);

    glDeleteShader(vertexId);
    glDeleteShader(fragmentId);
    return ShaderProgram(programId, name);
}

graphics::ShaderProgram::ShaderProgram (GLuint program, std::string name) {
    this->programId = program;
    this->name = name;
    this->uniformMap = std::unordered_map<std::string, GLuint>();
}
void graphics::ShaderProgram::useProgram () {
    glUseProgram(programId);
}
void graphics::ShaderProgram::registerUniform (std::string name) {
    uniformMap[name] = glGetUniformLocation(programId, name.c_str());
}
void graphics::ShaderProgram::appplyUniform (std::string name, glm::mat4 matrix) {
    GLuint uniformId = uniformMap[name];
    glUniformMatrix4fv(uniformId, 1, GL_FALSE, &matrix[0][0]);
}