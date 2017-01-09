#include "CShader.h"

namespace PixPhetamine {

	namespace LowLevelWrapper {

		CShader::CShader(const char* a_vertexPath, const char* a_fragmentPath) {
			m_isRealodable = true;
			m_vertexFilePath = std::string(a_vertexPath);
			m_fragmentFilePath = std::string(a_fragmentPath);
			load(getCode(a_vertexPath), getCode(a_fragmentPath));
		}

		CShader::CShader(std::string a_vertexShaderName, std::string a_vertexCode, std::string a_fragmentShaderName, std::string a_fragmentCode) {
			m_isRealodable = false;
			m_vertexFilePath = a_vertexShaderName;
			m_fragmentFilePath = a_fragmentShaderName;
			load(a_vertexCode, a_fragmentCode);
		}


		CShader::~CShader() {
			if (glIsProgram(m_id)) {
				glDeleteProgram(m_id);
			}
		}

		void CShader::checkContext() const {
			// Verify that the current binded shader is this one! (To optimise performance, prefer not to switch between shaders)
			// Active the shader before sending textures...
			GLint currentShader;
			glGetIntegerv(GL_CURRENT_PROGRAM, &currentShader);
			if (currentShader != m_id) {
				glUseProgram(m_id);
			}
		}

		void CShader::load(std::string a_vertexCode, std::string a_fragmentCode) {
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
			const char * vertexCode = a_vertexCode.c_str();
			pxInt vertexCodeLength = a_vertexCode.length();
			glShaderSource(vertexId, 1, &vertexCode, &vertexCodeLength);
			std::cerr << "Compilation of Vertex shader " << m_vertexFilePath.c_str() << ":";
			glCompileShader(vertexId);
			checkCompilation(vertexId);

			// Fragment compilation
			const char * fragmentCode = a_fragmentCode.c_str();
			pxInt fragmentCodeLength = a_fragmentCode.length();
			glShaderSource(fragmentId, 1, &fragmentCode, &fragmentCodeLength);
			std::cerr << "Compilation of Fragment shader " << m_fragmentFilePath.c_str() << ":";
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

			STACK_MESSAGE("Checking OpenGL errors");
			Utility::UErrorHandler::checkOpenGLErrors();

			UNSTACK_TRACE;
		}


		void CShader::reload() {
			STACK_TRACE;
			// check if the program already contains a shader
			if (glIsProgram(m_id)) {
				// delete it...
				glDeleteProgram(m_id);
			}

			// ... and reload it
			if (m_isRealodable) {
				load(getCode(m_vertexFilePath.c_str()), getCode(m_fragmentFilePath.c_str()));
			}
			
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
			checkContext();
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
			checkContext();
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
			checkContext();
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
			checkContext();
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
			checkContext();
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
			checkContext();
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
