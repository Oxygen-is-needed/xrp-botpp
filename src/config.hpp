#ifndef CONFIG__HPP
#define CONFIG__HPP

/**
 * define SECRET_FILE_PREFIX  - api key file name prefix
 * define SECRET_ENV_PREFIX   - api key environment name prefix
 *
 * context: the file is usally in lowercase
 * context: the environment is usally in uppercase
 *
 * The prefix used in looking for the api key. It is then appended with a '-'
 * and the name of the publish method in lowercase, for FILE, and uppercase for
 * ENV.
 */
#define SECRET_FILE_PREFIX "secret"
#define SECRET_ENV_PREFIX "SECRET"

/**
 * define SAVE_FILE_PREFIX - prefix used in save file names
 *
 * contex: only used in this file, see namespace Conf for in program use.
 *
 * The prefix used when saving data for publishing methods, for xrp update
 * scanner, or other data.
 */
#define SAVE_FILE_PREFIX "save"

/**
 * define PROGRAM_DESCRIPTION - the program description.
 *
 * Description of program. Used in the help text.
 */
#define PROGRAM_DESCRIPTION                                                    \
  "A bot designed to aggregate and then disseminate inforamtion about an\n"    \
  "XRP token across multiple API endpoints.\n"

/**
 * define PROGRAM_POST_HELP - post help flags text.
 *
 * context: for each item, encapsulate them in the X() macro
 *
 * Additional help and context for using this program.
 */
#define PROGRAM_POST_HELP                                                      \
  X("When no API key is given, the program will check two alternative "        \
    "\nlocations before raising an error. The first location is an "           \
      "environment "                                                           \
    "\nviarble, which follows a specific naming convention: the prefix "       \
    "\n\"" SECRET_ENV_PREFIX                                                   \
      "-\" followed by the API name in uppercase letters. If the "             \
    "\nenvironment variable is not found, the system will then look for a "    \
      "file "                                                                  \
    "\nwith a name constructed using the prefix \"" SECRET_FILE_PREFIX         \
      "-\" followed by the API "                                               \
    "\nname lowercase letters.")

/**
 * define COMMANDLINE_ARGUMENTS - command flags and arguments configuration.
 *
 * context:
 *  There are three argument types:
 *    - no_argument
 *    - optional_argument
 *    - required_argument
 *
 * context: the two arguments after the single flag are used to determine if the
 *  flag has no arguments, or required arguments (':', ' '), or optional
 *  arguments (':', ':').
 *
 * context: if the long flag is changed you will have to change the getopt
 *  implementation names along with it.
 *
 *
 * Used to configure the arguments used when executing the program.
 * There are 6 arguments to the macro X:
 *    1) single flag name
 *    2) single flag argument type
 *    3) single flag argument type
 *    4) long flag name.
 *      - should be without quotes
 *    5) long flag argument type
 *    6) argument description
 */
#define COMMANDLINE_ARGUMENTS                                                  \
  X('v', ' ', ' ', version, no_argument, "Print version.")                     \
  X('h', ' ', ' ', help, no_argument, "Print help text.")                      \
  X('m', ':', ':', method, optional_argument,                                  \
    "Change method, or use no arguments to list methods.")                     \
  X('e', ':', ':', enable, no_argument, "Enable selected method.")             \
  X('d', ':', ':', disable, no_argument, "Disable selected method.")           \
  X('k', ':', ' ', key, required_argument,                                     \
    "Set the API key, by file, by stdin '-', or by argument. Enables method "  \
    "if not already.")

/**
 * define METHODS - Publishing methods, and how to interface with it.
 *
 * context: the functions are relative to the Publish namespace.
 *
 * Methods used for publishing, including: push, poll, load, save, check_key.
 */
#define METHODS                                                                \
  X(TELEGRAM, "Telegram",                                                      \
    Telegram::push,     /* Function used to push notification         */       \
    Telegram::poll,     /* Function used to check for other commands  */       \
    Telegram::load,     /* Function used to load necessary data       */       \
    Telegram::save,     /* Function used to save necessary data       */       \
    Telegram::check_key /* Function used to check and set the api key */       \
  )

// TODO: move to another part of file
/**
 * define METHOD_LENGTH - The length of the METHODS macro.
 *
 * This determines the count of macro X instances present in the METHODS macro.
 */
#define X(A, ...) 1,
#define COUNT(...) (sizeof((int[]){__VA_ARGS__}) / sizeof(int))
const unsigned int __method_length = COUNT(METHODS);
#define METHOD_LENGTH (int)__method_length
#undef COUNT
#undef X

/**
 * define XRP_ID_CODE - Essentially the name of the token.
 * define XRP_ADDRESS - Address of the token
 *
 * contex: only used in this file, see namespace Conf for in program use.
 *
 * Used to identify the specific token in which to look at.
 */
#define XRP_ID_CODE "FML"
#define XRP_ADDRESS "rw4tietmzbPG2G66UudSGaQ5uYztNow3gQ"

/**
 * namespace Conf - Functions and data for configuration, with the data
 *  potentially altered.
 *
 * context: this should not be changed as part of configuring settings.
 */
// Conf {{{
namespace Conf {
enum Methods {
#define X(a, ...) a,
  METHODS
#undef X
      NONE = -1
};

unsigned int request_wait_length = 0; ///< Length of waiting after a request

const std::string method_keys[METHOD_LENGTH] = {""};
const std::string method_secret_local[METHOD_LENGTH] = {
#define X(a, b, ...)                                                           \
  SECRET_FILE_PREFIX + boost::to_lower_copy(std::string("-") + b),
    METHODS
#undef X
};
const std::string method_secret_environment[METHOD_LENGTH] = {
#define X(a, b, ...)                                                           \
  std::string(SECRET_ENV_PREFIX) + boost::to_upper_copy(std::string(b)),
    METHODS
#undef X
};

enum Commandline_Flags {
#define X(A, A1, A2, B, ...) B = A,
  COMMANDLINE_ARGUMENTS
#undef X
};

const std::string id_code = XRP_ID_CODE;
const std::string address = XRP_ADDRESS;

const std::string save_file_prefix = SAVE_FILE_PREFIX;

const std::string method_save_files[METHOD_LENGTH] = {
#define X(a, b, ...)                                                           \
  std::string(SAVE_FILE_PREFIX) +                                              \
      boost::to_lower_copy(std::string("-" b ".data")),
    METHODS
#undef X
};
} // namespace Conf
// }}}

#undef XRP_ID_CODE
#undef XRP_ADDRESS
#endif
// vim: tabstop=2 shiftwidth=2 softtabstop=2 expandtab
