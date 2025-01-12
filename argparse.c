/**
 * @file argparse.c
 * @brief Implementation of parser for command-line options and arguments
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

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argparse.h"

/********************* ERROR MACROS *********************/

#define ARGPARSE_NO_ERROR()                                                    \
    ((argparse_error_t){ARGPARSE_NO_ERROR, NULL, NULL, 0})

#define ARGPARSE_INVALID_FLAG_OR_NAME_ERROR(msg, name)                         \
    ((argparse_error_t){ARGPARSE_INVALID_FLAG_OR_NAME_ERROR, (msg), (name), 0})

#define ARGPARSE_INVALID_TYPE_ERROR(type)                                      \
    ((argparse_error_t){ARGPARSE_INVALID_TYPE_ERROR, NULL, NULL, (type)})

#define ARGPARSE_INVALID_ACTION_ERROR(action)                                  \
    ((argparse_error_t){ARGPARSE_INVALID_ACTION_ERROR, NULL, NULL, (action)})

#define ARGPARSE_UNSUPPORTED_CHOICES_ERROR(name, flag)                         \
    ((argparse_error_t){ARGPARSE_UNSUPPORTED_CHOICES_ERROR, NULL, (name),      \
                        (flag)})

#define ARGPARSE_UNSUPPORTED_ACTION_ERROR(msg, name, flag)                     \
    ((argparse_error_t){ARGPARSE_UNSUPPORTED_ACTION_ERROR, (msg), (name),      \
                        (flag)})

#define ARGPARSE_CONFLICTING_OPTIONS_ERROR(msg, name, flag)                    \
    ((argparse_error_t){ARGPARSE_CONFLICTING_OPTIONS_ERROR, (msg), (name),     \
                        (flag)})

#define ARGPARSE_UNSUPPORTED_REQUIRE_ERROR(name, flag)                         \
    ((argparse_error_t){ARGPARSE_UNSUPPORTED_REQUIRE_ERROR, NULL, (name),      \
                        (flag)})

#define ARGPARSE_ARGUMENT_UNKNOWN_ERROR(name)                                  \
    ((argparse_error_t){ARGPARSE_ARGUMENT_UNKNOWN_ERROR, NULL, (name), 0})

#define ARGPARSE_INT_RANGE_EXCEEDED_ERROR(msg, name, flag)                     \
    ((argparse_error_t){ARGPARSE_INT_RANGE_EXCEEDED_ERROR, (msg), (name),      \
                        (flag)})

#define ARGPARSE_ARGUMENT_MISSING_VALUE_ERROR(name, flag)                      \
    ((argparse_error_t){ARGPARSE_ARGUMENT_MISSING_VALUE_ERROR, NULL, (name),   \
                        (flag)})

#define ARGPARSE_INVALID_CHOICE_ERROR(name, flag)                              \
    ((argparse_error_t){ARGPARSE_INVALID_CHOICE_ERROR, NULL, (name), (flag)})

#define ARGPARSE_MISSING_ARGUMENT_ERROR(name, flag)                            \
    ((argparse_error_t){ARGPARSE_MISSING_ARGUMENT_ERROR, NULL, (name), (flag)})

/*
 * Returns the error code and exits from the function if non-zero error code
 */
#define CHECK_ERROR(error)                                                     \
    do {                                                                       \
        if (error.error_val) {                                                 \
            return error;                                                      \
        }                                                                      \
    } while (0)

/*
 * Converts a lowercase letter to uppercase
 */
#define TO_UPPER(c) ((c) >= 'a' && (c) <= 'z' ? (c) - ('a' - 'A') : (c))

/*
 * Produces string from flag if name is not provided
 */
#define FORMAT_FN_STRING(error)                                                \
    (error.arg_name ? error.arg_name : (char[]){'-', error.arg_flag, '\0'})

/*
 * Epsilon tolerance for floating-point imprecision. Used for float choices
 */
#define EPSILON_FLOAT 1e-5

/*
 * Checks that `val` is within `EPSILON_FLOAT` of `tar`. Used for float choices
 */
#define WITHIN(val, tar)                                                       \
    ((val) >= (tar) - EPSILON_FLOAT && (val) <= (tar) + EPSILON_FLOAT)

/*
 * Prints the possible choices for the argument in a comma-delimited list. Wraps
 * choices in curly braces if curly is true. Helper for `argparse_print_help`
 */
static void print_choices(argparse_arg_t *arg, bool curly) {
    const char *start = curly ? " {" : " (choices: ";
    if (arg->choices_) {
        for (size_t j = 0; j < arg->num_choices_; j++) {
            switch (arg->type_) {
            case ARGPARSE_INT_TYPE:
                printf("%s%d", j ? "," : start, ((int *)arg->choices_)[j]);
                break;
            case ARGPARSE_FLOAT_TYPE:
                printf("%s%.3f", j ? "," : start, ((float *)arg->choices_)[j]);
                break;
            case ARGPARSE_STRING_TYPE:
                printf("%s\"%s\"", j ? "," : start,
                       ((const char **)arg->choices_)[j]);
                break;
            default:
                fprintf(stderr,
                        "should not get here, bools do not support choices\n");
                exit(EXIT_FAILURE);
            }
        }
        printf(curly ? "}" : ")");
    }
}

/*
 * Prints the formatted help message, including all the options and positional
 * arguments added to the parser. Invoked using `-h` or `--help` option
 */
void argparse_print_help(argument_parser_t *parser) {
    printf("usage: %s [-h]", parser->prog_);

    argparse_arg_t *arg = parser->options_;
    while (arg) {
        printf("%s", arg->required_ ? " " : " [");
        if (arg->action_ == ARGPARSE_BOOLEAN_OPTIONAL_ACTION) {
            if (arg->flag_ && arg->name_) {
                printf("-%c | %s | --no-%s", arg->flag_, arg->name_,
                       arg->name_ + 2);
            } else if (arg->flag_) {
                printf("-%c", arg->flag_);
            } else if (arg->name_) {
                printf("%s | --no-%s", arg->name_, arg->name_ + 2);
            }
        } else if (arg->flag_) {
            printf("-%c", arg->flag_);
        } else if (arg->name_) {
            printf("%s", arg->name_);
        }

        if (arg->action_ == ARGPARSE_STORE_ACTION && arg->choices_ == NULL) {
            if (arg->name_) {
                size_t i = 2;
                printf(" ");
                while (arg->name_[i]) {
                    printf("%c", arg->name_[i] == '-'
                                     ? '_'
                                     : TO_UPPER(arg->name_[i]));
                    i++;
                }
            } else {
                printf(" %c", TO_UPPER(arg->flag_));
            }
        }

        print_choices(arg, true);

        printf("%s", arg->required_ ? "" : "]");

        arg = arg->next_;
    }
    arg = parser->positional_args_;
    while (arg) {
        printf(" %s", arg->name_);
        arg = arg->next_;
    }
    printf("\n");

    if (parser->description_) {
        printf("\n%s\n", parser->description_);
    }

    if (parser->positional_args_) {
        printf("\npositional arguments:\n");
        arg = parser->positional_args_;
        while (arg) {
            printf("  %-24s %s\n", arg->name_, arg->help_ ? arg->help_ : "");
            arg = arg->next_;
        }
    }

    printf("\noptions:\n");
    printf("  -h, %-20s %s\n", "--help", "show this help message and exit");
    arg = parser->options_;
    while (arg) {
        printf("  -%c, %-20s %s", arg->flag_ ? arg->flag_ : ' ',
               arg->name_ ? arg->name_ : "", arg->help_ ? arg->help_ : "");
        print_choices(arg, false);
        printf("\n");
        arg = arg->next_;
    }

    if (parser->epilog_) {
        printf("\n%s\n", parser->epilog_);
    }
}

argparse_error_t argparse_init(argument_parser_t *parser, int argc, char **argv,
                               const char *description, const char *epilog) {
    parser->argc_ = --argc;
    parser->prog_ = argv++[0];
    parser->argv_ = argv;
    parser->description_ = description;
    parser->epilog_ = epilog;
    parser->options_ = NULL;
    parser->positional_args_ = NULL;

    return ARGPARSE_NO_ERROR();
}

/*
 * Checks if `string` starts with `prefix`
 */
static bool starts_with(const char *string, const char *prefix) {
    return strncmp(string, prefix, strlen(prefix)) == 0;
}

/*
 * Checks if the flags or names match between two arguments. Helper for
 * checking whether a conflicting argument is added in `argparse_add_argument`
 */
argparse_error_t check_added(argparse_arg_t *added, argparse_arg_t *arg) {
    if (arg->flag_ && arg->flag_ == added->flag_) {
        return ARGPARSE_CONFLICTING_OPTIONS_ERROR(
            "argparse_add_argument: option string %s already in use\n", NULL,
            arg->flag_);
    }

    if (arg->name_ && strcmp(arg->name_, added->name_) == 0) {
        return ARGPARSE_CONFLICTING_OPTIONS_ERROR(
            "argparse_add_argument: option string %s already in use\n",
            arg->name_, 0);
    }

    return ARGPARSE_NO_ERROR();
}

argparse_error_t argparse_add_argument(argument_parser_t *parser,
                                       argparse_arg_t *arg) {
    // check that at least one of flag or name is provided
    if (arg->flag_ == NO_FLAG &&
        (arg->name_ == NO_NAME || strlen(arg->name_) == 0)) {
        return ARGPARSE_INVALID_FLAG_OR_NAME_ERROR(
            "argparse_add_argument: argument must "
            "contain at least one of flag or name\n",
            NULL);
    }

    // check that type is valid
    switch (arg->type_) {
    case ARGPARSE_INT_TYPE:
    case ARGPARSE_FLOAT_TYPE:
    case ARGPARSE_BOOL_TYPE:
    case ARGPARSE_STRING_TYPE:
        break;
    default:
        return ARGPARSE_INVALID_TYPE_ERROR(arg->type_);
    }

    bool positional = arg->name_ && !starts_with(arg->name_, "-");
    bool bool_action = false;

    // check that action is valid
    // check that store and boolean actions have a bool type
    // check that non-store actions do not provide choices
    switch (arg->action_) {
    case ARGPARSE_STORE_TRUE_ACTION:
    case ARGPARSE_STORE_FALSE_ACTION:
    case ARGPARSE_BOOLEAN_OPTIONAL_ACTION:
        bool_action = true;
        if (arg->type_ != ARGPARSE_BOOL_TYPE) {
            return ARGPARSE_UNSUPPORTED_ACTION_ERROR(
                "argparse_add_argument: store true/false and boolean optional "
                "actions must have bool type for %s\n",
                arg->name_, arg->flag_);
        }
        goto remaining;
    case ARGPARSE_COUNT_ACTION:
        if (arg->type_ != ARGPARSE_INT_TYPE) {
            return ARGPARSE_UNSUPPORTED_ACTION_ERROR(
                "argparse_add_argument: count action must have int type for %s",
                arg->name_, arg->flag_);
        }
    remaining:
        if (arg->choices_) {
            return ARGPARSE_UNSUPPORTED_CHOICES_ERROR(arg->name_, arg->flag_);
        }
        if (positional) {
            return ARGPARSE_UNSUPPORTED_ACTION_ERROR(
                "argparse_add_argument: positional argument %s must have store "
                "action\n",
                arg->name_, 0);
        }
        if (arg->required_) {
            return ARGPARSE_UNSUPPORTED_REQUIRE_ERROR(arg->name_, arg->flag_);
        }
        __attribute__((fallthrough));
    case ARGPARSE_STORE_ACTION:
        if (!bool_action && arg->type_ == ARGPARSE_BOOL_TYPE) {
            return ARGPARSE_UNSUPPORTED_ACTION_ERROR(
                "argparse_add_argument: store or count actions must have "
                "non-bool type for %s\n",
                arg->name_, arg->flag_);
        }
        break;
    default:
        return ARGPARSE_INVALID_ACTION_ERROR(arg->action_);
    }

    // check that positional argument does not have a flag as well
    if (positional && arg->flag_) {
        return ARGPARSE_INVALID_FLAG_OR_NAME_ERROR(
            "argparse_add_argument: option string '%s' must start with '-'\n",
            arg->name_);
    }

    // check that optional argument name is not just '--'
    if (arg->name_ && !positional && strlen(arg->name_) == 2) {
        return ARGPARSE_INVALID_FLAG_OR_NAME_ERROR(
            "argparse_add_argument: must provide name for options like '--'\n",
            NULL);
    }

    // checks that flag or name does not overlap with help options
    if (arg->flag_ == 'h') {
        return ARGPARSE_CONFLICTING_OPTIONS_ERROR(
            "argparse_add_argument: -h flag reserved for help\n", NULL, 0);
    }
    if (arg->name_ && strcmp(arg->name_, "--help") == 0) {
        return ARGPARSE_CONFLICTING_OPTIONS_ERROR(
            "argparse_add_argument: --help option string reserved for help\n",
            NULL, 0);
    }

    // check that flag and name are unused
    argparse_arg_t *added =
        positional ? parser->positional_args_ : parser->options_;
    argparse_arg_t *added_prev = NULL;

    argparse_error_t error;
    while (added) {
        error = check_added(added, arg);
        CHECK_ERROR(error);
        added_prev = added;
        added = added->next_;
    }

    if (added_prev) {
        added_prev->next_ = arg;
    } else {
        if (positional) {
            parser->positional_args_ = arg;
        } else {
            parser->options_ = arg;
        }
    }

    return ARGPARSE_NO_ERROR();
}

argparse_error_t argparse_add_arguments(argument_parser_t *parser,
                                        argparse_arg_t *args, size_t num_args) {
    argparse_error_t error;
    for (size_t i = 0; i < num_args; i++) {
        error = argparse_add_argument(parser, &args[i]);
        CHECK_ERROR(error);
    }
    return ARGPARSE_NO_ERROR();
}

/*
 * Struct to store flag, name or index to search in list of arguments. The `tag`
 * field indicates which member of `u` to access. `prefix` should be unused
 * except when prepending a prefix when searching for a name
 */
typedef struct {
    union {
        char flag;
        const char *name;
        size_t index;
    } u;
    const char *prefix;
    enum { FLAG_TO_FIND, NAME_TO_FIND, INDEX_TO_FIND } tag;
} arg_to_find;

/*
 * Find an argument `arg` in the `arg_list`. Checks flag, name or index member
 * based on the tag
 */
static argparse_arg_t *find_arg(argparse_arg_t *arg_list, arg_to_find arg) {
    argparse_arg_t *curr_arg = arg_list;

    size_t i = 0;
    while (curr_arg) {
        switch (arg.tag) {
        case FLAG_TO_FIND:
            if (arg.u.flag == curr_arg->flag_) {
                return curr_arg;
            }
            break;
        case NAME_TO_FIND:
            if (arg.prefix) {
                if (starts_with(curr_arg->name_, arg.prefix) &&
                    strcmp(arg.u.name, curr_arg->name_ + strlen(arg.prefix)) ==
                        0) {
                    return curr_arg;
                }
            } else {
                if (strcmp(arg.u.name, curr_arg->name_) == 0) {
                    return curr_arg;
                }
            }
            break;
        case INDEX_TO_FIND:
            if (arg.u.index == i) {
                return curr_arg;
            }
            break;
        default:
            fprintf(stderr, "find_arg: should not ever get here\n");
            exit(EXIT_FAILURE);
        }

        curr_arg = curr_arg->next_;
        i++;
    }

    return NULL;
}

/*
 * Updates the memory location pointed to by `arg->value_` with `value`.
 * Performs type-cast based on `arg->type_`. Helper for `get_and_update_value`
 */
static argparse_error_t update_value(argparse_arg_t *arg, void *value,
                                     bool string) {
    switch (arg->type_) {
    case ARGPARSE_INT_TYPE: {
        errno = 0;
        long res;
        res = string ? strtol(value, NULL, 0) : *(int *)value;
        if (errno == ERANGE || res > INT32_MAX || res < INT32_MIN) {
            return ARGPARSE_INT_RANGE_EXCEEDED_ERROR(
                "argparse_parse_args: value for %s exceeds range of integer\n",
                arg->name_, arg->flag_);
        }
        *(int *)arg->value_ = (int)res;
        break;
    }
    case ARGPARSE_FLOAT_TYPE:
        *(float *)arg->value_ = string ? strtof(value, NULL) : *(float *)value;
        break;
    case ARGPARSE_BOOL_TYPE:
        *(bool *)arg->value_ = *(bool *)value;
        break;
    case ARGPARSE_STRING_TYPE:
        *(const char **)arg->value_ = value;
        break;
    default:
        fprintf(stderr, "argparse_parse_args: should not ever get here\n");
        exit(EXIT_FAILURE);
    }
    return ARGPARSE_NO_ERROR();
}

/*
 * Gets the value for the corresponding argument from `argv` and updates the
 * the `value_` field of that argument. Advances the index `i` in the parsing
 * function if the argument is positional. Helper for `argparse_parse_args`
 */
static argparse_error_t get_and_update_value(argument_parser_t *parser,
                                             argparse_arg_t *arg, int *i,
                                             bool positional, bool negated) {
    arg->count_++;
    bool tmp_bool = arg->num_choices_ == 0;
    void *value = NULL;

    switch (arg->action_) {
    case ARGPARSE_STORE_ACTION:
        if (!positional && *i + 1 == parser->argc_) {
            return ARGPARSE_ARGUMENT_MISSING_VALUE_ERROR(parser->argv_[*i], 0);
        }

        value = positional ? (void *)parser->argv_[*i]
                           : (void *)parser->argv_[++(*i)];

        // check if the value is in the choices
        for (size_t j = 0; j < arg->num_choices_; j++) {
            switch (arg->type_) {
            case ARGPARSE_INT_TYPE:
                errno = 0;
                long res = strtol(value, NULL, 0);
                if (errno == ERANGE || res > INT32_MAX || res < INT32_MIN) {
                    return ARGPARSE_INT_RANGE_EXCEEDED_ERROR(
                        "argparse_parse_args: choice for %s exceeds range of "
                        "integer\n",
                        arg->name_, arg->flag_);
                }
                tmp_bool = ((int *)arg->choices_)[j] == (int)res;
                break;
            case ARGPARSE_FLOAT_TYPE:
                tmp_bool =
                    WITHIN(strtof(value, NULL), ((float *)arg->choices_)[j]);
                break;
            case ARGPARSE_BOOL_TYPE:
                // non-store actions and bool types do not support choices
                break;
            case ARGPARSE_STRING_TYPE:
                tmp_bool = strcmp(((const char **)arg->choices_)[j],
                                  (const char *)value) == 0;
                break;
            default:
                fprintf(stderr, "should not get here\n");
                exit(EXIT_FAILURE);
            }
            if (tmp_bool) {
                break;
            }
        }
        if (!tmp_bool) {
            return ARGPARSE_INVALID_CHOICE_ERROR(
                parser->argv_[*i - !positional], 0);
        }

        break;

    case ARGPARSE_STORE_TRUE_ACTION:
        tmp_bool = true;
        value = &tmp_bool;
        break;

    case ARGPARSE_STORE_FALSE_ACTION:
        tmp_bool = false;
        value = &tmp_bool;
        break;

    case ARGPARSE_BOOLEAN_OPTIONAL_ACTION:
        tmp_bool = !negated;
        value = &tmp_bool;
        break;

    case ARGPARSE_COUNT_ACTION:
        // count will be set when argument is finalised
        return ARGPARSE_NO_ERROR();

    default:
        fprintf(stderr, "get_and_update_value: should not ever get here\n");
        exit(EXIT_FAILURE);
    }

    argparse_error_t error =
        update_value(arg, value, arg->action_ == ARGPARSE_STORE_ACTION);
    CHECK_ERROR(error);

    return ARGPARSE_NO_ERROR();
}

/*
 * Finalises the argument after all the arguments have been parsed. Values for
 * count actions are updated and missing arguments result in an error
 */
static argparse_error_t finalise_arg(argparse_arg_t *arg) {
    if (arg->action_ == ARGPARSE_COUNT_ACTION) {
        return update_value(arg, &arg->count_, false);
    }

    if (arg->count_ == 0 && arg->required_) {
        return ARGPARSE_MISSING_ARGUMENT_ERROR(arg->name_, arg->flag_);
    }

    return ARGPARSE_NO_ERROR();
}

argparse_error_t argparse_parse_args(argument_parser_t *parser) {
    size_t positional_i = 0;
    for (int i = 0; i < parser->argc_; i++) {
        const char *arg_string = parser->argv_[i];
        argparse_arg_t *arg = NULL;
        bool positional = false, negated = false;
        if (strcmp(arg_string, "-h") == 0 ||
            strcmp(arg_string, "--help") == 0) {
            // display usage string
            argparse_print_help(parser);
            exit(EXIT_SUCCESS);
        } else if (strlen(arg_string) == 2 && starts_with(arg_string, "-")) {
            // option flag
            arg = find_arg(parser->options_,
                           (arg_to_find){.tag = FLAG_TO_FIND,
                                         .prefix = NULL,
                                         .u.flag = *(arg_string + 1)});
        } else if (starts_with(arg_string, "-")) {
            if (starts_with(arg_string, "--no-")) {
                // potential boolean optional
                arg = find_arg(parser->options_,
                               (arg_to_find){.tag = NAME_TO_FIND,
                                             .prefix = "--",
                                             .u.name = arg_string + 5});
                negated = true;
            }

            if (arg == NULL ||
                arg->action_ != ARGPARSE_BOOLEAN_OPTIONAL_ACTION) {
                // option name
                arg = find_arg(parser->options_,
                               (arg_to_find){.tag = NAME_TO_FIND,
                                             .prefix = NULL,
                                             .u.name = arg_string});
                negated = false;
            }

            if (arg == NULL) {
                // grouped option flags
                size_t j = 1;
                while (arg_string[j]) {
                    arg = find_arg(parser->options_,
                                   (arg_to_find){.tag = FLAG_TO_FIND,
                                                 .prefix = NULL,
                                                 .u.flag = *(arg_string + j)});
                    if (arg == NULL) {
                        goto unknown;
                    }
                    argparse_error_t error = get_and_update_value(
                        parser, arg, &i, arg->action_ != ARGPARSE_STORE_ACTION,
                        false);
                    CHECK_ERROR(error);
                    j++;
                }
                continue;
            }
        } else {
            // positional argument
            arg = find_arg(parser->positional_args_,
                           (arg_to_find){.tag = INDEX_TO_FIND,
                                         .prefix = NULL,
                                         .u.index = positional_i++});
            positional = true;
        }

    unknown:
        if (arg == NULL) {
            return ARGPARSE_ARGUMENT_UNKNOWN_ERROR(arg_string);
        }

        argparse_error_t error =
            get_and_update_value(parser, arg, &i, positional, negated);
        CHECK_ERROR(error);
    }

    // set default values for missing options and raise an error if any missing
    // arguments are required
    argparse_arg_t *args[] = {parser->options_, parser->positional_args_};
    argparse_error_t error;
    for (size_t i = 0; i < 2; i++) {
        argparse_arg_t *arg = args[i];
        while (arg) {
            error = finalise_arg(arg);
            CHECK_ERROR(error);
            arg = arg->next_;
        }
    }

    return ARGPARSE_NO_ERROR();
}

argparse_error_val argparse_check_error(argparse_error_t error) {
    if (error.error_val != ARGPARSE_NO_ERROR) {
        fprintf(stderr, "\n");
    }
    switch (error.error_val) {
    case ARGPARSE_NO_ERROR:
        break;
    case ARGPARSE_INVALID_FLAG_OR_NAME_ERROR:
    case ARGPARSE_UNSUPPORTED_ACTION_ERROR:
    case ARGPARSE_CONFLICTING_OPTIONS_ERROR:
    case ARGPARSE_INT_RANGE_EXCEEDED_ERROR:
        if (error.arg_name || error.arg_flag) {
            fprintf(stderr, error.error_msg, FORMAT_FN_STRING(error));
        } else {
            fprintf(stderr, "%s", error.error_msg);
        }
        break;
    case ARGPARSE_INVALID_TYPE_ERROR:
        fprintf(stderr, "argparse_add_argument: %d is not a valid type\n",
                error.arg_flag);
        break;
    case ARGPARSE_INVALID_ACTION_ERROR:
        fprintf(stderr, "argparse_add_argument: %d is not a valid action\n",
                error.arg_flag);
        break;
    case ARGPARSE_UNSUPPORTED_CHOICES_ERROR:
        fprintf(
            stderr,
            "argparse_parse_args: only store action supports choices for %s\n",
            FORMAT_FN_STRING(error));
        break;
    case ARGPARSE_UNSUPPORTED_REQUIRE_ERROR:
        fprintf(stderr,
                "argparse_add_argument: only store actions can be required for "
                "%s\n",
                FORMAT_FN_STRING(error));
        break;
    case ARGPARSE_ARGUMENT_UNKNOWN_ERROR:
        fprintf(stderr, "argparse_parse_args: unknown %s argument\n",
                error.arg_name);
        break;
    case ARGPARSE_ARGUMENT_MISSING_VALUE_ERROR:
        fprintf(stderr, "argparse_parse_args: expected value for %s\n",
                error.arg_name);
        break;
    case ARGPARSE_INVALID_CHOICE_ERROR:
        fprintf(stderr,
                "argparse_parse_args: value provided for %s is not a valid "
                "choice\n",
                error.arg_name);
        break;
    case ARGPARSE_MISSING_ARGUMENT_ERROR:
        fprintf(stderr, "argparse_parse_args: missing required argument %s\n",
                FORMAT_FN_STRING(error));
        break;
    default:
        fprintf(stderr, "unknown error: %d\n", error.error_val);
        exit(EXIT_FAILURE);
    }
    return error.error_val;
}

void argparse_check_error_and_exit(argparse_error_t error) {
    if (argparse_check_error(error)) {
        exit(error.error_val);
    }
}
