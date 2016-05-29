/// \file		Utility.h
///	\author		Daniel Huc
/// \date		May 2016
#pragma once

/* Standard library includes */

/* External dependencies */

/* Internal headers includes */
#include "HInternalTypesAliases.h"

namespace Utility {

	template<typename... Args>
	std::string concat(Args&&... a_args);
}