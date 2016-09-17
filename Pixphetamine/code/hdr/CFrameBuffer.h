/// \file		CFrameBuffer.h
///	\author		Daniel Huc
/// \date		June 2016
#pragma once

/* Standard library includes */
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
			std::map<std::string, CTexture*>	m_depthTexture;				///< Depth texture map
			std::map<std::string, GLenum>		m_textureAttachment;		///< Updated with m_texture but stores the attachment in the FBO of the texture
			std::map<std::string, GLenum>		m_depthTextureAttachment;	///< As depth attachment has a singular value, we want to differentiate it!
			pxBool								m_isMultisampled{ false };	///< Required for texture generation: is it multisampled texture?

		/* Methods */
		private:
			void checkFrameBufferErrors();
		public:
			CFrameBuffer(pxUInt16 const& width, pxUInt16 const& height, pxBool const& willBeMultisampled);
			~CFrameBuffer();
			void addTexture(std::string const& textureName, ETextureType const& textureType);
			GLvramLocation getId() const { return m_id; }
            CTexture* getTexture(const std::string & textureName) { return m_texture[textureName]; }
            GLenum getTextureAttachment(const std::string & textureName) { return m_textureAttachment[textureName]; }
			//void resize(pxUInt16 width, pxUInt16 height, pxBool willBeInterpolated); might be downsample?
		};
	}
}
