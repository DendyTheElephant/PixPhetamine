#include "CShader.h"

namespace PixPhetamine {

	namespace LowLevelWrapper {

		CShader::CShader(const char* a_vertexPath, const char* a_fragmentPath) {
			load(a_vertexPath, a_fragmentPath);
		}


		CShader::~CShader() {
			if (glIsProgram(m_id)) {
				glDeleteProgram(m_id);
			}
		}


		void CShader::load(const char* a_vertexPath, const char* a_fragmentPath) {
			STACK_TRACE;
			pxUInt vertexId = glCreateShader(GL_VERTEX_SHADER);
			if (vertexId == 0) {
				STACK_TRACE;
				ERROR("Creation of Vertex shader failed!");
			}
			pxUInt fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
			if (vertexId == 0) {
				STACK_TRACE;
				ERROR("Creation of Fragment shader failed!");
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
			m_id = glCreateProgram();
			glAttachShader(m_id, fragmentId);
			glAttachShader(m_id, vertexId);
			glLinkProgram(m_id);
			checkLinks(m_id);

			glDeleteShader(vertexId);
			glDeleteShader(fragmentId);
			UNSTACK_TRACE;
		}


		void CShader::reload(const char *a_vertexPath, const char *a_fragmentPath) {
			STACK_TRACE;
			// check if the program already contains a shader
			if (glIsProgram(m_id)) {
				// delete it...
				glDeleteProgram(m_id);
			}

			// ... and reload it
			load(a_vertexPath, a_fragmentPath);
			UNSTACK_TRACE;
		}


		void CShader::checkCompilation(pxUInt a_shaderId) const {
			// check if the compilation was successfull (and display syntax errors)
			// call it after each shader compilation
			pxInt result = GL_FALSE;
			pxInt infoLogLength;

			glGetShaderiv(a_shaderId, GL_COMPILE_STATUS, &result);
			glGetShaderiv(a_shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

			if (infoLogLength > 1) {
				std::cerr << std::endl;
				std::vector<char> message(infoLogLength + 1);
				glGetShaderInfoLog(a_shaderId, infoLogLength, NULL, &message[0]);
				std::cerr << &message[0] << std::endl;
				glDeleteShader(a_shaderId); // Don't leak the shader.
			}
			else {
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
			STACK_TRACE;
			// Return a string containing the source code of the input file
			std::string   shaderCode;
			std::ifstream shaderStream(a_filePath, std::ios::in);

			if (!shaderStream.is_open()) {
				STACK_TRACE;
				ERROR("File " + std::string(a_filePath) + " not found!");
			}

			std::string line = "";
			while (getline(shaderStream, line))
				shaderCode += "\n" + line;
			shaderStream.close();

			UNSTACK_TRACE;
			return shaderCode;
		}


		void CShader::bindVariableName(const char * a_correspondingVariableNameInShader) {
			STACK_TRACE;
			m_variableNames[a_correspondingVariableNameInShader] = glGetUniformLocation(m_id, a_correspondingVariableNameInShader);
			STACK_MESSAGE("Binding " + std::string(a_correspondingVariableNameInShader));
			Utility::UErrorHandler::checkOpenGLErrors();
			UNSTACK_TRACE;
		}


		// Here goes the specifications of the sendVariable():

		// Template specification <pxFloat> to send
		template <>
		void CShader::sendVariable(const char * a_correspondingVariableNameInShader, pxFloat const& a_variable) {
			STACK_TRACE;
			if (m_variableNames.count(a_correspondingVariableNameInShader) != 0) {
				glUniform1f(m_variableNames[a_correspondingVariableNameInShader], a_variable);
				STACK_MESSAGE("Sending " + std::string(a_correspondingVariableNameInShader));
				Utility::UErrorHandler::checkOpenGLErrors();
			}
			else {
				ERROR("Error : " + std::string(a_correspondingVariableNameInShader) + " haven't been binded to the shader!");
			}
			 UNSTACK_TRACE;
		}

		// Template specification <pxInt> to send
		template <>
		void CShader::sendVariable(const char * a_correspondingVariableNameInShader, pxInt const& a_variable) {
			STACK_TRACE;
			if (m_variableNames.count(a_correspondingVariableNameInShader) != 0) {
				glUniform1i(m_variableNames[a_correspondingVariableNameInShader], a_variable);
				STACK_MESSAGE("Sending " + std::string(a_correspondingVariableNameInShader));
				Utility::UErrorHandler::checkOpenGLErrors();
			}
			else {
				ERROR("Error : " + std::string(a_correspondingVariableNameInShader) + " haven't been binded to the shader!");
			}
			UNSTACK_TRACE;
		}

		// Template specification <pxVec3f> to send
		template <>
		void CShader::sendVariable(const char * a_correspondingVariableNameInShader, pxVec3f const& a_variable) {
			STACK_TRACE;
			if (m_variableNames.count(a_correspondingVariableNameInShader) != 0) {
				glUniform3f(m_variableNames[a_correspondingVariableNameInShader], a_variable[0], a_variable[1], a_variable[2]);
				STACK_MESSAGE("Sending " + std::string(a_correspondingVariableNameInShader));
				Utility::UErrorHandler::checkOpenGLErrors();
			}
			else {
				ERROR("Error : " + std::string(a_correspondingVariableNameInShader) + " haven't been binded to the shader!");
			}
			UNSTACK_TRACE;
		}

		// Template specification <pxVec4f> to send
		template <>
		void CShader::sendVariable(const char * a_correspondingVariableNameInShader, pxVec4f const& a_variable) {
			STACK_TRACE;
			if (m_variableNames.count(a_correspondingVariableNameInShader) != 0) {
				glUniform4f(m_variableNames[a_correspondingVariableNameInShader], a_variable[0], a_variable[1], a_variable[2], a_variable[3]);
				STACK_MESSAGE("Sending " + std::string(a_correspondingVariableNameInShader));
				Utility::UErrorHandler::checkOpenGLErrors();
			}
			else {
				ERROR("Error : " + std::string(a_correspondingVariableNameInShader) + " haven't been binded to the shader!");
			}
			UNSTACK_TRACE;
		}

		// Template specification <pxMat4f> to send
		template <>
		void CShader::sendVariable(const char * a_correspondingVariableNameInShader, pxMat4f const& a_variable) {
			STACK_TRACE;
			if (m_variableNames.count(a_correspondingVariableNameInShader) != 0) {
				glUniformMatrix4fv(m_variableNames[a_correspondingVariableNameInShader], 1, GL_FALSE, glm::value_ptr(a_variable));
				STACK_MESSAGE("Sending " + std::string(a_correspondingVariableNameInShader));
				Utility::UErrorHandler::checkOpenGLErrors();
			}
			else {
				ERROR("Error : " + std::string(a_correspondingVariableNameInShader) + " haven't been binded to the shader!");
			}
			UNSTACK_TRACE;
		}
	}
}
