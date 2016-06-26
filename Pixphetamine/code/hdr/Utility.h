/// \file		Utility.h
///	\author		Daniel Huc
/// \date		May 2016
#pragma once

/* Standard library includes */
#include <string>

/* External dependencies */

/* Internal headers includes */

namespace Utility {

	namespace { /* Anonymous namespace */
		// Used with concat
		void addToStream(std::ostringstream&) { /***/ }

		// Used with concat
		template<typename T, typename... Args>
		void addToStream(std::ostringstream& a_stream, T&& a_value, Args&&... a_args) {
			a_stream << std::forward<T>(a_value);
			addToStream(a_stream, std::forward<Args>(a_args)...);
		}
	} // End of Anonymous namespace


	template<typename... Args>
	std::string concat(Args&&... a_args) {
		std::ostringstream s;
		addToStream(s, std::forward<Args>(a_args)...);
		return s.str();
	}


	/// \brief		Deleter to free an entire structure from RAM using <algorithm> (like for_each)
	/// \details    Template, definition of () operator witch deletes the Object. Use it in algorithm functions (STL). \n
	///				Example: for_each(m_gameObject.begin(), m_gameObject.end(), deleter<GameObject>());
	/// \param		<T>		Object type witch be deleted.
	template<typename T>
	struct deleter {
		void operator()(T* objectToDelete) {
			if (objectToDelete != nullptr) { // But acctualy in c++ we can free a nullpointer (Also handled?)
				delete objectToDelete;
				objectToDelete = nullptr;
			}
		}
	};
}