#include "CFrameBuffer.h"

namespace PixPhetamine {

	namespace PostProcess {

		void CFrameBuffer::checkFrameBufferErrors() {
			GLenum status;
			if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
				switch (status) {
				case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
					ERROR("Framebuffer error: Framebuffer incomplete, Attachment is NOT complete");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
					ERROR("Framebuffer error: Framebuffer incomplete, No image is attached to FBO");
					break;
				case GL_FRAMEBUFFER_UNSUPPORTED:
					ERROR("Framebuffer error: Unsupported FBO implementation");
					break;
				default:
					ERROR("Framebuffer error: Unknown error");
					break;
				}
			}
		}


		CFrameBuffer::CFrameBuffer(pxUInt16 const& a_width, pxUInt16 const& a_height, pxBool const& a_willBeMultisampled) {
			STACK_TRACE;
			m_isMultisampled = a_willBeMultisampled;
			m_width = a_width;
			m_height = a_height;

			glGenFramebuffers(1, &m_id);

			STACK_MESSAGE("Checking Framebuffer errors");
			checkFrameBufferErrors();
			UNSTACK_TRACE;
		}


		CFrameBuffer::~CFrameBuffer() {
			for (auto &it_texture : m_texture) {
				delete (it_texture.second);
			}
			glDeleteFramebuffers(1, &m_id);
		}


		void CFrameBuffer::addTexture(std::string const& a_textureName, LowLevelWrapper::ETextureType const& a_textureType) {
			STACK_TRACE;
			
			GLenum textureTarget;
			(m_isMultisampled) ? textureTarget = GL_TEXTURE_2D_MULTISAMPLE : textureTarget = GL_TEXTURE_2D;

			// Resolve attachment
			GLvramLocation attachment = GL_COLOR_ATTACHMENT0 + m_textureAttachment.size();
			if (a_textureType == LowLevelWrapper::DEPTH) {
				attachment = GL_DEPTH_ATTACHMENT;
			}

			// Create attachment
			m_textureAttachment[a_textureName] = attachment;

			// Create texture
			glActiveTexture(GL_TEXTURE0 + m_texture.size());
			m_texture[a_textureName] = new LowLevelWrapper::CTexture(m_width, m_height, a_textureType, m_isMultisampled);
			
			// Framebuffer link (we can perform it everytime, because addTexture will be called as pre-process (not in main game loop))
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);

			// Attachment
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textureTarget, m_texture[a_textureName]->getID(), 0);

			STACK_MESSAGE("Checking Framebuffer errors");
			checkFrameBufferErrors();
			STACK_MESSAGE("Checking Framebuffer errors [OK]");

			STACK_MESSAGE("Checking OpenGL errors");
			Utility::UErrorHandler::checkOpenGLErrors();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			
			UNSTACK_TRACE;
		}

		void CFrameBuffer::replaceTexture(const char* a_textureNameToReplace, CFrameBuffer* a_readenFrame, const char* a_remplacementTexture) {
			// FIXME: Will only work with non depth textures I guess (GL_LINEAR and stuff)
			STACK_TRACE;

			glBindFramebuffer(GL_READ_FRAMEBUFFER, a_readenFrame->getId());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->m_id);
			glClearColor(0.5, 0.5, 0.5, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			// Resolve color multisampling
			glReadBuffer(a_readenFrame->getTextureAttachment(a_remplacementTexture));
			glDrawBuffer(this->getTextureAttachment(a_textureNameToReplace));
			glBlitFramebuffer(0, 0, a_readenFrame->getWidth(), a_readenFrame->getHeight(), 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			STACK_MESSAGE("Checking Framebuffer errors");
			checkFrameBufferErrors();
			STACK_MESSAGE("Checking Framebuffer errors [OK]");

			STACK_MESSAGE("Checking OpenGL errors");
			Utility::UErrorHandler::checkOpenGLErrors();

			UNSTACK_TRACE;
		}
	}
}