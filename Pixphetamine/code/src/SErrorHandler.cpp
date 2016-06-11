#include "SErrorHandler.h"

namespace Utility {

	SErrorHandler::~SErrorHandler() {
		if (m_outputStream.is_open()) {
			m_outputStream.close();
		}
	}

	void SErrorHandler::setOutputFile(std::string a_fileName) {
		m_outputStream.open(a_fileName);
		m_isErrorStreamSpecified = true;
	}

	SErrorHandler& SErrorHandler::getInstance() {
		static SErrorHandler* errorHandler = nullptr;
		if (errorHandler == nullptr) {
			errorHandler = new SErrorHandler();
		}

		return *errorHandler;
	}

	void SErrorHandler::destroyInstance() {
		static SErrorHandler* errorHandler = &getInstance();
		if (errorHandler != nullptr) {
			delete errorHandler;
		}
	}

	void SErrorHandler::display(std::string a_errorMessage) {
		std::streambuf* originErrorStream = nullptr;
		std::stack<std::string> backupStack;
		std::string callStack;

		while (!m_errorCallStack.empty()) {
			callStack += "\t" + m_errorCallStack.top();
			backupStack.push(m_errorCallStack.top());
			m_errorCallStack.pop();
		}
		
		if (m_isErrorStreamSpecified) {
			originErrorStream = std::cerr.rdbuf();
			std::cerr.rdbuf(m_outputStream.rdbuf());
		}
		
		std::cerr << a_errorMessage << std::endl;
		std::cerr << callStack;

		if (m_isErrorStreamSpecified) {
			std::cerr.rdbuf(originErrorStream);
		}
		
		while (!backupStack.empty()) {
			m_errorCallStack.push(backupStack.top());
			backupStack.pop();
		}
	}

	void SErrorHandler::displayAndCrash(std::string a_errorMessage) {
		display(a_errorMessage);
		if (m_outputStream.is_open()) {
			m_outputStream.close();
		}
		exit(EXIT_FAILURE);
	}
}