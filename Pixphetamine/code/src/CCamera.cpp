#include "CCamera.h"

using namespace MathPhetamine;

namespace PixPhetamine {
	CCamera::CCamera(SDL_Window* a_window) {
		pxInt width, height;
		SDL_GetWindowSize(a_window, &width, &height);
		m_width		= static_cast<pxFloat>(width);
		m_height	= static_cast<pxFloat>(height);

		m_direction = normalize(m_direction);

		m_strafe = cross(Y_AXIS, m_direction);
		m_strafe = normalize(m_strafe);
	}


	void CCamera::setFOV(pxFloat const& a_newAngle) {
		m_viewAngle = a_newAngle;
	}


	void CCamera::moveCameraForward(pxFloat const& a_distance) {
		m_position	= m_position + m_direction * a_distance;
		m_sight		= m_position + m_direction;
	}


	void CCamera::moveCameraBackward(pxFloat const& a_distance) {
		m_position	= m_position - m_direction * a_distance;
		m_sight		= m_position + m_direction;
	}


	void CCamera::moveCameraLeft(pxFloat const& a_distance) {
		m_position	= m_position + m_strafe * a_distance;
		m_sight		= m_position + m_direction;
	}


	void CCamera::moveCameraRight(pxFloat const& a_distance) {
		m_position	= m_position - m_strafe * a_distance;
		m_sight		= m_position + m_direction;
	}


	void CCamera::moveCameraUp(pxFloat const& a_distance) {
		m_position = m_position + Y_AXIS * a_distance;
		m_sight = m_position + m_direction;
	}


	void CCamera::moveCameraDown(pxFloat const& a_distance) {
		m_position = m_position - Y_AXIS * a_distance;
		m_sight = m_position + m_direction;
	}


	void CCamera::setPosition(pxVec3f const& a_newPosition) {
		m_position	= a_newPosition;
		m_sight		= m_position + m_sight;
	}


	void CCamera::moveView(pxFloat const& a_dx, pxFloat const& a_dy) {
		m_angleX -= a_dx / 5.0f;
		m_angleY -= a_dy / 5.0f;

		m_angleX = mod(m_angleX, 360.0f);
		m_angleY = mod(m_angleY, 360.0f);

		//std::cout << "Angle X = " << m_angleX << "   Angle Y = " << m_angleY << std::endl;

		// Limitation Y
		m_angleY = clamp(m_angleY, -89.0f, 89.0f);

		pxFloat radAngleX = toRadians(m_angleX);
		pxFloat radAngleY = toRadians(m_angleY);

		// Spherical resolution
		m_direction.x = glm::cos(radAngleY) * glm::sin(radAngleX);
		m_direction.y = glm::sin(radAngleY);
		m_direction.z = glm::cos(radAngleY) * glm::cos(radAngleX);


		// Sight is :
		m_sight = m_position + m_direction;

		m_strafe = glm::cross(Y_AXIS, m_direction);
		m_strafe = glm::normalize(m_strafe);
	}


	pxMat4f CCamera::getViewProjectionMatrix() {
		m_ProjectionMatrix = glm::perspective((m_viewAngle), m_width / m_height, MIN_DEPTH_OF_FIELD, MAX_DEPTH_OF_FIELD);
		m_ViewMatrix = glm::lookAt(m_position, m_sight, Y_AXIS);
		return m_ProjectionMatrix * m_ViewMatrix;
	}
}