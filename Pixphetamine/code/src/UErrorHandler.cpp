#include "UErrorHandler.h"

namespace Utility {

	UErrorHandler::~UErrorHandler() {
		if (m_outputStream.is_open()) {
			m_outputStream.close();
		}
	}

	/// For error redirection (into a streamfile)
	void UErrorHandler::setOutputFile(std::string a_fileName) {
		m_outputStream.open(a_fileName);
		m_isErrorStreamSpecified = true;
	}

	/// Singleton pattern stuff (unique creation only)
	UErrorHandler& UErrorHandler::getInstance() {
		static UErrorHandler* errorHandler = nullptr;
		if (errorHandler == nullptr) {
			errorHandler = new UErrorHandler();
		}

		return *errorHandler;
	}

	/// Singleton pattern stuff (unique delete only)
	void UErrorHandler::destroyInstance() {
		static UErrorHandler* errorHandler = &getInstance();
		if (errorHandler != nullptr) {
			delete errorHandler;
		}
	}

	void UErrorHandler::stack(std::string a_context) {
		m_errorCallStack.push("T");
		m_errorCallStack.push(a_context);
	}

	void UErrorHandler::stackMessage(std::string a_message) {
		m_errorCallStack.push(a_message+"\n");
	}

	void UErrorHandler::unstack() {
		// Drop message stack and call
		while (m_errorCallStack.top() != "T") {
			if (!m_errorCallStack.empty()) m_errorCallStack.pop();
		}
		// Drop switch context marker
		if (!m_errorCallStack.empty()) m_errorCallStack.pop();
	}

	void UErrorHandler::display(std::string a_errorMessage) {
		std::streambuf* originErrorStream = nullptr; // Saving cerr stream
		std::stack<std::string> backupStack; // Backup for stack (if non blocking error, we want to keep the context!)
		std::string currentMessage;
		std::string callStack;

		// Drop context/messages for display (update backup stack)
		while (!m_errorCallStack.empty()) {
			currentMessage = m_errorCallStack.top();
			if (currentMessage != "T") {
				callStack += "\t" + m_errorCallStack.top();
			}
			backupStack.push(m_errorCallStack.top());
			m_errorCallStack.pop();
		}
		
		// If stream is redirected to file, redirect cerr to this streamfile
		if (m_isErrorStreamSpecified) {
			originErrorStream = std::cerr.rdbuf();
			std::cerr.rdbuf(m_outputStream.rdbuf());
		}
		
		// Display error message and call stack
		std::cerr << a_errorMessage << std::endl;
		std::cerr << callStack;

		// Rebind cerr if binded to streamfile
		if (m_isErrorStreamSpecified) {
			std::cerr.rdbuf(originErrorStream);
		}
		
		// Restore call stack from backup
		while (!backupStack.empty()) {
			m_errorCallStack.push(backupStack.top());
			backupStack.pop();
		}
	}

	void UErrorHandler::displayAndCrash(std::string a_errorMessage) {
		display(a_errorMessage);
		if (m_outputStream.is_open()) {
			m_outputStream.close();
		}
		exit(EXIT_FAILURE);
	}
}