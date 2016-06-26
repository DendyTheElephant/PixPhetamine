/// \file		CPostProcessPass.h
///	\author		Daniel Huc
/// \date		June 2016
#pragma once

/* Standard library includes */
#include <string>

/* External dependencies */
#include <GL/glew.h>

/* Internal headers includes */
#include "HInternalTypesAliases.h"
#include "UErrorHandler.h"
#include "CFrameBuffer.h"
#include "CShader.h"

namespace PixPhetamine {

	/// This part of the program is in charge of the post process pipeline
	namespace PostProcess {

		class CPostProcessPass {
		/* Members */
		private:
			LowLevelWrapper::CShader*		m_shader{ nullptr };
			LowLevelWrapper::CFrameBuffer*	m_inputFrame{ nullptr };
			LowLevelWrapper::CFrameBuffer*	m_processedFrame{ nullptr };

		/* Methods */
		public:
			CPostProcessPass();
			~CPostProcessPass();

			void setShader(LowLevelWrapper::CShader* shader) { m_shader = shader; }
			
			void bindInTexture(std::string correspondingVariableNameInShader, std::string inTextureName);
			void bindOutTexture(std::string correspondingVariableNameInShader, std::string outTextureName);

			void process();
		};
	}
}