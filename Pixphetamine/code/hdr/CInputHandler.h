/// \file		CInputHandler.h
///	\author		Daniel Huc
/// \date		April 2016
#pragma once

/* Standard library includes */
#include <iostream>

/* External dependencies */
#include <SDL2/SDL.h>

/* Internal headers includes */
#include "HInternalTypesAliases.h"

class CInputHandler {
	/* Members */
	private:
		SDL_Event event;
		pxUInt m_mousePositionX;
		pxUInt m_mousePositionY;
		pxInt m_mouseMotionX;
		pxInt m_mouseMotionY;
		pxUInt m_endEvent;
		pxUInt m_moveBackward;
		pxUInt m_moveForward;
		pxUInt m_moveLeft;
		pxUInt m_moveRight;
		pxUInt m_shoot;
		pxUInt m_bulletTime;
		pxUInt m_pause;

		SDL_Window* m_window;
		pxInt WINDOW_CENTER_X;
		pxInt WINDOW_CENTER_Y;
		
	
	/* Methods */
		void keyPressed(SDL_KeyboardEvent* key);
		void keyReleased(SDL_KeyboardEvent* key);
		void mouseMoved(SDL_MouseMotionEvent* mouse);
		void mousePressed(SDL_MouseButtonEvent* mouse);
		void mouseReleased(SDL_MouseButtonEvent* mouse);
		void incInputTime();
	
	public:
		CInputHandler(SDL_Window* window);
		pxUInt getMoveForward()		const { return m_moveForward; }
		pxUInt getMoveBackward()	const { return m_moveBackward; }
		pxUInt getMoveLeft()		const { return m_moveLeft; }
		pxUInt getMoveRight()		const { return m_moveRight; }
		pxInt getMouseMotionX()		{ return m_mouseMotionX; m_mouseMotionX = 0; }
		pxInt getMouseMotionY()		{ return m_mouseMotionY; m_mouseMotionY = 0; }
		pxUInt getShoot()			const { return m_shoot; }
		pxUInt getBulletTime()		const { return m_bulletTime; }
		pxUInt isNotQuit()			const { return !m_endEvent; }
		pxUInt isNotPaused()		const { return !m_pause; }
		
		void update();
};
