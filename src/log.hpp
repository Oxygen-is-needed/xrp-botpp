#ifndef LOG_HPP
#define LOG_HPP

#include <sstream>

namespace Log {
#define X(a,b,...) Y(a,b, "32")
  // TODO: move to config
#define LOG_METHODS	\
	Y(SYSTEM,   "System",   "35")	\
	Y(XRP,      "XRP",      "36")	\
	Y(SETTINGS, "Settings", "33")	\
	Y(NETWORK,  "Network",  "34")	\
  METHODS

	enum Log_Methods {
#define Y(a,b,...)	a,
		LOG_METHODS
#undef Y
	};


  // TODO: change to string_view?
	const std::string log_methods[] = {
#define Y(a,b,c)	"[\033[" c "m" b "\033[0m]: ",
		LOG_METHODS
#undef Y
	};
#undef X


  namespace {
    template<typename T>
    void print(T first) {
      std::cout << first;
    }
  }

	template< typename T, typename ... Args >
	void print(T first, Args const& ... args) {
		print(first);
		print(args...);
	}


	template< typename T>
	void print(enum Log_Methods lm, T first) {
		std::cout << log_methods[lm] << first << std::endl;
	}

	template< typename T, typename ... Args >
	void print(enum Log_Methods lm, T first,
			Args const& ... args) {
		std::cout << log_methods[lm] << first;
		print(args...);
    std::cout << std::endl;
	}


#define LOG_ERROR "\033[31mERROR\033[0m: "
	template< typename T, typename ... Args >
	void print_error(enum Log_Methods lm, T first) {
		std::cout << log_methods[lm] << LOG_ERROR << first << std::endl;
	}
	template< typename T, typename ... Args >
	void print_error(enum Log_Methods lm, T first,
			Args const& ... args) {
		std::cout << log_methods[lm] << LOG_ERROR << first;
		print(args...);
    std::cout << std::endl;
	}
#undef LOG_ERROR

}

#endif
// vim: tabstop=2 shiftwidth=2 softtabstop=2 expandtab
