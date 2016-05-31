#include "CShader.h"

namespace PixPhetamine {
	CShader::CShader() : m_programId(0) {

	}

	CShader::~CShader() {
		if (glIsProgram(m_programId)) {
			glDeleteProgram(m_programId);
		}
	}

	void CShader::load(const char* a_vertexPath, const char* a_fragmentPath) {
		try {
			pxUInt vertexId = glCreateShader(GL_VERTEX_SHADER);
			pxUInt fragmentId = glCreateShader(GL_FRAGMENT_SHADER);

			std::vector<const char*> shaderPaths;
			shaderPaths.push_back(a_vertexPath);
			shaderPaths.push_back(a_fragmentPath);

			for (auto const &shaderPath : shaderPaths) {
				const char * code = (getCode(shaderPath)).c_str();
				std::cerr << shaderPath << " :" << std::endl;
				if (strcmp(shaderPath, a_vertexPath) == 1) {
					glShaderSource(vertexId, 1, &(code), NULL);
					glCompileShader(vertexId);
					checkCompilation(vertexId);
				} else {
					glShaderSource(fragmentId, 1, &(code), NULL);
					glCompileShader(fragmentId);
					checkCompilation(vertexId);
				}
			}
			
			// Create, Attach and Link program
			m_programId = glCreateProgram();
			glAttachShader(m_programId, vertexId);
			glAttachShader(m_programId, fragmentId);
			glLinkProgram(m_programId);
			checkLinks(m_programId);

			glDeleteShader(vertexId);
			glDeleteShader(fragmentId);
		} catch (const Exception::FileNotFound_Exception &e) {
			std::cerr << e.what() << "\n";
		}
	}


	void CShader::reload(const char *a_vertexPath, const char *a_fragmentPath) {

		// check if the program already contains a shader 
		if (glIsProgram(m_programId)) {
			// delete it...
			glDeleteProgram(m_programId);
		}

		// ... and reload it
		load(a_vertexPath, a_fragmentPath);
	}

	void CShader::checkCompilation(pxUInt a_shaderId) const {
		// check if the compilation was successfull (and display syntax errors)
		// call it after each shader compilation
		pxInt result = GL_FALSE;
		pxInt infoLogLength;

		glGetShaderiv(a_shaderId, GL_COMPILE_STATUS, &result);
		glGetShaderiv(a_shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0) {
			std::vector<char> message(infoLogLength + 1);
			glGetShaderInfoLog(a_shaderId, infoLogLength, NULL, &message[0]);
			std::cerr << &message[0] << std::endl;
		}
	}

	void CShader::checkLinks(pxUInt a_programId) const {
		// check if links were successfull (and display errors)
		// call it after linking the program  
		pxInt result = GL_FALSE;
		pxInt infoLogLength;

		glGetProgramiv(a_programId, GL_LINK_STATUS, &result);
		glGetProgramiv(a_programId, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0) {
			std::vector<char> message(infoLogLength + 1);
			glGetProgramInfoLog(a_programId, infoLogLength, NULL, &message[0]);
			std::cerr << &message[0] << std::endl;
		}
	}

	std::string CShader::getCode(const char * a_filePath) const {
		// Return a string containing the source code of the input file
		std::string   shaderCode;
		std::ifstream shaderStream(a_filePath, std::ios::in);

		if (!shaderStream.is_open()) {
			throw Exception::FileNotFound_Exception(a_filePath, __FILE__, __LINE__);
			return "";
		}

		std::string line = "";
		while (getline(shaderStream, line))
			shaderCode += "\n" + line;
		shaderStream.close();

		return shaderCode;
	}
}