#include <iomanip>
#include <iostream>
#include <locale>
#include <signal.h>
#include <string>
#include <sys/stat.h>
#include <getopt.h>
#include <termios.h>
#include <cstdlib>

#include <boost/algorithm/string.hpp>
#include <curl/curl.h>
#include <tgbot/tgbot.h>
#include <fmt/core.h>
#include "../lib/json.hpp"

#include "utils.hpp"
#include "config.hpp"
#include "log.hpp"
#include "network.hpp"
#include "bot.hpp"
#include "escape.hpp"

// Args {{{
namespace Args {

  // Print {{{
  namespace Print {
#define ARGUMENTS  COMMANDLINE_ARGUMENTS

    struct option longopts[] = {
#define X(A,A1,A2,B,C,...)  { STR(B), C, NULL, A },
      ARGUMENTS
#undef  X
    };

    const char opts[] = {
#define X(A,A1,A2,...)  A, A1, A2,
      ARGUMENTS
#undef  X
    };

    void help(void) {
      std::cout << PROGRAM_DESCRIPTION << std::endl;
#define X(A,A1,A2,B,C,E)  std::cout << "\t" << "-" << A \
      << " | " << "--" << STR(B) << "\t - " << E << std::endl;
      ARGUMENTS
#undef  X

      std::cout <<
#define X(A)  "\n\t" A <<
        PROGRAM_POST_HELP
#undef  X
        std::endl;
    }

    void methods(void) {
      std::cout << "Publish Methods:" << std::endl;
      for (int x=0; x<METHOD_LENGTH; x++) {
        std::cout << "\t" << x+1 << "| " << Bot::Publish::method_config[x].name
                  << std::endl;
      }
    }

    void version(void) {
      std::cout << PROGRAM_NAME " " VERSION " (" __DATE__ ", " __TIME__ ") [" CC
                                " " __VERSION__ "]"
                << std::endl;
    }

#undef ARGUMENTS
  }
  // }}}

  Conf::Methods method = Conf::NONE;
  bool enabled[METHOD_LENGTH]    = {false};
  bool keys_set[METHOD_LENGTH]  = {false};

  // Check {{{
  bool check_key(const std::string& key, Conf::Methods m) {
    return Bot::Publish::method_config[m].check_key(key);
  }
  bool is_key(const std::string& pkey, Conf::Methods m) {
    std::string key = "";
    if (pkey == "-") {
      std::cout << "Insert api key for " << Bot::Publish::method_config[m].name
                << ": ";

      Utils::disableEcho();
      std::cin >> key;
      Escape::Term::restoreTermios();

      std::cout << std::endl;
      if (key.empty()) {
        return false;
      }
    } else {
      std::ifstream fin(pkey);
      if (fin) {
        std::getline(fin, key);
        fin.close();
      } else {
        key = pkey;
      }
    }

    return check_key(key, m);
  }

  bool enable_method(std::string m) {
    try {
      int num = std::stoi(m);
      if (num > 0 && num <= METHOD_LENGTH) {
        method = static_cast<Conf::Methods>(num-1);
        return true;
      }
    } catch (std::invalid_argument& e) {
    }

    for (int x=0; x<METHOD_LENGTH; x++) {
      if (boost::iequals(m, Bot::Publish::method_config[x].name)) {
        method = static_cast<Conf::Methods>(x);
        return true;
      }
    }

    return false;
  }


  bool check_environment(int m) {
    const char *env_value =
        std::getenv(Conf::method_secret_environment[m].c_str());
    if (env_value &&
        (check_key(env_value, static_cast<Conf::Methods>(m)) == true)) {

      Log::print(Log::SETTINGS, "For ", Bot::Publish::method_config[m].name,
                 " Got key environment variable: ",
                 Conf::method_secret_environment[m]);
      return true;
    }
    return false;
  }

  bool check_file(int m) {
    std::ifstream check_secret(Conf::method_secret_local[m]);
    std::string key;
    if (check_secret) {
      std::getline(check_secret, key);
      check_secret.close();
      if (check_key(key, static_cast<Conf::Methods>(m)) == true) {
        Log::print(Log::SETTINGS, "For ", Bot::Publish::method_config[m].name,
                   " Got key from local file: ", Conf::method_secret_local[m]);
        return true;
      }
    }
    return false;
  }
  // }}}

  void enable() {
    if (method != Conf::NONE) {
      enabled[method] = true;
      Log::print(Log::SETTINGS, "Enabled (",
          Bot::Publish::method_config[method].name, ": ",
          static_cast<int>(method) + 1, ") Method");
      return;
    }

    Log::print_error(Log::SETTINGS, "No method selected");
    exit(1);
  }

  void parse(int argc, char* argv[]) {
    int opt = 0;
    while ((opt = getopt_long(argc, argv, Print::opts, Print::longopts, NULL)) != -1) {
      switch (opt) {
        case '?':
          {
            Log::print_error(Log::SETTINGS, "Invalid option");
            exit(1);
          }

        case Conf::version:
          {
            Print::version();
            exit(0);
          }
        case Conf::help:
          {
            Print::help();
            exit(0);
          }

        case Conf::method:
          {
            std::string method;
            if (optarg == nullptr || (method = optarg).empty()) {
              Print::methods();
              exit(0);
            }

            if (enable_method(method) == true) {
              enable();
              break;
            }

            Log::print_error(Log::SETTINGS, "Invalid method selected");
            exit(1);
          }
        case Conf::enable:
          {
            enable();
            break;
          }
        case Conf::key:
          {
            if (optarg == nullptr) {
              Log::print_error(Log::SETTINGS, "Optarg was set to NULL. No key provided");
              exit(1);
            }
            std::string key = optarg;
            if (key.empty()) {
              Log::print_error(Log::SETTINGS, "No key provided");
              exit(1);
            }

            if (is_key(key, method) == true) {
              keys_set[method] = true;
              if (enabled[method] != true) {
                enable();
              }
              break;
            }

            Log::print_error(Log::SETTINGS, "Invalid key provided");
            exit(1);
          }
      }
    }

    // Check if all enabled methods, also have a key; as well as at least one
    // method is enabled.
    unsigned int activated = 0;
    for (int x=0; x<METHOD_LENGTH; x++) {
      if (enabled[x] != true)
        continue;

      activated++;
      if (keys_set[x] == true)
        continue;

      if (check_environment(x) == true)
        continue;
      if (check_file(x) == true)
        continue;

      Log::print_error(Log::SETTINGS, "Missing an API key for ",
                       Bot::Publish::method_config[method].name, ".\n");
      exit(1);
    }

    if (activated <= 0) {
      Log::print_error(Log::SETTINGS, "No methods were enabled.");
      exit(1);
    }
    Log::print(Log::SETTINGS, "Total methods enabled: ", activated);
  }
}
// }}}

int main(int argc, char* argv[]) {
  Escape::set();
	Args::parse(argc, argv);


  Bot::set(Args::enabled);
  Bot::load(); ///< NOTE: data will only be loaded for methods whose state is set to true
  Bot::run();

  return 0;
}
// vim: tabstop=2 shiftwidth=2 softtabstop=2 expandtab
