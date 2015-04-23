#pragma once

namespace Platform
{
	struct EControlCommand
	{
		enum Type
		{
			Restart = 1,
			SetConfigProperties = 2,
			Shutdown = 3,
			Pause = 4,
			Resume = 5,
			UpdateConfigInfo = 6
		};
	};
}