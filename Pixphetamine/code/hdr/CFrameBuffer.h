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

	/// This part of the program is in charge of the post process pipeline
	namespace PostProcess {

		class CFrameBuffer {
		/* Members */
		private:
			pxUInt16											m_width{ 0 };				///< Frame width
			pxUInt16											m_height{ 0 };				///< Frame height
			GLvramLocation										m_id{ 0 };					///< ID of the FBO, used to reference the active FBO in the rendering pipeline
			std::map<std::string, LowLevelWrapper::CTexture*>	m_texture;					///< Texture map
			std::map<std::string, GLenum>						m_textureAttachment;		///< Updated with m_texture but stores the attachment in the FBO of the texture
			pxBool												m_isMultisampled{ false };	///< Required for texture generation: is it multisampled texture?

		/* Methods */
		private:
			void checkFrameBufferErrors();
		public:
			CFrameBuffer(pxUInt16 const& width, pxUInt16 const& height, pxBool const& willBeMultisampled);
			~CFrameBuffer();

			void addTexture(std::string const& textureName, LowLevelWrapper::ETextureType const& textureType);
			void replaceTexture(const char* textureNameToReplace, CFrameBuffer* readenFrame, const char* remplacementTexture);

			GLvramLocation getId() const { return m_id; }
			pxUInt16 getWidth() const { return m_width; }
			pxUInt16 getHeight() const { return m_height; }
			LowLevelWrapper::CTexture* getTexture(const char* textureName) { 
				STACK_TRACE;
				if (m_texture.count(textureName) == 0)
					ERROR(std::string(textureName) + " not found!");
				UNSTACK_TRACE;
				return m_texture[textureName]; 
			}
			GLenum getTextureAttachment(const char* textureName) { 
				STACK_TRACE;
				if (m_textureAttachment.count(textureName) == 0) 
					ERROR(std::string(textureName)+" not found!"); 
				UNSTACK_TRACE;
				return m_textureAttachment[textureName]; }
		};
	}
}