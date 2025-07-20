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
}

#endif
// vim: tabstop=2 shiftwidth=2 softtabstop=2 expandtab
