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
#include "FBasicMeshes.h"

namespace PixPhetamine {

	/// This part of the program is in charge of the post process pipeline
	namespace PostProcess {

		class CPostProcessPass {
		/* Members */
		private:
			LowLevelWrapper::CShader*			m_shader{ nullptr };
			CFrameBuffer*						m_frameBuffer{ nullptr };
			LowLevelWrapper::CBasicMeshQuad		m_quad;
			struct STextureUnit {
				pxUInt16 binding;
				GLvramLocation textureID;
				pxBool isMultisampled;
				STextureUnit(pxUInt16 a_binding, GLvramLocation a_textureID, pxBool a_isMultisampled)
					:binding(a_binding), textureID(a_textureID), isMultisampled(a_isMultisampled) { /***/ }
			}; ///< In texture unit bind, Corresponding texture's ID
			std::vector<STextureUnit>			m_inTextureUnits;			

		/* Methods */
		private:
			void checkContext() const;

		public:
			CPostProcessPass(LowLevelWrapper::CShader* shader, CFrameBuffer* frameBuffer = nullptr) { 
				m_shader = shader; m_frameBuffer = frameBuffer;
			}
			~CPostProcessPass() { /***/ }

			void bindVariableName(const char* correspondingVariableNameInShader) { m_shader->bindVariableName(correspondingVariableNameInShader); }

			// Process
			void bindTexture(LowLevelWrapper::CTexture* textureToBind, const char* correspondingVariableNameInShader, pxUInt16 textureUnitLocation);
			void process(std::initializer_list<const char*> textureTargets = {});
			template<typename SHADER_FRIENDLY_TYPE>
			void sendVariable(const char * correspondingVariableNameInShader, SHADER_FRIENDLY_TYPE const& variable) {
				m_shader->sendVariable(correspondingVariableNameInShader, variable);
			}
		};
	}
}