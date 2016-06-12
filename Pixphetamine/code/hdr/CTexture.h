/// \file		CTexture.h
///	\author		Daniel Huc
/// \date		June 2016
#pragma once

/* Standard library includes */

/* External dependencies */
#include <GL/glew.h>

/* Internal headers includes */
#include "HInternalTypesAliases.h"

namespace PixPhetamine {

	namespace LowLevelWrapper {

		enum TextureType {
			DEPTH,
			NORMAL,
			TRANSPARENT,
			HDR,
			HDR_TRANSPARENT
		};

		class CTexture {
		/* Members */
		private:
			pxUInt16		m_width{ 0 };	 ///< Texture width
			pxUInt16		m_height{ 0 }; ///< Texture height
			GLvramLocation	m_id{ 0 };
			pxBool			m_isMultisampled{ false };
			TextureType		m_textureType{ NORMAL };
		
		/* Methods */
		public:
			CTexture(pxUInt16 const& width, pxUInt16 const& height, TextureType const& textureType, pxBool const& willBeMultisampled);
			~CTexture();
			pxUInt16 getWidth() const { return m_width; }
			pxUInt16 getHeight() const { return m_height; }
			GLvramLocation getID() const { return m_id; }
		};
	}
}