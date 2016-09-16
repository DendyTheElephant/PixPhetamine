/// \file		CCamera.h
///	\author		Daniel Huc
/// \date		April 2016
#pragma once

/* Standard library includes */
#include <iostream>
#include <math.h>       /* fmod */

/* External dependencies */
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* Internal headers includes */
#include "HInternalTypesAliases.h"

namespace PixPhetamine {
	/// \brief		Camera class
	/// \details	Handling the camera and creates the ViewProjection matrix is easier with this abstraction! \n
	///				Usage: set the OpenGL camera parameters at the instanciation (static values)\n
	///				then move this camera with basic primitives (move view along axis, move view by angles, set position) \n
	///				finally, get the ViewProjection matrix, it's simple, just ask!
	class CCamera {
	/* Members */
	private:
		pxFloat m_viewAngle{ 70.0f };
		const pxFloat MIN_FIELD_OF_VIEW{ 0.1f };
		const pxFloat MAX_FIELD_OF_VIEW{ 1000.0f };
		const pxVec3f Y_AXIS{ 0.0f, 1.0f, 0.0f };
		pxFloat m_width;
		pxFloat m_height;

		pxFloat m_angleX{ 180.0f };
		pxFloat m_angleY{ 0.0f };
		pxVec3f m_position{ 0.0f, 0.0f, 5.0f };
		pxVec3f m_sight{ 0.0f, 0.0f, 4.0f };
		pxVec3f m_direction{ 0.0f, 0.0f, -1.0f };
		pxVec3f m_strafe;

		pxMat4f m_ProjectionMatrix;
		pxMat4f m_ViewMatrix;

	/* Methods */
	public:
		/// \brief		Constructor
		/// \details    Provide the window context, and change initialisation values in the header file (static init).
		/// \param		window		SDL_Window to provide context.
		CCamera(SDL_Window* window);
		void setFOV(const pxFloat &newAngle);
		void moveCameraForward(const pxFloat &distance);
		void moveCameraBackward(const pxFloat &distance);
		void moveCameraLeft(const pxFloat &distance);
		void moveCameraRight(const pxFloat &distance);
		void moveCameraUp(const pxFloat &distance);
		void moveCameraDown(const pxFloat &distance);
		void setPosition(const pxVec3f &newPosition);
		void moveView(const pxFloat &dx, const pxFloat &dy);
		pxMat4f getViewProjectionMatrix();
		pxVec3f getPosition() const { return m_position; }
		pxVec3f getView() const { return m_direction; }

	};
}