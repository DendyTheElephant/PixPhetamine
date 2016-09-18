/// \file		CPostProcessPass.h
/// \author		Daniel Huc
/// \date		June 2016
#pragma once

/* Standard library includes */
#include <string>
#include <vector>

/* External dependencies */
#include <GL/glew.h>

/* Internal headers includes */
#include "HInternalTypesAliases.h"
#include "UErrorHandler.h"
#include "CFrameBuffer.h"
#include "CShader.h"
#include "CTexture.h"
#include "SMeshQuad.h"

namespace PixPhetamine {

	/// This part of the program is in charge of the post process pipeline
	namespace PostProcess {

		class CPostProcessPass {
		/* Members */
		private:
			LowLevelWrapper::CShader*		m_shader{ nullptr };
			CFrameBuffer*					m_frameBuffer{ nullptr };
			LowLevelWrapper::SMeshQuad		m_quad;

		/* Methods */
		public:
			CPostProcessPass(LowLevelWrapper::CShader* shader, CFrameBuffer* frameBuffer) { 
				m_shader = shader; m_frameBuffer = frameBuffer;
			}
			~CPostProcessPass() {/***/}

			void bindVariableName(const char* correspondingVariableNameInShader) { m_shader->bindVariableName(correspondingVariableNameInShader); }

			// Process
			void activate() const { glUseProgram(m_shader->id()); }
			void sendTexture(LowLevelWrapper::CTexture* textureToSend, const char* correspondingVariableNameInShader, pxUInt16 textureUnitLocation);
			void process(pxUInt16 a_numberOfOutputTextures, std::string outputTextureName ...);
			template<typename SHADER_FRIENDLY_TYPE>
			void sendVariable(const char * correspondingVariableNameInShader, SHADER_FRIENDLY_TYPE const& variable) {
				m_shader->sendVariable(correspondingVariableNameInShader, variable);
			}
		};
	}
}