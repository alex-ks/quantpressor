#pragma comment( lib, "winmm.lib" )

#include "Timer.h"

#include <windows.h>

namespace time_measurement
{
	void Timer::start( )
	{
		start_timer = timeGetTime( );
	}

	double Timer::stop( )
	{
		auto span = timeGetTime( ) - start_timer;
		return span / 1e3;
	}
}