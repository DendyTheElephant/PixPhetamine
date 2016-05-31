/// \file		Utility.h
///	\author		Daniel Huc
/// \date		May 2016
#pragma once

/* Standard library includes */
#include <string>

/* External dependencies */

/* Internal headers includes */

namespace Utility {

	template<typename... Args>
	std::string concat(Args&&... a_args);
}