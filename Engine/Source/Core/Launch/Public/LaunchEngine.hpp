#pragma once

#include <Event.hpp>
#include <ShutdownEvent.hpp>

namespace RightEngine
{
    class LaunchEngine
    {
    public:
        static void Init();

        static void Exit();

    private:
        class LaunchContext
        {
        friend class LaunchEngine;

        public:
            bool OnEvent(const Event& event);

        private:
            LaunchContext();
        };

        static LaunchContext* launchContext;
    };
}