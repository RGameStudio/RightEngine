#pragma once

#include "Types.hpp"

namespace RightEngine
{
	class IService : public NonMovable
	{
	public:
		virtual void OnUpdate(float dt) = 0;
		virtual void OnRegister() = 0;
		virtual ~IService() = default;
	};
}
