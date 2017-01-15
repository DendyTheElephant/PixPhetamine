/// \file		CFrameBuffer.h
///	\author		Daniel Huc
/// \date		June 2016
#pragma once

/* Standard library includes */
#include <map>
#include <string>
#include <memory>

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
		public:
			enum class EType {
				NORMAL,
				WITH_DEPTH,
				MULTISAMPLED,				///< Required for texture generation: is it multisampled texture?
				WITH_DEPTH_MULTISAMPLED		
			};
			/// Used for blit() function!
			struct SBlit {
				std::string textureToReadFrom{ "" };
				std::string textureToWriteInto{ "" };
				SBlit(std::string textureToReadFrom, std::string texturetoWriteInto) : textureToReadFrom(textureToReadFrom), textureToWriteInto(textureToWriteInto) { /***/ }
			};
		private:
			const std::string													M_DEPTH_TEXTURE_NAME{ "depth_texture" };
			GLvramLocation														m_id{ 0 };					///< ID of the FBO, used to reference the active FBO in the rendering pipeline
			pxUInt16															m_width{ 0 };				///< Frame width
			pxUInt16															m_height{ 0 };				///< Frame height
			pxBool																m_isMultisampled{ false };
			pxBool																m_withDepth{ false };
			std::map<std::string, std::unique_ptr<LowLevelWrapper::CTexture>>	m_texture;					///< Texture map
			std::map<std::string, pxUInt16>										m_textureAttachment;		///< Updated with m_texture but stores the attachment in the FBO of the texture
			EType																m_type;
		
		/* Methods */
		private:
			void checkFrameBufferErrors();
			void bindToFBO(GLvramLocation const& textureID, pxUInt16 const& attachment, pxBool const& isMultisampled);

		public:
			CFrameBuffer(pxUInt16 const& width, pxUInt16 const& height, EType const& frameBufferType);
			~CFrameBuffer();

			void addTexture(std::string const& textureName, LowLevelWrapper::CTexture::ETextureType const& textureType, pxUInt16 const& attachment);
			void blit(CFrameBuffer* destination, std::initializer_list<SBlit> blits);

			GLvramLocation getId() const { return m_id; }
			pxUInt16 getWidth() const { return m_width; }
			pxUInt16 getHeight() const { return m_height; }
			LowLevelWrapper::CTexture* getTexture(std::string textureName) { 
				STACK_TRACE;
				if (m_texture.count(textureName) == 0)
					ERROR(std::string(textureName) + " not found!");
				UNSTACK_TRACE;
				return m_texture[textureName].get(); 
			}
			GLenum getTextureAttachment(std::string textureName) { 
				STACK_TRACE;
				if (m_textureAttachment.count(textureName) == 0) 
					ERROR(std::string(textureName)+" not found!"); 
				UNSTACK_TRACE;
				return m_textureAttachment[textureName]; 
			}
			const std::string& depthName() {
				return M_DEPTH_TEXTURE_NAME;
			}
		};
	}
}