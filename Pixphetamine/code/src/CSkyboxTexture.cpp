#include "CSkyboxTexture.h"

namespace PixPhetamine {

	namespace LowLevelWrapper {

		CSkyboxTexture::CSkyboxTexture(std::vector<std::string> a_skyboxTexturePaths) {
			STACK_TRACE;

			glActiveTexture(GL_TEXTURE0); //MIGHT BE NEEDED?

			glGenTextures(1, &m_id);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

			for (pxUInt16 i_face = 0; i_face < 6; ++i_face) {
				SDL_Surface *image = IMG_Load(a_skyboxTexturePaths[i_face].c_str());
				if (image == nullptr) {
					STACK_TRACE;
					ERROR("Failed to load image: " + a_skyboxTexturePaths[i_face] + " please check if it exists");
				}
				m_width = image->w;
				m_height = image->h;
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i_face, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
				SDL_FreeSurface(image);
			}

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

			Utility::UErrorHandler::checkOpenGLErrors();
			UNSTACK_TRACE;
		}

		CSkyboxTexture::~CSkyboxTexture() {
			glDeleteTextures(1, &m_id);
		}
	}
}