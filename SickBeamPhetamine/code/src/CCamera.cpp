#include "CCamera.h"

namespace Render {
	CCamera::CCamera(SDL_Window* window) {
		int width, height;
		SDL_GetWindowSize(window, &width, &height);
		m_width = width;
		m_height = height;

		m_direction = glm::normalize(m_direction);

		m_strafe = glm::cross(Y_AXIS, m_direction);
		m_strafe = glm::normalize(m_strafe);
	}


	void CCamera::setFOV(const float &newAngle) {
		m_viewAngle = newAngle;
	}


	void CCamera::moveCameraForward(const float &distance) {
		m_position = m_position + m_direction * distance;
		m_sight = m_position + m_direction;
	}


	void CCamera::moveCameraBackward(const float &distance) {
		m_position = m_position - m_direction * distance;
		m_sight = m_position + m_direction;
	}


	void CCamera::moveCameraLeft(const float &distance) {
		m_position = m_position + m_strafe * distance;
		m_sight = m_position + m_direction;
	}


	void CCamera::moveCameraRight(const float &distance) {
		m_position = m_position - m_strafe * distance;
		m_sight = m_position + m_direction;
	}


	void CCamera::setPosition(const glm::vec3 &newPosition) {
		m_position = newPosition;
		m_sight = m_position + m_sight;
	}


	void CCamera::moveView(const float &dx, const float &dy) {

		m_angleX -= dx / 5.0;
		m_angleY -= dy / 5.0;

		m_angleX = fmod(m_angleX, 360.0f);
		m_angleY = fmod(m_angleY, 360.0f);

		//std::cout << "Angle X = " << m_angleX << "   Angle Y = " << m_angleY << std::endl;

		// Limitation Y
		m_angleY = glm::clamp(m_angleY, -89.0f, 89.0f);

		float radAngleX = glm::radians(m_angleX);
		float radAngleY = glm::radians(m_angleY);

		// Spherical resolution
		m_direction.x = glm::cos(radAngleY) * glm::sin(radAngleX);
		m_direction.y = glm::sin(radAngleY);
		m_direction.z = glm::cos(radAngleY) * glm::cos(radAngleX);


		// Sight is :
		m_sight = m_position + m_direction;

		m_strafe = glm::cross(Y_AXIS, m_direction);
		m_strafe = glm::normalize(m_strafe);
	}


	glm::mat4 CCamera::getViewProjectionMatrix() {
		m_ProjectionMatrix = glm::perspective((m_viewAngle), m_width / m_height, MIN_FIELD_OF_VIEW, MAX_FIELD_OF_VIEW);
		m_ViewMatrix = glm::lookAt(m_position, m_sight, Y_AXIS);
		return m_ProjectionMatrix * m_ViewMatrix;
	}
}