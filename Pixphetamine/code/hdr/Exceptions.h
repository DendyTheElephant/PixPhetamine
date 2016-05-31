/// \file		Exceptions.h
///	\author		Daniel Huc
/// \date		May 2016
/// \brief		Custom exceptions definition
/// \details	All custom exceptions must be defined in this header file, as child of VGeneric_Exception
#pragma once

/* Standard library includes */
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <exception>

/* Internal headers includes */
#include "HInternalTypesAliases.h"
#include "Utility.h"

/// Provides custom exceptions for the program
namespace Exception {

	/* Anonymous namespace */
	namespace {
		/// \brief		Generic exception class (Abstract)
		/// \details	Defines the structure of program exceptions (with a fancy message)
		class VGeneric_Exception : public std::exception {
			/* Members */
		protected:
			std::string m_message;

			/* Methods */
		public:
			VGeneric_Exception(std::string file, pxInt line);
			virtual ~VGeneric_Exception() throw() { /***/ }
			virtual const char * what() const throw() {
				return m_message.c_str();
			}
		};
	} // End of Anonymous namespace
	

	class FileNotFound_Exception : public VGeneric_Exception {
	public:
		FileNotFound_Exception(std::string filePath, std::string file, pxInt line);
	};

} // End of Exception namespace