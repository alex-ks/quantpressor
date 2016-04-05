#pragma once

namespace time_measurement
{
	class Timer
	{
	private:
		unsigned long start_timer = 0;

	public:

		void start( );

		///<summary>Returns time from previous start( ) call in seconds.</summary>
		double stop( );
	};
}