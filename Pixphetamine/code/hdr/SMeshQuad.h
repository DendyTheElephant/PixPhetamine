/// \file		SMeshQuad.h
///	\author		Daniel Huc, Henry Lefèvre
/// \date		September 2016
#pragma once

/* External dependencies */
#include <GL/glew.h>

/* Internal headers includes */
#include "HInternalTypesAliases.h"
#include "UErrorHandler.h"

namespace PixPhetamine {

	namespace LowLevelWrapper {

		/// \brief		Geometry used for screen space rendering (post processing)
		/// \details	Allocate the geometry for a Quad in VRAM
		struct SMeshQuad {
			GLvramLocation VAO_id{ 0 };				///< ID of the VAO, used to reference the active VBO in the rendering pipeline
			GLvramLocation VBO_Vertices{ 0 };		///< Buffer containing the vertices of the quad

			SMeshQuad();
			~SMeshQuad();
		};
	}
}