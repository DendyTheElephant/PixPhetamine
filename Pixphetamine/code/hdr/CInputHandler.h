////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////====================================================================================================================================////
/// | \file		CInputHandler.h
///	| \author	Daniel Huc
/// | \date		February 2017
/// |----------------------------------------------------------------------------------------------------------------------------------
/// | \brief	Based on GLFW
/// | \details	Blabla
////====================================================================================================================================////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
/* Standard library includes */
#include <iostream>

/* External dependencies */
#include <SDL2/SDL.h>

/* Internal headers includes */
#include "HInternalTypesAliases.h"

//// ---- Namespaces ---- ////
namespace Utility {

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////====================================================================================================================================////
	//// CInputHandler
	/// \brief		Blabla
	/// \details	Blabla
	////====================================================================================================================================////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class CInputHandler {

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//// ---- Enumerations -----                                                                                                            ////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	public:
		enum class EInput {
			UP = SDLK_z,
			DOWN = SDLK_s,
			RIGHT = SDLK_q,
			LEFT = SDLK_d,
			SHOULDER_RIGHT = SDLK_e,
			SHOULDER_LEFT = SDLK_a,
			QUIT = SDLK_ESCAPE
		};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//// ---- Members -----																													//// 
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	private:
		SDL_Event event;
		pxMap<EInput, pxUInt16> m_inputs;
		pxInt m_mouseMotionX{ 0 };
		pxInt m_mouseMotionY{ 0 };
		pxUInt16 m_mouseButton1{ 0 };
		pxUInt16 m_mouseButton2{ 0 };
		pxBool m_endEvent{ false };
		pxBool m_pause{ false };

		SDL_Window* m_window;
		pxInt M_WINDOW_CENTER_X;
		pxInt M_WINDOW_CENTER_Y;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//// ---- Methods -----																													////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	private:
	//// ---- Internal methods ---- ////
		void _updateInput(SDL_Keycode key, pxUInt16 value);

	public:
	//// ---- Constructor/Destructor ---- ////
		CInputHandler(SDL_Window* window);

	//// ---- Getter ---- ////
		pxUInt getKey(EInput const& requiredInput) const { return m_inputs.at(requiredInput); }
		pxInt getMouseMotionX() const { return m_mouseMotionX; }
		pxInt getMouseMotionY() const { return m_mouseMotionY; }
		pxBool isQuit() const { return (m_endEvent || getKey(EInput::QUIT)); }

	//// ---- Core ---- ////
		void update();
	};
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////====================================================================================================================================////
///// | \file		CInputHandler.h
/////	| \author	Daniel Huc
///// | \date		April 2016
///// |----------------------------------------------------------------------------------------------------------------------------------
///// | \brief	Blabla
///// | \details	Blabla
//////====================================================================================================================================////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#pragma once
///* Standard library includes */
//#include <iostream>
//
///* External dependencies */
//#include <SDL2/SDL.h>
//
///* Internal headers includes */
//#include "HInternalTypesAliases.h"
//
////// ---- Namespaces ---- ////
//namespace PixPhetamine {
//
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	////====================================================================================================================================////
//	//// ClassName
//	/// \brief		Blabla
//	/// \details	Blabla
//	////====================================================================================================================================////
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	class CInputHandler {
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	//// ---- Members -----																													//// 
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	private:
//		SDL_Event event;
//		pxUInt m_mousePositionX{ 0 };
//		pxUInt m_mousePositionY{ 0 };
//		pxInt m_mouseMotionX{ 0 };
//		pxInt m_mouseMotionY{ 0 };
//		pxUInt m_endEvent{ 0 };
//		pxUInt m_moveBackward{ 0 };
//		pxUInt m_moveForward{ 0 };
//		pxUInt m_moveLeft{ 0 };
//		pxUInt m_moveRight{ 0 };
//		pxUInt m_moveUp{ 0 };
//		pxUInt m_moveDown{ 0 };
//		pxUInt m_shoot{ 0 };
//		pxUInt m_bulletTime{ 0 };
//		pxUInt m_pause{ 0 };
//
//		SDL_Window* m_window;
//		pxInt WINDOW_CENTER_X;
//		pxInt WINDOW_CENTER_Y;
//
//
//		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		//// ---- Methods -----																													////
//		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	private:
//		//// ---- Internal methods ---- ////
//
//	public:
//		//// ---- Constructor/Destructor ---- ////
//
//
//		//// ---- Operator ---- ////
//
//
//		//// ---- Getter ---- ////
//
//
//		//// ---- Setter ---- ////
//
//
//		//// ---- Core ---- ////
//
//
//		//// ---- Static ---- ////
//
//	};
//}
//
//
//
//
//class  {
//	/* Members */
//	private:
//		
//		
//	
//	/* Methods */
//		void keyPressed(SDL_KeyboardEvent* key);
//		void keyReleased(SDL_KeyboardEvent* key);
//		void mouseMoved(SDL_MouseMotionEvent* mouse);
//		void mousePressed(SDL_MouseButtonEvent* mouse);
//		void mouseReleased(SDL_MouseButtonEvent* mouse);
//		void incInputTime();
//	
//	public:
//		CInputHandler(SDL_Window* window);
//		pxUInt getMoveForward()		const { return m_moveForward; }
//		pxUInt getMoveBackward()	const { return m_moveBackward; }
//		pxUInt getMoveLeft()		const { return m_moveLeft; }
//		pxUInt getMoveRight()		const { return m_moveRight; }
//		pxUInt getMoveUp()		const { return m_moveUp; }
//		pxUInt getMoveDown()		const { return m_moveDown; }
//		pxInt getMouseMotionX()		{ return m_mouseMotionX; m_mouseMotionX = 0; }
//		pxInt getMouseMotionY()		{ return m_mouseMotionY; m_mouseMotionY = 0; }
//		pxUInt getShoot()			const { return m_shoot; }
//		pxUInt getBulletTime()		const { return m_bulletTime; }
//		pxUInt isNotQuit()			const { return !m_endEvent; }
//		pxUInt isNotPaused()		const { return !m_pause; }
//		
//		void update();
//};
