#ifndef UTILS__HPP
#define UTILS__HPP

#define _STR(x)	#x
#define STR(x)	_STR(x)




namespace Utils {
  template<class T>
  std::string formatWithCommas(T value) {
    std::stringstream ss;
    ss.imbue(std::locale("en_US.UTF-8"));
    ss << std::fixed << value;
    return ss.str();
  }

  bool file_exists(const char* file) {
    struct stat buf;
    if (stat(file, &buf) == -1)
      return false;
    return true;
  }

  void disableEcho() {
    struct termios t;

    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ECHO; // Enable ECHO flag
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
  }

  // TODO: update
  // NOTE: specific for xrp.hpp
  void string2tm(const char* time, struct tm& tm) {
    const char* format = "%Y-%m-%dT%H:%M:%S";

    if (strptime(time, format, &tm) == nullptr) {
      std::cerr << "Failed to parse time string." << std::endl;
      throw "Failed to parse time";
    }

    tm.tm_isdst = 0;
  }


  std::string tm2String(const std::tm& timeStruct, const std::string& format) {
    char buffer[100]; // Buffer to hold the formatted string
    memset(buffer, 0, 100);
    strftime(buffer, sizeof(buffer), format.c_str(), &timeStruct);
    return std::string(buffer);
  }
}

#endif
// vim: tabstop=2 shiftwidth=2 softtabstop=2 expandtab
