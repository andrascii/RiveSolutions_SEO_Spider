#pragma once

namespace QuickieWebBot
{

class QuickieWebBotHelpers
{
public:
	static int pointsToPixels(double sizeInPoints, double dpi = 0);
	static double pixelsToPoints(int sizeInPixels, double dpi = 0);

	static void moveWidgetToHostCenter(QWidget* widget, QWidget* host = nullptr);

	template <typename DestinationType, typename SourceType>
	static DestinationType safe_static_cast(SourceType* p)
	{
		if constexpr(std::is_pointer<DestinationType>::value)
		{
			DEBUG_ASSERT(dynamic_cast<DestinationType>(p));
			return static_cast<DestinationType>(p);
		}
		else
		{
			static_assert(!"Destination type must be a pointer");
		}
	}

	template <typename DestinationType, typename SourceType>
	static DestinationType safe_static_cast(SourceType&& p)
	{
		if constexpr(std::is_reference<DestinationType>::value)
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
			static_assert(!"Destination type must be a reference");
		}
	}
};

}