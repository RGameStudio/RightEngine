#include <Engine/Engine.hpp>
#include <any>

int main(int argc, char* argv[])
{
    std::any any;
    engine::Engine engine(argc, argv);
    return engine.Run();
}
