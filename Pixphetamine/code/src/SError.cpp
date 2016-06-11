#include "SError.h"

namespace Utility {

	SError::~SError() {
		if (m_outputStream.is_open()) {
			m_outputStream.close();
		}
	}

	void SError::setOutputFile(std::string a_fileName) {
		m_outputStream.open(a_fileName);
		m_isErrorStreamSpecified = true;
	}

	SError& SError::getInstance() {
		static SError* errorHandler = nullptr;
		if (errorHandler == nullptr) {
			errorHandler = new SError();
		}

		return *errorHandler;
	}

	void SError::destroyInstance() {
		static SError* errorHandler = &getInstance();
		if (errorHandler != nullptr) {
			delete errorHandler;
		}
	}

	void SError::display(std::string a_errorMessage) {
		std::stack<std::string> backupStack;
		std::string callStack;

		while (!m_errorCallStack.empty()) {
			callStack += "\t" + m_errorCallStack.top();
			backupStack.push(m_errorCallStack.top());
			m_errorCallStack.pop();
		}
		
		if (m_isErrorStreamSpecified) {
			std::cerr.rdbuf(m_outputStream.rdbuf());
		}
		
		std::cerr << a_errorMessage << std::endl;
		std::cerr << callStack;

		while (!backupStack.empty()) {
			m_errorCallStack.push(backupStack.top());
			backupStack.pop();
		}
	}

	void SError::displayAndCrash(std::string a_errorMessage) {
		display(a_errorMessage);
		if (m_outputStream.is_open()) {
			m_outputStream.close();
		}
		exit(EXIT_FAILURE);
	}
}