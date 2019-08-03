#include "graphics_headers.h"
#include <string>
namespace graphics {
    GLuint loadShader (const char* filepath, GLuint shaderType);
    ShaderProgram loadShaders (const char* vertexPath, const char* fragmentPath, std::string name);
    class ShaderProgram {
        private:
            GLuint programId;
            std::string name;
        public:
            ShaderProgram (GLuint program, std::string name);
    };
}