#include "SandboxLayer.hpp"
#include <glslang/Include/glslang_c_interface.h>

using namespace RightEngine;

void SandboxLayer::OnAttach()
{
    glslang_initialize_process();
    glslang_finalize_process();
    std::cout << "Hello!" << std::endl;
}

void SandboxLayer::OnUpdate(float ts)
{

}

void SandboxLayer::OnImGuiRender()
{

}
