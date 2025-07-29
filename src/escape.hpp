#ifndef ESCAPE__HPP
#define ESCAPE__HPP
namespace Escape {

  // Terminal {{{
  namespace Term {
    struct termios orig_termios;

    void restoreTermios() {
      errno = 0;
      if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
        Log::print_error(Log::SYSTEM, "tcsetattr: ", strerror(errno));
        exit(1);
      }
    }

    void saveTermios() {
      errno = 0;
      if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        Log::print_error(Log::SYSTEM, "tcgetattr: ", strerror(errno));
        exit(1);
      }
    }
  }
  // }}}

  namespace Save {
    void save_all() {
      Bot::Publish::save_data();
    }
  }

	void trigger() {
    Save::save_all();
    Term::restoreTermios();
	}

  // Signals {{{
  namespace Signals {
    static int used_signals = 0;

    void ifUsed(int sig, std::string name) {
      if (used_signals == sig) {
        Log::print(Log::SYSTEM, "Caught Multiple ", name, ".\n\tExiting Immediately.");
        exit(sig);
      }
      used_signals = sig;
    }

    void user(int sig) {
      std::cout << std::endl;
      switch (sig) {
        case SIGINT:  {
                        ifUsed(sig, "SIGINT");
                        Log::print(Log::SYSTEM, "Caught SIGINT (Ctrl+C).\n\tSaving then Exiting Gracefully.");
                        break;
                      }
        case SIGTERM: {
                        ifUsed(sig, "SIGTERM");
                        Log::print(Log::SYSTEM, "Received SIGTERM.\n\tSaving then Exiting Gracefully.");
                        break;
                      }
      }
      trigger();
      exit(sig);
    }

    void system(int sig) {
      std::cout << std::endl;
      switch (sig) {
        case SIGSEGV: {
                        ifUsed(sig, "SIGSEGV");
                        Log::print(Log::SYSTEM, "Received SIGSEGV.\n\tSaving then Exiting Gracefully.");
                        break;
                      }
      }
      trigger();
      exit(sig);
    }

    void signal_handler() {
      struct sigaction usa;
      usa.sa_handler   = &user;
      usa.sa_flags     = SA_SIGINFO | SA_RESTART;

      struct sigaction ssa;
      ssa.sa_handler   = &system;
      ssa.sa_flags     = SA_SIGINFO | SA_RESTART;

      
      sigaction(SIGINT,   &usa, nullptr);
      sigaction(SIGTERM,  &usa, nullptr);
      sigaction(SIGSEGV,  &ssa, nullptr);
    }
	}
  // }}}

	void set() {
    Term::saveTermios();
    std::atexit(trigger);
    Signals::signal_handler();
  }
}
#endif
// vim: tabstop=2 shiftwidth=2 softtabstop=2 expandtab
