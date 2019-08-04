#include "graphics/graphics_headers.h"
#include "graphics/math_headers.h"
#include <string>
#ifndef SHADERS_H
    #define SHADERS_H
    namespace graphics {
        class ShaderProgram {
            private:
                GLuint programId;
                std::string name;
            public:
                ShaderProgram (GLuint program, std::string name);
                void useProgram ();
                void registerUniform (std::string name);
                void appplyUniform(std::string name, glm::mat4 matrix);
        };

        GLuint loadShader (const char* filepath, GLuint shaderType);
        ShaderProgram loadShaders (const char* vertexPath, const char* fragmentPath, std::string name);
    }
#endif