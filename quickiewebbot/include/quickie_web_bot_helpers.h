#pragma once

namespace QuickieWebBot
{

class QuickieWebBotHelpers
{
public:
	static int pointsToPixels(double sizeInPoints, double dpi = 0);
	static double pixelsToPoints(int sizeInPixels, double dpi = 0);

	template <typename DestinationType, typename SourceType>
	static DestinationType safe_runtime_static_cast(SourceType p)
	{
		if constexpr(std::is_pointer<DestinationType>::value && std::is_pointer<SourceType>::value)
		{
			DEBUG_ASSERT(dynamic_cast<DestinationType>(p));
			return static_cast<DestinationType>(p);
		}
		else if constexpr(std::is_reference<DestinationType>::value && std::is_reference<SourceType>::value)
		{
		#ifdef DEBUG
			try
			{
				dynamic_cast<DestinationType>(p);
			}
			catch (...)
			{
				DEBUG_ASSERT(!"Actual type differs from DestinationType");
			}
		#endif

			return static_cast<DestinationType>(p);
		}
		else
		{
			static_assert(!"Destination type and source type must be a pointer or a reference");
		}
	}
};

}