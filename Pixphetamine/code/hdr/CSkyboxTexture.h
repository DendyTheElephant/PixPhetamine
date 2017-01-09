/// \file		CSkyboxTexture.h
///	\author		Daniel Huc
/// \date		January 2017
#pragma once

/* Standard library includes */

/* External dependencies */
#include <GL/glew.h>
#include <SDL_image.h>

/* Internal headers includes */
#include "HInternalTypesAliases.h"
#include "UErrorHandler.h"

namespace PixPhetamine {

	namespace LowLevelWrapper {

		class CSkyboxTexture {
			/* Members */
		private:
			GLvramLocation	m_id{ 0 };		///< ID of the Texture, used to reference the active Tetxure in the rendering pipeline
			pxUInt16		m_width{ 0 };	///< Texture width
			pxUInt16		m_height{ 0 };	///< Texture height

			/* Methods */
		public:
			CSkyboxTexture(std::vector<std::string> skyboxTexturePaths);
			~CSkyboxTexture();
			pxUInt16 getWidth() const { return m_width; }
			pxUInt16 getHeight() const { return m_height; }
			GLvramLocation getID() const { return m_id; }
		};
	}
}