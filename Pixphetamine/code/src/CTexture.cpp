#include "CTexture.h"

namespace PixPhetamine {

	namespace LowLevelWrapper {

		void CTexture::initializeTexture(pxUInt16 const& a_width, pxUInt16 const& a_height, TextureType const& a_textureType, pxBool const& a_willBeMultisampled, const void* a_data) {
			m_textureType = a_textureType;
			m_isMultisampled = a_willBeMultisampled;
			m_width = a_width;
			m_height = a_height;

			GLenum target;
			GLint internalFormat;
			GLenum format;
			const void* data;
			(a_data == nullptr) ? data = a_data : data = NULL;

			switch (a_textureType) {
			case NORMAL:
				internalFormat = GL_RGB;
				format = GL_RGB;
				break;
			case TRANSPARENT:
				internalFormat = GL_RGBA;
				format = GL_RGBA;
				break;
			case HDR:
				internalFormat = GL_RGB16F;
				format = GL_RGB;
				break;
			case HDR_TRANSPARENT:
				internalFormat = GL_RGBA16F;
				format = GL_RGBA;
				break;
			case DEPTH:
				internalFormat = GL_DEPTH_COMPONENT24;
				format = GL_DEPTH_COMPONENT;
				break;
			}

			(a_willBeMultisampled) ? target = GL_TEXTURE_2D_MULTISAMPLE : target = GL_TEXTURE_2D;

			glGenTextures(1, &m_id);
			glBindTexture(target, m_id);
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);

			if (a_willBeMultisampled) {
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, internalFormat, a_width, a_height, GL_TRUE);
			}
			else {
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, a_width, a_height, 0, format, GL_FLOAT, data);
			}

			glBindTexture(target, 0);
		}

		CTexture::CTexture(std::string const& a_texturePath, TextureType const& a_textureType, pxBool const& a_willBeMultisampled) {
			STACK_TRACE;
			SDL_Surface *image = IMG_Load(a_texturePath.c_str());
			if (image == NULL) {
				STACK_TRACE;
				ERROR("Failed to load image: " + a_texturePath + " please check if it exists");
			}
			initializeTexture(image->w, image->h, a_textureType, a_willBeMultisampled, image->pixels);
			SDL_FreeSurface(image);
			UNSTACK_TRACE;
		}

		CTexture::CTexture(pxUInt16 const& a_width, pxUInt16 const& a_height, TextureType const& a_textureType, pxBool const& a_willBeMultisampled) {
			initializeTexture(a_width, a_height, a_textureType, a_willBeMultisampled, nullptr);
		}

		CTexture::~CTexture() {
			glDeleteTextures(1, &m_id);
		}
	}
}