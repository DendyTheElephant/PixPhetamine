/// \file		CTexture.h
///	\author		Daniel Huc
/// \date		June 2016
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

		/// Texture type description
		enum ETextureType {
			DEPTH,				///< Used to save the depth component of a render pass (depth intensity (black-white))
			NORMAL,				///< Used to save the colors of a render pass will be clamped between [0,1] as floats in RGB space
			TRANSPARENT,		///< Used to save the colors of a render pass with transparency, will  be clamped between [0,1] as floats in RGBA space
			HDR,				///< Used to save the colors of a render pass will not be clamped, as floats in RGB space
			HDR_TRANSPARENT		///< Used to save the colors of a render pass with transparency, will not be clamped between, as floats in RGBA space
		};

		class CTexture {
		/* Members */
		private:
			pxUInt16		m_width{ 0 };				///< Texture width
			pxUInt16		m_height{ 0 };				///< Texture height
			GLvramLocation	m_id{ 0 };					///< ID of the Texture, used to reference the active Tetxure in the rendering pipeline
			pxBool			m_isMultisampled{ false };	///< If is multisampled, AA filters can be applied using interpolation
			ETextureType	m_textureType{ NORMAL };	///< Texture type, see ETextureType to know more about textures
		
		/* Methods */
		private:
			void initializeTexture(pxUInt16 const& width, pxUInt16 const& height, ETextureType const& textureType, pxBool const& willBeMultisampled, const void* data);

		public:
			CTexture(std::string const& texturePath, ETextureType const& textureType, pxBool const& willBeMultisampled);
			CTexture(pxUInt16 const& width, pxUInt16 const& height, ETextureType const& textureType, pxBool const& willBeMultisampled);
			~CTexture();
			pxUInt16 getWidth() const { return m_width; }
			pxUInt16 getHeight() const { return m_height; }
			GLvramLocation getID() const { return m_id; }
			ETextureType getType() const { return m_textureType; }
			pxBool isMultisampled() const { return m_isMultisampled; }
		};
	}
}