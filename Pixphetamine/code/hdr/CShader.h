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

/* Internal headers includes */
#include "HInternalTypesAliases.h"
#include "UErrorHandler.h"

namespace PixPhetamine {
	/// \brief		Shader loader
	/// \details	Provides a shader code loader, and error displayer (once loaded, you can just reload the shader)
	class CShader {

	public:
		CShader() { /***/ };
		~CShader();

		void load(const char * vertexFilePath, const char * fragmentFilePath);
		void reload(const char * vertexFilePath, const char * fragmentFilePath);
		inline pxUInt id() { return m_programId; }

	private:
		pxUInt m_programId{ 0 };

		// string containing the source code of the input file
		std::string CShader::getCode(const char * filePath) const;

		// call it after each shader compilation
		void checkCompilation(pxUInt shaderId) const;

		// call it after linking the program
		void checkLinks(pxUInt programId) const;
	};

}