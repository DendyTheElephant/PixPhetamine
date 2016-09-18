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
			LowLevelWrapper::CFrameBuffer*	m_inputFrame{ nullptr };
			LowLevelWrapper::CFrameBuffer*	m_processedFrame{ nullptr };
			LowLevelWrapper::SMeshQuad		m_quad;

		/* Methods */
		public:
			CPostProcessPass(LowLevelWrapper::CShader* shader, LowLevelWrapper::CFrameBuffer* inputFrame, LowLevelWrapper::CFrameBuffer* outputFrame) { 
				m_shader = shader; m_inputFrame = inputFrame; m_processedFrame = outputFrame; 
			}
			~CPostProcessPass() {/***/}

			void bindVariableName(const char* correspondingVariableNameInShader) { glGetError(); m_shader->bindVariableName(correspondingVariableNameInShader); }

			// Process
			void sendTexture(const char * correspondingVariableNameInShader, const char * textureName, pxUInt16 location);
			void process();
			template<typename SHADER_FRIENDLY_TYPE>
			void sendVariable(const char * correspondingVariableNameInShader, SHADER_FRIENDLY_TYPE const& variable) {
				m_shader->sendVariable(correspondingVariableNameInShader, variable);
			}
		};
	}
}