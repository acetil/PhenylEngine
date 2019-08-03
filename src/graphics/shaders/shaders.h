#include "graphics_headers.h"
namespace graphics {
    GLuint loadShader (const char* filepath, GLuint shaderType);
    ShaderProgram loadShaders (const char* vertexPath, const char* fragmentPath, char* name);
    class ShaderProgram {
        private:
            GLuint programId;
            char* name;
        public:
            ShaderProgram (GLuint program, char* name);
    };
}