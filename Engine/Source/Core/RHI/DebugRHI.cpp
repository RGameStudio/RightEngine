#include "DebugRHI.hpp"
#include <Logger.hpp>

void RightEngine::DebugRHI::Init()
{
#ifndef R_TEST_BUILD
//    glEnable(GL_DEBUG_OUTPUT);
//    glDebugMessageCallback(GLErrorCallback, nullptr);
#endif
}

void RightEngine::DebugRHI::GLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                            const GLchar *message, const void *userParam)
{
    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        R_CORE_ERROR("(OpenGL): {0} Error id: {1}", message, id);
    }
    else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
        R_CORE_WARN("(OpenGL): {0}", message);
    }
    else if (severity == GL_DEBUG_SEVERITY_LOW) {
        R_CORE_WARN("(OpenGL): {0}", message);
    }
    else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        R_CORE_TRACE("(OpenGL): {0}", message);
    }
}
