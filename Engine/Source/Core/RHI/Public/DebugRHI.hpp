#pragma once
#include <glad/glad.h>

namespace RightEngine {
    class DebugRHI {
    public:
        static void Init();

    private:
        static void GLAPIENTRY GLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
                                               const void* userParam);
    };
}