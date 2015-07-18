#pragma once

namespace Platform
{
	struct EServiceLogSeverity
	{
		enum Type
		{
			Fatal = 1,
			Error = 2,
			Warning = 3,
			Info = 4,
			Debug = 5
		};
	};
}