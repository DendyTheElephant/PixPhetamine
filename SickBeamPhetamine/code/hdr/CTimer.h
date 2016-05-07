#pragma once

/* Standard library includes */
#include <iostream>

/* External dependencies */
#include <SDL2/SDL.h>

class CTimer {
/* Members */
private:
	int m_startTicks{ 0 }; // Initial time
	int m_pausedTicks{ 0 }; // Time on pause

	bool m_isPaused{ false }; // Status in pause
	bool m_isStarted{ false }; // Status in activity

/* Methods */
public:
	CTimer();

	void start();
	void stop();
	void pause();
	void restart();

	float getElapsedTime(); // Get miliseconds elapsed from start or pause

	bool isStarted()	const { return m_isStarted; }
	bool isPaused()		const { return m_isPaused; }
};