/// \file		CFrameBuffer.h
///	\author		Daniel Huc
/// \date		June 2016
#pragma once

/* Standard library includes */
#include <iostream>
#include <map>
#include <string>

/* External dependencies */
#include <GL/glew.h>

/* Internal headers includes */
#include "HInternalTypesAliases.h"
#include "UErrorHandler.h"
#include "CTexture.h"

namespace PixPhetamine {

	namespace LowLevelWrapper {

		class CFrameBuffer {
		/* Members */
		private:
			pxUInt16							m_width{ 0 };				///< Frame width
			pxUInt16							m_height{ 0 };				///< Frame height
			GLvramLocation						m_id{ 0 };					///< ID of the FBO, used to reference the active FBO in the rendering pipeline
			std::map<std::string, CTexture*>	m_texture;					///< Texture map
			std::map<std::string, GLenum>		m_textureAttachment;		///< Updated with m_texture but stores the attachment in the FBO of the texture
			pxBool								m_isMultisampled{ false };

		/* Methods */
		private:
			void checkFrameBufferErrors();
		public:
			CFrameBuffer(pxUInt16 const& width, pxUInt16 const& height, pxBool const& willBeMultisampled);
			~CFrameBuffer();
			void addTexture(std::string const& textureName, TextureType const& textureType);
			GLvramLocation getId() const { return m_id; }
			CTexture* getTexture(std::string &const textureName) { return m_texture[textureName]; }
			GLenum getTextureAttachment(std::string &const textureName) { return m_textureAttachment[textureName]; }
			void resize(pxUInt16 width, pxUInt16 height, pxBool willBeInterpolated);
		};
	}
}