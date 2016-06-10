#include "CShader.h"

namespace PixPhetamine {
	CShader::CShader() {

	}

	CShader::~CShader() {
		if (glIsProgram(m_programId)) {
			glDeleteProgram(m_programId);
		}
	}

	void CShader::load(const char* a_vertexPath, const char* a_fragmentPath) {
		pxUInt vertexId = glCreateShader(GL_VERTEX_SHADER);
		if (vertexId == 0) {
			std::cerr << "Creation of Vertex shader failed!" << std::endl;
		}
		pxUInt fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
		if (vertexId == 0) {
			std::cerr << "Creation of Fragment shader failed!" << std::endl;
		}

		// Vertex compilation
		std::string vertexCodeString = getCode(a_vertexPath);
		const char * vertexCode = vertexCodeString.c_str();
		pxInt vertexCodeLength = vertexCodeString.length();
		glShaderSource(vertexId, 1, &vertexCode, &vertexCodeLength);
		std::cerr << "Compilation of Vertex shader " << a_vertexPath << ":";
		glCompileShader(vertexId);
		checkCompilation(vertexId);
			
		// Fragment compilation
		std::string fragmentCodeString = getCode(a_fragmentPath);
		const char * fragmentCode = fragmentCodeString.c_str();
		pxInt fragmentCodeLength = fragmentCodeString.length();
		glShaderSource(fragmentId, 1, &fragmentCode, &fragmentCodeLength);
		std::cerr << "Compilation of Fragment shader " << a_fragmentPath << ":";
		glCompileShader(fragmentId);
		checkCompilation(fragmentId);

			
		// Create, Attach and Link program
		m_programId = glCreateProgram();
		glAttachShader(m_programId, fragmentId);
		glAttachShader(m_programId, vertexId);
		glLinkProgram(m_programId);
		checkLinks(m_programId);

		glDeleteShader(vertexId);
		glDeleteShader(fragmentId);
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
			std::cerr << std::endl;
			std::vector<char> message(infoLogLength + 1);
			glGetShaderInfoLog(a_shaderId, infoLogLength, NULL, &message[0]);
			std::cerr << &message[0] << std::endl;
			glDeleteShader(a_shaderId); // Don't leak the shader.
		} else {
			std::cerr << " OK!" << std::endl;
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
			std::cerr << "File " << a_filePath << " not found! In file " << __FILE__ " at line " << __LINE__ << std::endl;
			return "";
		}

		std::string line = "";
		while (getline(shaderStream, line))
			shaderCode += "\n" + line;
		shaderStream.close();

		return shaderCode;
	}
}