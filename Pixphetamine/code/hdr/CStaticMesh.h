#pragma once

/* Standard library includes */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

/* External dependencies */
#include <GL/glew.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

/* Internal headers includes */
#include "HInternalTypesAliases.h"
#include "LowLevelRenderingWrapper.h"
#include "UErrorHandler.h"

namespace PixPhetamine {
	class CStaticMesh {
	/* Members */
	private:
		PixPhetamine::LowLevelWrapper::VAO*		m_VAO;
		pxFloatArray							m_vertices;
		pxFloatArray							m_normals;
		pxFloatArray							m_colors;
		pxUInt16Array							m_faces;

	/* Methods */
	public:
		CStaticMesh(const char* objectPath);// throw (Exception::FileNotFound_Exception);
		~CStaticMesh();

		GLvramLocation	getVBO() const				{ return m_VAO->id; }
		pxUInt			getNumberOfFaces() const	{ return m_faces.size(); }
	};
}
