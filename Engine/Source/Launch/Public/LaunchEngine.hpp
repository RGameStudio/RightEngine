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
            void OnEvent(const Event& event);

        private:
            LaunchContext();
        };

        static LaunchContext* launchContext;
    };
}