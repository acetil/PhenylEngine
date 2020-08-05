#include "graphics/graphics_headers.h"
#include "graphics/maths_headers.h"
#include <string>
#include <unordered_map>
#ifndef SHADERS_H
    #define SHADERS_H
    namespace graphics {
        class ShaderProgram {
            private:
                GLuint programId;
                std::string name;
                std::unordered_map<std::string, GLuint> uniformMap;
            public:
                ShaderProgram (GLuint program, std::string name);
                void useProgram () const;
                void registerUniform (const std::string& _name);
                void appplyUniform(const std::string& _name, glm::mat4 matrix);
        };

        GLuint loadShader (const char* filepath, GLuint shaderType);
        ShaderProgram* loadShaderProgram (const char* vertexPath, const char* fragmentPath, std::string name);
    }
#endif