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

		void CFrameBuffer::bindToFBO(GLvramLocation const& a_textureID, pxUInt16 const& a_attachment, pxBool const& a_isMultisampled) {
			STACK_TRACE;

			// Framebuffer link (we can perform it everytime, because addTexture will be called as pre-process (not in main game loop))
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);

			// Attachment
			if (a_isMultisampled)
				glFramebufferTexture2D(GL_FRAMEBUFFER, a_attachment, GL_TEXTURE_2D_MULTISAMPLE, a_textureID, 0);
			else
				glFramebufferTexture2D(GL_FRAMEBUFFER, a_attachment, GL_TEXTURE_2D, a_textureID, 0);

			STACK_MESSAGE("Checking Framebuffer errors");
			checkFrameBufferErrors();
			STACK_MESSAGE("Checking Framebuffer errors [OK]");

			STACK_MESSAGE("Checking OpenGL errors");
			Utility::UErrorHandler::checkOpenGLErrors();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			UNSTACK_TRACE;
		}

		CFrameBuffer::CFrameBuffer(pxUInt16 const& a_width, pxUInt16 const& a_height, EType const& a_frameBufferType) {
			STACK_TRACE;

			m_type = a_frameBufferType;
			m_width = a_width;
			m_height = a_height;
			m_isMultisampled = (a_frameBufferType == EType::MULTISAMPLED || a_frameBufferType == EType::WITH_DEPTH_MULTISAMPLED);
			m_withDepth = (a_frameBufferType == EType::WITH_DEPTH || a_frameBufferType == EType::WITH_DEPTH_MULTISAMPLED);

			glGenFramebuffers(1, &m_id);

			// Generation of the depth buffer here, as only one depth buffer is bindable:
			if (m_withDepth) {
				m_texture[M_DEPTH_TEXTURE_NAME] = std::unique_ptr<LowLevelWrapper::CTexture>(
					new LowLevelWrapper::CTexture(m_width, m_height, LowLevelWrapper::CTexture::ETextureType::DEPTH, m_isMultisampled)
				);
				bindToFBO(m_texture[M_DEPTH_TEXTURE_NAME]->getID(), GL_DEPTH_ATTACHMENT, m_isMultisampled);
			}

			UNSTACK_TRACE;
		}


		CFrameBuffer::~CFrameBuffer() {
			glDeleteFramebuffers(1, &m_id);
		}

		void CFrameBuffer::addTexture(std::string const& a_textureName, LowLevelWrapper::CTexture::ETextureType const& a_textureType, pxUInt16 const& a_attachment) {
			STACK_TRACE;

			if (a_textureType == LowLevelWrapper::CTexture::ETextureType::DEPTH) {
				ERROR("Error! Adding '" + a_textureName + "'  Don't add explicitely DEPTH textures into the FrameBuffer!");
			}

			// Resolve attachment
			GLvramLocation attachment = GL_COLOR_ATTACHMENT0 + a_attachment;

			// Create attachment
			m_textureAttachment[a_textureName] = attachment;

			// Create texture
			m_texture[a_textureName] = std::unique_ptr<LowLevelWrapper::CTexture>(
				new LowLevelWrapper::CTexture(m_width, m_height, a_textureType, m_isMultisampled)
			);

			// Bind it to this Frame Buffer!
			bindToFBO(m_texture[a_textureName]->getID(), a_attachment, m_isMultisampled);
		}

		void CFrameBuffer::blit(CFrameBuffer* a_destination, std::initializer_list<SBlit> a_blits) {
			STACK_TRACE;

			if (a_blits.size() == 0) {
				ERROR("Error! To perform a blit between two FBOs, you must at least have one couple of textures to blit!");
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0); // FIXME : Is this needed?
			glBindFramebuffer(GL_READ_FRAMEBUFFER, m_id);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, a_destination->getId());

			//glClearColor(0.5, 0.5, 0.5, 1.0);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST); // FIXME: Is this needed?

			for (auto &it_blit : a_blits) {
				if (!m_textureAttachment.count(it_blit.textureToReadFrom) && !m_textureAttachment.count(it_blit.textureToWriteInto)) {
					ERROR("Error! On bind: '" + it_blit.textureToReadFrom + "' or '" + it_blit.textureToWriteInto + "' not found!");
				}
				
				
				// Special blit for depth! 
				if (it_blit.textureToReadFrom == M_DEPTH_TEXTURE_NAME) {
					//If this one is depth, the other one must be depth to!
					if (it_blit.textureToWriteInto != M_DEPTH_TEXTURE_NAME) {
						ERROR("Error! Try to blit depth texture into something else");
					}
					// As I've understood, no need to specify attachment since there can be only one depth attachment!
					glBlitFramebuffer(0, 0, a_destination->getWidth(), a_destination->getHeight(), 0, 0, m_width, m_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
				}
				else {
					if (it_blit.textureToWriteInto == M_DEPTH_TEXTURE_NAME) {
						ERROR("Error! Try to blit into depth texture from something else");
					}
					glReadBuffer(m_textureAttachment[it_blit.textureToReadFrom]);
					glDrawBuffer(a_destination->getTextureAttachment(it_blit.textureToWriteInto));

					glBlitFramebuffer(0, 0, a_destination->getWidth(), a_destination->getHeight(), 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
				}

				STACK_MESSAGE("Checking OpenGL errors");
				Utility::UErrorHandler::checkOpenGLErrors();
			}
		}
	}
}