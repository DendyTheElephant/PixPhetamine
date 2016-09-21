#include "CTimer.h"

CTimer::CTimer() {
}

void CTimer::start() {
	m_isStarted = true;
	m_isPaused = false;

	m_startTicks = SDL_GetTicks();
}

void CTimer::stop() {
	m_isStarted = false;
	m_isPaused = false;
}

void CTimer::pause() {
	if (m_isStarted == true  &&  m_isPaused == false) {
		m_isPaused = true;
		m_pausedTicks = SDL_GetTicks() - m_startTicks;
	}
}

void CTimer::restart() {
	if (m_isPaused == true) {
		m_isPaused = false;

		m_startTicks = SDL_GetTicks() - m_pausedTicks;

		m_pausedTicks = 0;
	}
}


pxFloat CTimer::getElapsedTime() const {
	if (m_isStarted == true) {
		if (m_isPaused == true) {
			return m_pausedTicks;
		}
		else {
			return SDL_GetTicks() - m_startTicks;
		}
	}

	return 0;
}


