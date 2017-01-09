/// \file		CShader.h
/// \author		Daniel Huc
/// \date		April 2016
#pragma once

/* Standard library includes */
#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>

/* External dependencies */
#include <GL/glew.h>
#include <string>
#include <map>

/* Internal headers includes */
#include "HInternalTypesAliases.h"
#include "UErrorHandler.h"

namespace PixPhetamine {

	namespace LowLevelWrapper {


		/// \brief		Shader loader
		/// \details	Provides a shader code loader, and error displayer (once loaded, you can just reload the shader)
		class CShader {
		/* Members */
		private:
			pxUInt m_id{ 0 };
			std::map<std::string, GLvramLocation> m_variableNames;
			std::string m_vertexFilePath;
			std::string m_fragmentFilePath;
			pxBool m_isRealodable{ false };

		/* Methods */
		private:
			// string containing the source code of the input file
			std::string getCode(const char * filePath) const;
			void load(std::string vertexCode, std::string fragmentCode);
			// call it after each shader compilation
			void checkCompilation(pxUInt shaderId) const;
			// call it after linking the program
			void checkLinks(pxUInt programId) const;
			// call before sending or binding var/name to make shure that the current program is this shader!
			void checkContext() const;

		public:
			CShader(const char* a_vertexPath, const char* a_fragmentPath);
			CShader(std::string vertexShaderName, std::string vertexCode, std::string fragmentShaderName, std::string fragmentCode);
			~CShader();

			void reload();
			inline pxUInt id() { return m_id; }

			void bindVariableName(const char * correspondingVariableNameInShader);
			GLvramLocation getVariableAttachment(const char * correspondingVariableNameInShader) { return m_variableNames[correspondingVariableNameInShader]; }

			template<typename SHADER_FRIENDLY_TYPE>
			void sendVariable(const char * correspondingVariableNameInShader, SHADER_FRIENDLY_TYPE const& variable);
		};
	}
}
