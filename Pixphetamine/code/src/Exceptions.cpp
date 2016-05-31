#include "Exceptions.h"

namespace Exception {
		
	VGeneric_Exception::VGeneric_Exception(std::string a_file, pxInt a_line) {
		//m_message = Utility::concat(" - Exception raised in file: ", a_file, " at line: ", a_line, "\n", "\t> ");
	}

	FileNotFound_Exception::FileNotFound_Exception(std::string a_filePath, std::string a_file, pxInt a_line) : VGeneric_Exception(a_file, a_line) {
		//m_message = Utility::concat(m_message, "File not found: ", a_filePath, " !!\n");
	}
}