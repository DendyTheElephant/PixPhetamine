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
			VGeneric_Exception(std::string a_file, pxInt a_line) {
				m_message = Utility::concat(" - Exception raised in file: ", a_file, " at line: ", a_line, "\n", "\t> ");
			}

			virtual ~VGeneric_Exception() throw() = 0;

			virtual const char * what() const throw() {
				return m_message.c_str();
			}
		};
	} // End of anonymous namespace




	class FileNotFound_Exception : public VGeneric_Exception {
	public:
		FileNotFound_Exception(std::string a_filePath, std::string a_file, pxInt a_line) : VGeneric_Exception(a_file, a_line) {
			m_message = Utility::concat(m_message, "File not found: ", a_filePath, " !!\n");
		}
		virtual ~FileNotFound_Exception() throw() {};
	};

} // End of Exception namespace