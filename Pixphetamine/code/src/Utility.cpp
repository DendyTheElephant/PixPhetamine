#include "Utility.h"

/* Anonymous namespace */
namespace {
	// Used with concat
	void addToStream(std::ostringstream&) { /***/ }

	// Used with concat
	template<typename T, typename... Args>
	void addToStream(std::ostringstream& a_stream, T&& a_value, Args&&... a_args) {
		a_stream << std::forward<T>(a_value);
		addToStream(a_stream, std::forward<Args>(a_args)...);
	}
} // End of Anonymous namespace

namespace Utility {

	template<typename... Args>
	std::string concat(Args&&... a_args) {
		std::ostringstream s;
		addToStream(s, std::forward<Args>(a_args)...);
		return s.str();
	}
}