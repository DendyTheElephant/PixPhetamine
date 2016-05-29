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


namespace Render {
	/// \brief		Camera class
	/// \details	Handling the camera and creates the ViewProjection matrix is easier with this abstraction! \n
	///				Usage: set the OpenGL camera parameters at the instanciation (static values)\n
	///				then move this camera with basic primitives (move view along axis, move view by angles, set position) \n
	///				finally, get the ViewProjection matrix, it's simple, just ask!
	class CCamera {
		/* Members */
	private:
		float m_viewAngle{ 70.0f };
		const float MIN_FIELD_OF_VIEW{ 0.1f };
		const float MAX_FIELD_OF_VIEW{ 1000.0f };
		const glm::vec3 Y_AXIS{ 0.0f, 1.0f, 0.0f };
		float m_width;
		float m_height;

		float m_angleX{ 180.0f };
		float m_angleY{ 0.0f };
		glm::vec3 m_position{ 0.0f, 0.0f, 5.0f };
		glm::vec3 m_sight{ 0.0f, 0.0f, 4.0f };
		glm::vec3 m_direction{ 0.0f, 0.0f, -1.0f };
		glm::vec3 m_strafe;

		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;

		/* Methods */
	public:
		/// \brief		Constructor
		/// \details    Provide the window context, and change initialisation values in the header file (static init).
		/// \param		window		SDL_Window to provide context.
		CCamera(SDL_Window* window);
		void setFOV(const float &newAngle);
		void moveCameraForward(const float &distance);
		void moveCameraBackward(const float &distance);
		void moveCameraLeft(const float &distance);
		void moveCameraRight(const float &distance);
		void setPosition(const glm::vec3 &newPosition);
		void moveView(const float &dx, const float &dy);
		glm::mat4 getViewProjectionMatrix();
		glm::vec3 getPosition() const { return m_position; }
		glm::vec3 getView() const { return m_direction; }

	};
}