# Code style


## C++ Style
- Include order
```c++
// Engine include
// Third party lib include
// Std includes

#include "Renderer.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
```

- All getters and setters must follow semantic (except cases where object allocates new object on stack and returns it):
```c++
    const <Type> GetSomething() const;
    void SetSomething(const <Type>& value);
```
- Try to pass method parameters by **const reference** not by value

- Namespace must be declared only at header file
- In class definition public fields and methods goes first
```c++
namespace RightEngine
{
    class LaunchEngine
    {
    public:
        static void Init(int argc, char *argv[]);

        static void Exit();

    private:
        class LaunchContext
        {
            friend class LaunchEngine;

        public:
            bool OnEvent(const Event &event);

        private:
            LaunchContext();

            void SetCmdArgs(int argc, char *argv[]);

            void ParseCmdArgs();

            std::unique_ptr<EasyArgs> easyArgs;
        };

        static LaunchContext *launchContext;
    };
}
```
- `using namespace <...>` can be used in `.cpp` files for preventing lots of useless namespace declarations, but class methods
must be still marked with namespace
```c++
using namespace RightEngine;
// ...
void RightEngine::Geometry::CreateVertexArray(const VertexBufferLayout& layout)
{
    //...
}
```

## GLSL Style
- Uniforms must have `u_` prefix and start with capital letter (e.g. `u_ViewProjection`)
- Out variables in vertex shader must have `f_` prefix and start with capital letter (e.g. `f_LocalPos`)
- Constant variable in shaders must have `k` prefix (e.g. `kAngle`)
- Input and output layout variables must have `a` prefix
```glsl
layout (location = 0) out vec4 aAlbedo;
layout (location = 1) out vec4 aNormal;
```