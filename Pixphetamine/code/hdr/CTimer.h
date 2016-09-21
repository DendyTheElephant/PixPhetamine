#pragma once

/* Standard library includes */
#include <iostream>

/* External dependencies */
#include <SDL2/SDL.h>

/* Internal headers includes */
#include "HInternalTypesAliases.h"

class CTimer {
/* Members */
private:
	pxInt m_startTicks{ 0 }; // Initial time
	pxInt m_pausedTicks{ 0 }; // Time on pause

	pxBool m_isPaused{ false }; // Status in pause
	pxBool m_isStarted{ false }; // Status in activity

/* Methods */
public:
	CTimer();

	void start();
	void stop();
	void pause();
	void restart();

	pxFloat getElapsedTime() const; // Get miliseconds elapsed from start or pause

	pxBool isStarted()	const { return m_isStarted; }
	pxBool isPaused()	const { return m_isPaused; }
};