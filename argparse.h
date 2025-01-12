/**
 * @file argparse.h
 * @brief Header file for parser for command-line options and arguments
 *
 * MIT License
 *
 * Copyright (c) 2025 pollyren
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ARGPARSE_H
#define ARGPARSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

/********************* STRUCTS AND ENUMS *********************/

/**
 * @brief Enum representing errors in adding or parsing arguments
 */
typedef enum {
    /// no error occured
    ARGPARSE_NO_ERROR = 0,

    /** Add argument errors */
    /// the flag or name is invalid
    ARGPARSE_INVALID_FLAG_OR_NAME_ERROR = -1,
    /// type provided does not match available types
    ARGPARSE_INVALID_TYPE_ERROR = -2,
    /// action provided does not match available actions
    ARGPARSE_INVALID_ACTION_ERROR = -3,
    /// the presence of choices is not supported (e.g., for flags and counts)
    ARGPARSE_UNSUPPORTED_CHOICES_ERROR = -4,
    /// the action of the argument does not match its type
    ARGPARSE_UNSUPPORTED_ACTION_ERROR = -5,
    /// an argument with the same flag or name has already been added
    ARGPARSE_CONFLICTING_OPTIONS_ERROR = -6,
    /// argument cannot be required based on action
    ARGPARSE_UNSUPPORTED_REQUIRE_ERROR = -7,

    /** Parse argument errors */
    /// argument provided has not been added to parser
    ARGPARSE_ARGUMENT_UNKNOWN_ERROR = -8,
    /// no value is provided for a store-action argument
    ARGPARSE_ARGUMENT_MISSING_VALUE_ERROR = -9,
    /// integer argument value exceeds the range of an int
    ARGPARSE_INT_RANGE_EXCEEDED_ERROR = -10,
    /// value of argument does not match choices provided
    ARGPARSE_INVALID_CHOICE_ERROR = -11,
    /// argument is not provided on command line
    ARGPARSE_MISSING_ARGUMENT_ERROR = -12
} argparse_error_val;

/**
 * @brief Struct representing an argparse error
 *
 * Contains information on the error code, the error message and the argument
 * causing the error.
 */
typedef struct {
    /// error code
    argparse_error_val error_val;
    /// error message for printing (NULL if using generic error message)
    const char *error_msg;
    /// name of argument causing the error
    const char *arg_name;
    /// flag of argument causing the error
    char arg_flag;
} argparse_error_t;

/**
 * @brief Enum representing possible actions for an argument
 */
typedef enum {
    /// store the value of the argument, must be non-bool type
    ARGPARSE_STORE_ACTION,
    /// store true if the argument is present, must be bool type
    ARGPARSE_STORE_TRUE_ACTION,
    /// store false if the argument is present, must be bool type
    ARGPARSE_STORE_FALSE_ACTION,
    /// count the occurrences of argument, must be int type
    ARGPARSE_COUNT_ACTION,
    /// boolean action, similar to a flag, must be bool type
    ARGPARSE_BOOLEAN_OPTIONAL_ACTION
} argparse_action_t;

/**
 * @brief Enum representing possible types for an argument
 */
typedef enum {
    /// integer argument type, value should be `int`
    ARGPARSE_INT_TYPE,
    /// float argument type, value should be `float`
    ARGPARSE_FLOAT_TYPE,
    /// boolean argument type, value should be `bool`
    ARGPARSE_BOOL_TYPE,
    /// string argument type, value should be `const char *`
    ARGPARSE_STRING_TYPE
} argparse_type_t;

/**
 * @brief Struct representing an individual argument
 *
 * Contains metadata on argument type, next pointer and count, as well as
 * user-provided information on the flag and name and other argument data.
 */
typedef struct argparse_arg_t {
    /** Argument metadata */
    /// argument type
    argparse_type_t type_;
    /// pointer to the next argument, for linked list
    struct argparse_arg_t *next_;
    /// count occurrences when arguments are parsed
    int count_;

    /** User-provided information */
    /// single-character flag for the argument
    const char flag_;
    /// full name of the argument
    const char *name_;
    /// pointer to where the value should be stored, type should be consistent
    void *value_;
    /// action to perform when arguments are parsed
    argparse_action_t action_;
    /// help description for the argument
    const char *help_;
    /// whether the option is required
    bool required_;
    /// array of valid choices, type should be consistent
    void *choices_;
    /// number of valid choices choices provided in the array
    size_t num_choices_;
} argparse_arg_t;

/**
 * @brief Struct representing the parser
 *
 * Contains argc, argv, program name, program description and help message
 * epilog, as well as linked lists to all the options and positional arguments
 * that have been added to the parser.
 */
typedef struct {
    /// number of arguments
    int argc_;
    /// array of argument strings
    char **argv_;

    /// program name
    const char *prog_;
    /// text to display before argument help (NULL if no description)
    const char *description_;
    /// text to display after argument help (NULL if no epilog)
    const char *epilog_;

    /// linked list of non-positional options added to parser
    argparse_arg_t *options_;
    /// linked list of positional arguments added to parser
    argparse_arg_t *positional_args_;
} argument_parser_t;

/********************* FUNCTION DECLARATIONS *********************/

/**
 * @brief Prints the usage message
 *
 * @param parser Pointer to the `argument_parser_t`
 *
 * @note Only the options and positional arguments added to the parser will be
 * printed. This function can be called independently or by providing `-h` or
 * `--help` command-line options.
 */
void argparse_print_help(argument_parser_t *parser);

/**
 * @brief Initialises the parser
 *
 * @param parser Pointer to the `argument_parser_t` to initialise
 * @param argc Argument count (from `main`)
 * @param argv Argument vector (from `main`)
 * @param description Text to display before help (NULL if no description)
 * @param epilog Text to display after help (NULL if no epiilog)
 * @return An `argparse_error_t` indicating success or failure
 */
argparse_error_t argparse_init(argument_parser_t *parser, int argc, char **argv,
                               const char *description, const char *epilog);

/**
 * @brief Adds a single argument to the parser
 *
 * @param parser Pointer to the parser
 * @param arg Pointer to the argument to add
 * @return An `argparse_error_t` indicating success or failure
 *
 * @note The argument should be initialised. The lifetime of `*arg` must also
 * extend at least until `argparse_parse_args` is called. The parser retains a
 * pointer to `*arg`, so having `*arg` go out-of-scope when the arguments are
 * parsed can lead to undefined behaviour. This means that if the arguments are
 * created in a different function from the one where `argparse_parse_args` is
 * called, the user is responsible for ensuring that `arg` is correctly
 * dynamically allocated.
 */
argparse_error_t argparse_add_argument(argument_parser_t *parser,
                                       argparse_arg_t *arg);

/**
 * @brief Adds multiple arguments to the parser
 *
 * @param parser Pointer to the parser
 * @param args Array of arguments to add
 * @param num_args The number of arguments in args
 * @return An `argparse_error_t` indicating success or failure
 *
 * @note All arguments should be initialised. The lifetime of each argument in 
 * `args` must also extend at least until `argparse_parse_args` is called, as 
 * the parser retains a pointer to each argument. See function comment for
 * `argparse_add_argument` for additional note.
 */
argparse_error_t argparse_add_arguments(argument_parser_t *parser,
                                        argparse_arg_t *args, size_t num_args);

/**
 * @brief Parses the command-line arguments
 *
 * @param parser Pointer to the parser
 * @return An `argparse_error_t` indicating success or failure
 *
 * @note The arguments should already be added to the parser. The parser and
 * all the arguments that have been added to the parser must also be in scope
 * when this function is called to avoid undefined behaviour.
 */
argparse_error_t argparse_parse_args(argument_parser_t *parser);

/**
 * @brief Prints error message if there is an error
 *
 * @param error Error struct containing error information
 * @return An `argparse_error_val` containing the exit code of the error
 */
argparse_error_val argparse_check_error(argparse_error_t error);

/**
 * @brief Prints error message and exits if there is an error
 *
 * @param error Error struct containing error information
 *
 * @note This function exits the program and returns the number corresponding
 * to the error code of the error, only if there is a non-zero exit code.
 */
void argparse_check_error_and_exit(argparse_error_t error);

/********************* CONVENIENCE MACROS *********************/

/**
 * Convenience placeholders for creating arguments. Mostly used for readability
 * in the convenience macros for creating arguments.
 */
#define NO_DESCRIPTION NULL
#define NO_EPILOG NULL
#define NO_FLAG '\0'
#define NO_NAME NULL
#define NO_HELP NULL
#define REQUIRED true
#define OPTIONAL false
#define NO_CHOICES NULL

/********************* BASIC ARGUMENT MACROS *********************/

/**
 * @defgroup ARGPARSE_MACROS Argument macros
 * @brief Macros for creating arguments
 *
 * These macros make it more convenient to create a `argparse_arg_t` argument.
 * They provide a shorthand for defining positional arguments and options
 * with various properties, such as required options or choices lists.
 *
 * For the following macros:
 * - `type`: must be `INT`, `FLOAT`, `BOOL` or `STRING`
 * - `flag`: a character flag, used as short name of argument
 * - `name`: long-form name of argument, should generally have `--` prefix
 * - `res`: pointer to the variable where parsed result should be stored
 * - `help`: a `const char *` describing the argument, for the help message
 * - `choices`: an array of valid choices
 * - `num_choices`: the number of entries within the `choices` array
 *
 * @note The type of `res` and the type of `choices` must be consistent with the
 * type of the argument.
 *
 * @{
 */

/**
 * @brief Defines a basic argparse argument
 * @param type Argument type (must be INT, FLOAT, BOOL or STRING)
 * @param ... Additional `argument_arg_t` fields
 *
 * @note All the fields of the argument, aside from type_, next_ and count_,
 * must be initialised in the varadic args.
 */
#define ARGPARSE_ARG(type, ...)                                                \
    ((argparse_arg_t){.type_ = ARGPARSE_##type##_TYPE,                         \
                      .next_ = NULL,                                           \
                      .count_ = 0,                                             \
                      __VA_ARGS__})

/**
 * @brief Initialises an already allocated argparse argument
 * @param ptr Pointer to memory allocated for an `argparse_arg_t` 
 * @param type Argument type (must be INT, FLOAT, BOOL or STRING)
 * @param ... Additional `argparse_arg_t` fields
 *
 * @note The pointer `ptr` must not be NULL and should point to memory allocated
 * to hold an `argparse_arg_t`. All the fields of the argument, aside from 
 * type_, next_ and count_, must be initialised in the varadic args.
 */
#define ARGPARSE_ARG_INIT(ptr, type, ...)                                      \
    do {                                                                       \
        *(ptr) = (argparse_arg_t){.type_ = ARGPARSE_##type##_TYPE,             \
                                  .next_ = NULL,                               \
                                  .count_ = 0,                                 \
                                  __VA_ARGS__};                                \
    } while (0)

/********************* POSITIONAL ARGUMENT MACROS *********************/

/**
 * @brief Creates a positional argument without choices
 */
#define ARGPARSE_POSITIONAL(type, name, res, help)                             \
    ARGPARSE_ARG(type, '\0', (name), (res), ARGPARSE_STORE_ACTION, (help),     \
                 REQUIRED, NO_CHOICES, 0)

/**
 * @brief Creates a positional argument with choices
 */
#define ARGPARSE_POSITIONAL_WITH_CHOICES(type, name, res, help, choices,       \
                                         num_choices)                          \
    ARGPARSE_ARG(type, '\0', (name), (res), ARGPARSE_STORE_ACTION, (help),     \
                 REQUIRED, (choices), (num_choices))

/********************* OPTION MACROS *********************/

/**
 * @brief Creates an option
 */
#define ARGPARSE_OPTION(type, flag, name, res, help)                           \
    ARGPARSE_ARG(type, (flag), (name), (res), ARGPARSE_STORE_ACTION, (help),   \
                 OPTIONAL, NO_CHOICES, 0)

/**
 * @brief Creates a required option
 */
#define ARGPARSE_OPTION_REQUIRED(type, flag, name, res, help)                  \
    ARGPARSE_ARG(type, (flag), (name), (res), ARGPARSE_STORE_ACTION, (help),   \
                 REQUIRED, NO_CHOICES, 0)

/**
 * @brief Creates an option with choices
 */
#define ARGPARSE_OPTION_WITH_CHOICES(type, flag, name, res, help, choices,     \
                                     num_choices)                              \
    ARGPARSE_ARG(type, (flag), (name), (res), ARGPARSE_STORE_ACTION, (help),   \
                 OPTIONAL, (choices), (num_choices))

/********************* SPECIAL OPTION MACROS *********************/

/**
 * @brief Creates an integer option that counts the occurrences of the argument
 * on the command line when parsing
 */
#define ARGPARSE_COUNT(flag, name, res, help)                                  \
    ARGPARSE_ARG(INT, (flag), (name), (res), ARGPARSE_COUNT_ACTION, (help),    \
                 OPTIONAL, NO_CHOICES, 0)

/**
 * @brief Creates a boolean toggle option that stores true if --name is
 * provided on the command line and false if --no-name is provided
 */
#define ARGPARSE_TOGGLE(flag, name, res, help)                                 \
    ARGPARSE_ARG(BOOL, (flag), (name), (res),                                  \
                 ARGPARSE_BOOLEAN_OPTIONAL_ACTION, (help), OPTIONAL,           \
                 NO_CHOICES, 0)

/**
 * @brief Creates a boolean option that stores true if argument is provided
 * on the command line when parsing
 */
#define ARGPARSE_FLAG_TRUE(flag, name, res, help)                              \
    ARGPARSE_ARG(BOOL, (flag), (name), (res), ARGPARSE_STORE_TRUE_ACTION,      \
                 (help), OPTIONAL, NO_CHOICES, 0)

/**
 * @brief Creates a boolean option that stores false if argument is provided
 * on the command line when parsing
 */
#define ARGPARSE_FLAG_FALSE(flag, name, res, help)                             \
    ARGPARSE_ARG(BOOL, (flag), (name), (res), ARGPARSE_STORE_FALSE_ACTION,     \
                 (help), OPTIONAL, NO_CHOICES, 0)

/** @} */ // end of ARGPARSE_MACROS group

#ifdef __cplusplus
}
#endif

#endif /* ARGPARSE_H */
