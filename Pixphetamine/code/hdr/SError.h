/// \file		SError.h
///	\author		Daniel Huc
/// \date		June 2016
#pragma once

/* Standard library includes */
#include <string>
#include <stack>
#include <iostream>
#include <fstream>

/* External dependencies */

/* Internal headers includes */
#include "HInternalTypesAliases.h"
#include "Utility.h"

// Define __CURRENT_FUNCTION__ macro to call __func__ define for c++11 gcc or __FUNCTION__ defined for MSVSC!
#ifndef __CURRENT_FUNCTION__
#ifdef _MSC_VER
#define __CURRENT_FUNCTION__ __FUNCTION__
#else
#define __CURRENT_FUNCTION__ __func__
#endif
#endif

// Double macro to display the number of the line instead of "__LINE__"
#define S(value) #value
#define PRINTVALUE(value) S(value)

// Stack display message (file / line / function)
#define __CONTEXT__ ("> File " __FILE__ ":"  PRINTVALUE(__LINE__) " in function " __CURRENT_FUNCTION__ "\n")

#define STACK_TRACE (&Utility::SError::getInstance())->stack(__CONTEXT__)
#define UNSTACK_TRACE (&Utility::SError::getInstance())->unstack()
#define ERROR(message) (&Utility::SError::getInstance())->display(#message)

namespace Utility {

	class SError {
	/* Members */
	private:
		pxBool m_isErrorStreamSpecified{ false };	/// If Error's constructor is default, display error goes to print in cerr!
		std::stack<std::string> m_errorCallStack;
		std::ofstream m_outputStream;

	/* Methods */
	private: 
		SError() { /***/ };
		~SError();

	public:
		static SError& getInstance();
		static void destroyInstance();
		
		void setOutputFile(std::string fileName);
		void stack(std::string context) { m_errorCallStack.push(context); }
		void unstack() { m_errorCallStack.pop(); }
		void display(std::string errorMessage);
		void displayAndCrash(std::string errorMessage);
	};
}