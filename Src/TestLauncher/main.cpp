#pragma once

#include <Engine/Engine.hpp>
#include <Engine/Tests/Service/DoctestService.hpp>

int main(int argc, char* argv[])
{
    engine::Engine engine(argc, argv);

    engine.GetServiceManager()->RegisterService<tests::DoctestService>();
    engine.GetServiceManager()->UpdateDependencyOrder();

    auto context = std::make_unique<doctest::Context>();
    context->applyCommandLine(argc, argv);
    context->setOption("no-breaks", true);

    engine.Service<tests::DoctestService>().Context(std::move(context));

    return engine.Run();
}
