#pragma once

/* Standard library includes */
#include <iostream>

/* External dependencies */
#include <SDL2/SDL.h>


class CInputHandler {
	private:
	// Attributs
		SDL_Event event;
		unsigned int m_mousePositionX;
		unsigned int m_mousePositionY;
		int m_mouseMotionX;
		int m_mouseMotionY;
		unsigned int m_endEvent;
		unsigned int m_moveBackward;
		unsigned int m_moveForward;
		unsigned int m_moveLeft;
		unsigned int m_moveRight;
		unsigned int m_shoot;
		unsigned int m_bulletTime;
		unsigned int m_pause;

		SDL_Window* m_window;
		int WINDOW_CENTER_X;
		int WINDOW_CENTER_Y;
		
	
	// Methodes
		void keyPressed(SDL_KeyboardEvent* key);
		void keyReleased(SDL_KeyboardEvent* key);
		void mouseMoved(SDL_MouseMotionEvent* mouse);
		void mousePressed(SDL_MouseButtonEvent* mouse);
		void mouseReleased(SDL_MouseButtonEvent* mouse);
		void incInputTime();
	
	public:
		CInputHandler(SDL_Window* window);
		unsigned int getMoveForward()	const { return m_moveForward; }
		unsigned int getMoveBackward()	const { return m_moveBackward; }
		unsigned int getMoveLeft()		const { return m_moveLeft; }
		unsigned int getMoveRight()		const { return m_moveRight; }
		int getMouseMotionX()			{ return m_mouseMotionX; m_mouseMotionX = 0; }
		int getMouseMotionY()			{ return m_mouseMotionY; m_mouseMotionY = 0; }
		unsigned int getShoot()			const { return m_shoot; }
		unsigned int getBulletTime()	const { return m_bulletTime; }
		unsigned int isNotQuit()		const { return !m_endEvent; }
		unsigned int isNotPaused()      const { return !m_pause; }
		
		void update();
};
