/// \file		CShader.h
///	\author		Daniel Huc
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

		/* Methods */
		private:
			// string containing the source code of the input file
            std::string getCode(const char * filePath) const;
			// call it after each shader compilation
			void checkCompilation(pxUInt shaderId) const;
			// call it after linking the program
			void checkLinks(pxUInt programId) const;

			void checkUniformsErrors();

		public:
            CShader() { /***/ }
			~CShader();

			void load(const char * vertexFilePath, const char * fragmentFilePath);
			void reload(const char * vertexFilePath, const char * fragmentFilePath);
			inline pxUInt id() { return m_id; }

			void bindVariableName(const char * correspondingVariableNameInShader);

			template<typename SHADER_FRIENDLY_TYPE>
            void sendVariable(const char * correspondingVariableNameInShader, const SHADER_FRIENDLY_TYPE & variable);
		};
	}
}
