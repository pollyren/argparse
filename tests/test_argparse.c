#include "argparse.h"
#include <criterion/criterion.h>
#include <stdio.h>

#define TESTS_PRINT_HELP 0
#define EPSILON_FLOAT 1e-5
#define WITHIN(val, tar)                                                       \
    ((val) >= (tar) - EPSILON_FLOAT && (val) <= (tar) + EPSILON_FLOAT)

void newlines(void) {
    if (TESTS_PRINT_HELP)
        printf("\n\n========\n\n\n");
}

// STORING INTS
Test(argparse, store_single_int, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--value", "52"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 3, argv, "Storing an int", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 2);
    cr_assert_str_eq(parser.argv_[0], "--value");
    cr_assert_str_eq(parser.argv_[1], "52");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    int value = 9999;
    argparse_arg_t arg1 =
        ARGPARSE_OPTION(INT, 'v', "--value", &value, "a value");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(value, 52);
    cr_assert_eq(arg1.count_, 1);
}

Test(argparse, store_multi_int, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--value1", "52", "--value2",
                    "42",     "--sum",    "94"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 7, argv, "Storing multiple ints", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 6);
    cr_assert_str_eq(parser.argv_[0], "--value1");
    cr_assert_str_eq(parser.argv_[1], "52");
    cr_assert_str_eq(parser.argv_[2], "--value2");
    cr_assert_str_eq(parser.argv_[3], "42");
    cr_assert_str_eq(parser.argv_[4], "--sum");
    cr_assert_str_eq(parser.argv_[5], "94");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    int x = 9999, y = 9999, sum = -1;
    argparse_arg_t arg1 =
        ARGPARSE_OPTION(INT, 'x', "--value1", &x, "value to add");
    argparse_arg_t arg2 =
        ARGPARSE_OPTION(INT, 'y', "--value2", &y, "another value to add");
    argparse_arg_t arg3 = ARGPARSE_OPTION(INT, 's', "--sum", &sum, "sum");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, &arg2);
    cr_assert_eq(parser.options_->next_->next_, &arg3);
    cr_assert_eq(parser.options_->next_->next_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(x, 52);
    cr_assert_eq(y, 42);
    cr_assert_eq(sum, 94);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
}

Test(argparse, store_multi_int_override, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--value1", "52",    "--value2", "42",
                    "--sum",  "94",       "--sum", "111"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 9, argv, "Storing multiple ints", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 8);
    cr_assert_str_eq(parser.argv_[0], "--value1");
    cr_assert_str_eq(parser.argv_[1], "52");
    cr_assert_str_eq(parser.argv_[2], "--value2");
    cr_assert_str_eq(parser.argv_[3], "42");
    cr_assert_str_eq(parser.argv_[4], "--sum");
    cr_assert_str_eq(parser.argv_[5], "94");
    cr_assert_str_eq(parser.argv_[6], "--sum");
    cr_assert_str_eq(parser.argv_[7], "111");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    int x = 9999, y = 9999, sum = -1;
    argparse_arg_t arg1 =
        ARGPARSE_OPTION(INT, 'x', "--value1", &x, "value to add");
    argparse_arg_t arg2 =
        ARGPARSE_OPTION(INT, 'y', "--value2", &y, "another value to add");
    argparse_arg_t arg3 = ARGPARSE_OPTION(INT, 's', "--sum", &sum, "sum");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, &arg2);
    cr_assert_eq(parser.options_->next_->next_, &arg3);
    cr_assert_eq(parser.options_->next_->next_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(x, 52);
    cr_assert_eq(y, 42);
    cr_assert_eq(sum, 111);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 2);
}

Test(argparse, store_multi_int_with_optional, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "-x", "52", "-y", "42", "--sum", "94"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 7, argv, "Storing multiple ints", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 6);
    cr_assert_str_eq(parser.argv_[0], "-x");
    cr_assert_str_eq(parser.argv_[1], "52");
    cr_assert_str_eq(parser.argv_[2], "-y");
    cr_assert_str_eq(parser.argv_[3], "42");
    cr_assert_str_eq(parser.argv_[4], "--sum");
    cr_assert_str_eq(parser.argv_[5], "94");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    int x = 9999, y = 9999, z = 23, sum = -1;
    argparse_arg_t arg1 =
        ARGPARSE_OPTION(INT, 'x', "--value1", &x, "value to add");
    argparse_arg_t arg2 =
        ARGPARSE_OPTION(INT, 'y', "--value2", &y, "another value to add");
    argparse_arg_t arg3 =
        ARGPARSE_OPTION(INT, 'z', "--value3", &z, "yet another value to add");
    argparse_arg_t arg4 = ARGPARSE_OPTION(INT, 's', "--sum", &sum, "the sum");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg4)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, &arg2);
    cr_assert_eq(parser.options_->next_->next_, &arg3);
    cr_assert_eq(parser.options_->next_->next_->next_, &arg4);
    cr_assert_eq(parser.options_->next_->next_->next_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(x, 52);
    cr_assert_eq(y, 42);
    cr_assert_eq(z, 23);
    cr_assert_eq(sum, 94);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 0);
    cr_assert_eq(arg4.count_, 1);
}

Test(argparse, store_int_missing_required, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "-x", "52", "-y", "42"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 5, argv, "Missing a required value", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 4);
    cr_assert_str_eq(parser.argv_[0], "-x");
    cr_assert_str_eq(parser.argv_[1], "52");
    cr_assert_str_eq(parser.argv_[2], "-y");
    cr_assert_str_eq(parser.argv_[3], "42");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    int x = 9999, y = 9999, sum = -1;
    argparse_arg_t arg1 =
        ARGPARSE_OPTION(INT, 'x', "--value1", &x, "value to add");
    argparse_arg_t arg2 =
        ARGPARSE_OPTION(INT, 'y', "--value2", &y, "another value to add");
    argparse_arg_t arg3 =
        ARGPARSE_OPTION_REQUIRED(INT, 's', "--sum", &sum, "the sum");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, &arg2);
    cr_assert_eq(parser.options_->next_->next_, &arg3);
    cr_assert_eq(parser.options_->next_->next_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_MISSING_ARGUMENT_ERROR);
    cr_assert_eq(x, 52);
    cr_assert_eq(y, 42);
    cr_assert_eq(sum, -1);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 0);
}

Test(argparse, store_single_positional_int, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "54321"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 2, argv, "Storing a positional int", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 1);
    cr_assert_str_eq(parser.argv_[0], "54321");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    int value = 9999;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(INT, "value", &value, "an int with no option");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, &arg1);
    cr_assert_eq(parser.positional_args_->next_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(value, 54321);
    cr_assert_eq(arg1.count_, 1);
}

Test(argparse, store_multi_positional_int, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "54321", "123", "90"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 4, argv, "Storing multiple positional ints", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 3);
    cr_assert_str_eq(parser.argv_[0], "54321");
    cr_assert_str_eq(parser.argv_[1], "123");
    cr_assert_str_eq(parser.argv_[2], "90");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    int height = 9999, width = 9999, angle = 999;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(INT, "width", &width, "an int with no flag");
    argparse_arg_t arg2 =
        ARGPARSE_POSITIONAL(INT, "height", &height, "another int with no flag");
    argparse_arg_t arg3 =
        ARGPARSE_POSITIONAL(INT, "angle", &angle, "one more int with no flag");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, &arg1);
    cr_assert_eq(parser.positional_args_->next_, &arg2);
    cr_assert_eq(parser.positional_args_->next_->next_, &arg3);
    cr_assert_eq(parser.positional_args_->next_->next_->next_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(width, 54321);
    cr_assert_eq(height, 123);
    cr_assert_eq(angle, 90);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
}

Test(argparse, store_multi_positional_int_missing, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "54321", "123"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 3, argv, "Storing multiple positional ints", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 2);
    cr_assert_str_eq(parser.argv_[0], "54321");
    cr_assert_str_eq(parser.argv_[1], "123");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    int height = 9999, width = 9999, angle = 999;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(INT, "width", &width, "an int with no flag");
    argparse_arg_t arg2 =
        ARGPARSE_POSITIONAL(INT, "height", &height, "another int with no flag");
    argparse_arg_t arg3 =
        ARGPARSE_POSITIONAL(INT, "angle", &angle, "one more int with no flag");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, &arg1);
    cr_assert_eq(parser.positional_args_->next_, &arg2);
    cr_assert_eq(parser.positional_args_->next_->next_, &arg3);
    cr_assert_eq(parser.positional_args_->next_->next_->next_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_MISSING_ARGUMENT_ERROR);
    cr_assert_eq(width, 54321);
    cr_assert_eq(height, 123);
    cr_assert_eq(angle, 999);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 0);
}

Test(argparse, store_multi_positional_int_with_choices, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "54321", "123", "90"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 4, argv, "Storing multiple positional ints", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 3);
    cr_assert_str_eq(parser.argv_[0], "54321");
    cr_assert_str_eq(parser.argv_[1], "123");
    cr_assert_str_eq(parser.argv_[2], "90");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    int height = 9999, width = 9999, angle = 999;
    int angle_choices[] = {0, 90, 180, 270};
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(INT, "width", &width, "an int with no flag");
    argparse_arg_t arg2 =
        ARGPARSE_POSITIONAL(INT, "height", &height, "another int with no flag");
    argparse_arg_t arg3 = ARGPARSE_POSITIONAL_WITH_CHOICES(
        INT, "angle", &angle, "one more int with choices", angle_choices, 4);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, &arg1);
    cr_assert_eq(parser.positional_args_->next_, &arg2);
    cr_assert_eq(parser.positional_args_->next_->next_, &arg3);
    cr_assert_eq(parser.positional_args_->next_->next_->next_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(width, 54321);
    cr_assert_eq(height, 123);
    cr_assert_eq(angle, 90);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
}

Test(argparse, store_int_flags_and_positionals, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "54321", "123", "--angle", "90"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 5, argv, "Storing combination of flag and positional ints",
            NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 4);
    cr_assert_str_eq(parser.argv_[0], "54321");
    cr_assert_str_eq(parser.argv_[1], "123");
    cr_assert_str_eq(parser.argv_[2], "--angle");
    cr_assert_str_eq(parser.argv_[3], "90");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    int height = 9999, width = 9999, angle = 999;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(INT, "width", &width, "an int with no flag");
    argparse_arg_t arg2 =
        ARGPARSE_POSITIONAL(INT, "height", &height, "another int with no flag");
    argparse_arg_t arg3 =
        ARGPARSE_OPTION(INT, 'a', "--angle", &angle, "int with flag");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.positional_args_, &arg1);
    cr_assert_eq(parser.positional_args_->next_, &arg2);
    cr_assert_eq(parser.positional_args_->next_->next_, NULL);
    cr_assert_eq(parser.options_, &arg3);
    cr_assert_eq(parser.options_->next_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(width, 54321);
    cr_assert_eq(height, 123);
    cr_assert_eq(angle, 90);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
}

Test(argparse, store_int_flags_and_positionals_1, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--angle", "90", "54321", "123"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 5, argv,
            "Storing combination of flag and positional ints, different order",
            NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 4);
    cr_assert_str_eq(parser.argv_[0], "--angle");
    cr_assert_str_eq(parser.argv_[1], "90");
    cr_assert_str_eq(parser.argv_[2], "54321");
    cr_assert_str_eq(parser.argv_[3], "123");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    int height = 9999, width = 9999, angle = 999;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(INT, "width", &width, "an int with no flag");
    argparse_arg_t arg2 =
        ARGPARSE_POSITIONAL(INT, "height", &height, "another int with no flag");
    argparse_arg_t arg3 =
        ARGPARSE_OPTION(INT, 'a', "--angle", &angle, "int with flag");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.positional_args_, &arg1);
    cr_assert_eq(parser.positional_args_->next_, &arg2);
    cr_assert_eq(parser.positional_args_->next_->next_, NULL);
    cr_assert_eq(parser.options_, &arg3);
    cr_assert_eq(parser.options_->next_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(angle, 90);
    cr_assert_eq(width, 54321);
    cr_assert_eq(height, 123);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
}

Test(argparse, store_multi_int_flags_and_positionals, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "54321",       "123", "--angle",
                    "90",     "--thickness", "2"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 7, argv,
                     "Storing combination of multiple flag and positional ints",
                     NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 6);
    cr_assert_str_eq(parser.argv_[0], "54321");
    cr_assert_str_eq(parser.argv_[1], "123");
    cr_assert_str_eq(parser.argv_[2], "--angle");
    cr_assert_str_eq(parser.argv_[3], "90");
    cr_assert_str_eq(parser.argv_[4], "--thickness");
    cr_assert_str_eq(parser.argv_[5], "2");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    int height = 9999, width = 9999, angle = 999, thickness = -1;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(INT, "width", &width, "an int with no flag");
    argparse_arg_t arg2 =
        ARGPARSE_POSITIONAL(INT, "height", &height, "another int with no flag");
    argparse_arg_t arg3 =
        ARGPARSE_OPTION_REQUIRED(INT, 'a', "--angle", &angle, "int with flag");
    argparse_arg_t arg4 = ARGPARSE_OPTION(INT, 't', "--thickness", &thickness,
                                          "another int with flag");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg4)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.positional_args_, &arg1);
    cr_assert_eq(parser.positional_args_->next_, &arg2);
    cr_assert_eq(parser.positional_args_->next_->next_, NULL);
    cr_assert_eq(parser.options_, &arg3);
    cr_assert_eq(parser.options_->next_, &arg4);
    cr_assert_eq(parser.options_->next_->next_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(width, 54321);
    cr_assert_eq(height, 123);
    cr_assert_eq(angle, 90);
    cr_assert_eq(thickness, 2);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
    cr_assert_eq(arg4.count_, 1);
}

Test(argparse, store_multi_int_flags_and_positionals_1, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--angle", "90", "--thickness",
                    "2",      "54321",   "123"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 7, argv,
                     "Storing combination of multiple flag and positional ints",
                     NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 6);
    cr_assert_str_eq(parser.argv_[0], "--angle");
    cr_assert_str_eq(parser.argv_[1], "90");
    cr_assert_str_eq(parser.argv_[2], "--thickness");
    cr_assert_str_eq(parser.argv_[3], "2");
    cr_assert_str_eq(parser.argv_[4], "54321");
    cr_assert_str_eq(parser.argv_[5], "123");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    int height = 9999, width = 9999, angle = 999, thickness = -1;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(INT, "width", &width, "an int with no flag");
    argparse_arg_t arg2 =
        ARGPARSE_POSITIONAL(INT, "height", &height, "another int with no flag");
    argparse_arg_t arg3 =
        ARGPARSE_OPTION_REQUIRED(INT, 'a', "--angle", &angle, "int with flag");
    argparse_arg_t arg4 = ARGPARSE_OPTION(INT, 't', "--thickness", &thickness,
                                          "another int with flag");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg4)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.positional_args_, &arg1);
    cr_assert_eq(parser.positional_args_->next_, &arg2);
    cr_assert_eq(parser.positional_args_->next_->next_, NULL);
    cr_assert_eq(parser.options_, &arg3);
    cr_assert_eq(parser.options_->next_, &arg4);
    cr_assert_eq(parser.options_->next_->next_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(width, 54321);
    cr_assert_eq(height, 123);
    cr_assert_eq(angle, 90);
    cr_assert_eq(thickness, 2);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
    cr_assert_eq(arg4.count_, 1);
}

Test(argparse, store_multi_int_flags_and_positionals_2, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--thickness", "2", "54321",
                    "123",    "--angle",     "90"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 7, argv,
                     "Storing combination of multiple flag and positional ints",
                     NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 6);
    cr_assert_str_eq(parser.argv_[0], "--thickness");
    cr_assert_str_eq(parser.argv_[1], "2");
    cr_assert_str_eq(parser.argv_[2], "54321");
    cr_assert_str_eq(parser.argv_[3], "123");
    cr_assert_str_eq(parser.argv_[4], "--angle");
    cr_assert_str_eq(parser.argv_[5], "90");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    int height = 9999, width = 9999, angle = 999, thickness = -1;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(INT, "width", &width, "an int with no flag");
    argparse_arg_t arg2 =
        ARGPARSE_POSITIONAL(INT, "height", &height, "another int with no flag");
    argparse_arg_t arg3 =
        ARGPARSE_OPTION_REQUIRED(INT, 'a', "--angle", &angle, "int with flag");
    argparse_arg_t arg4 = ARGPARSE_OPTION(INT, 't', "--thickness", &thickness,
                                          "another int with flag");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg4)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.positional_args_, &arg1);
    cr_assert_eq(parser.positional_args_->next_, &arg2);
    cr_assert_eq(parser.positional_args_->next_->next_, NULL);
    cr_assert_eq(parser.options_, &arg3);
    cr_assert_eq(parser.options_->next_, &arg4);
    cr_assert_eq(parser.options_->next_->next_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(width, 54321);
    cr_assert_eq(height, 123);
    cr_assert_eq(angle, 90);
    cr_assert_eq(thickness, 2);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
    cr_assert_eq(arg4.count_, 1);
}

Test(argparse, store_multi_int_flags_and_positionals_3, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog",  "--thickness", "2",  "54321",
                    "--angle", "90",          "123"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 7, argv,
                     "Storing combination of multiple flag and positional ints",
                     NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 6);
    cr_assert_str_eq(parser.argv_[0], "--thickness");
    cr_assert_str_eq(parser.argv_[1], "2");
    cr_assert_str_eq(parser.argv_[2], "54321");
    cr_assert_str_eq(parser.argv_[3], "--angle");
    cr_assert_str_eq(parser.argv_[4], "90");
    cr_assert_str_eq(parser.argv_[5], "123");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    int height = 9999, width = 9999, angle = 999, thickness = -1;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(INT, "width", &width, "an int with no flag");
    argparse_arg_t arg2 =
        ARGPARSE_POSITIONAL(INT, "height", &height, "another int with no flag");
    argparse_arg_t arg3 =
        ARGPARSE_OPTION_REQUIRED(INT, 'a', "--angle", &angle, "int with flag");
    argparse_arg_t arg4 = ARGPARSE_OPTION(INT, 't', "--thickness", &thickness,
                                          "another int with flag");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg4)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.positional_args_, &arg1);
    cr_assert_eq(parser.positional_args_->next_, &arg2);
    cr_assert_eq(parser.positional_args_->next_->next_, NULL);
    cr_assert_eq(parser.options_, &arg3);
    cr_assert_eq(parser.options_->next_, &arg4);
    cr_assert_eq(parser.options_->next_->next_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(width, 54321);
    cr_assert_eq(height, 123);
    cr_assert_eq(angle, 90);
    cr_assert_eq(thickness, 2);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
    cr_assert_eq(arg4.count_, 1);
}

Test(argparse, store_single_int_with_choices, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--value", "52"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 3, argv, "Storing an int with choices", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 2);
    cr_assert_str_eq(parser.argv_[0], "--value");
    cr_assert_str_eq(parser.argv_[1], "52");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    int value = 9999;
    int choices[] = {50, 51, 52, 53, 54, 55};
    argparse_arg_t arg1 = ARGPARSE_OPTION_WITH_CHOICES(
        INT, 'v', "--value", &value, "a value", choices, 6);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(value, 52);
    cr_assert_eq(arg1.count_, 1);
}

Test(argparse, store_multi_int_with_choices, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "-x", "52", "-y", "50"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 5, argv, "Storing an int with choices", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 4);
    cr_assert_str_eq(parser.argv_[0], "-x");
    cr_assert_str_eq(parser.argv_[1], "52");
    cr_assert_str_eq(parser.argv_[2], "-y");
    cr_assert_str_eq(parser.argv_[3], "50");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    int x = 9999, y = 9999;
    int choices[] = {50, 51, 52, 53, 54, 55};
    argparse_arg_t arg1 = ARGPARSE_OPTION_WITH_CHOICES(INT, 'x', "--x", &x,
                                                       "a value", choices, 6);
    argparse_arg_t arg2 = ARGPARSE_OPTION_WITH_CHOICES(
        INT, 'y', "--y", &y, "another value", choices, 6);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, &arg2);
    cr_assert_eq(parser.options_->next_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(x, 52);
    cr_assert_eq(y, 50);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
}

Test(argparse, store_single_int_invalid_choice, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--value", "52"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 3, argv, "Storing an int with an invalid choice",
                     NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 2);
    cr_assert_str_eq(parser.argv_[0], "--value");
    cr_assert_str_eq(parser.argv_[1], "52");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    int value = 9999;
    int choices[] = {1, 2, 3};
    argparse_arg_t arg1 = ARGPARSE_OPTION_WITH_CHOICES(
        INT, 'v', "--value", &value, "a value", choices, 3);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_INVALID_CHOICE_ERROR);
}

// STORING FLOATS
Test(argparse, store_single_float, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--value", "3.14"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 3, argv, "Storing a float", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 2);
    cr_assert_str_eq(parser.argv_[0], "--value");
    cr_assert_str_eq(parser.argv_[1], "3.14");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    float value = 1.23;
    argparse_arg_t arg1 =
        ARGPARSE_OPTION(FLOAT, 'v', "--value", &value, "a value");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert(WITHIN(value, 3.14));
    cr_assert_eq(arg1.count_, 1);
}

Test(argparse, store_multi_float, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog",  "--value1", "3.14",       "--value2",
                    "2.71828", "--sum",    "5.858280000"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 7, argv, "storing multiple floats", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 6);
    cr_assert_str_eq(parser.argv_[0], "--value1");
    cr_assert_str_eq(parser.argv_[1], "3.14");
    cr_assert_str_eq(parser.argv_[2], "--value2");
    cr_assert_str_eq(parser.argv_[3], "2.71828");
    cr_assert_str_eq(parser.argv_[4], "--sum");
    cr_assert_str_eq(parser.argv_[5], "5.858280000");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    float x = 9999, y = 9999, sum = -1;
    argparse_arg_t arg1 =
        ARGPARSE_OPTION(FLOAT, 'x', "--value1", &x, "value to add");
    argparse_arg_t arg2 =
        ARGPARSE_OPTION(FLOAT, 'y', "--value2", &y, "another value to add");
    argparse_arg_t arg3 = ARGPARSE_OPTION(FLOAT, 's', "--sum", &sum, "sum");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, &arg2);
    cr_assert_eq(parser.options_->next_->next_, &arg3);
    cr_assert_eq(parser.options_->next_->next_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert(WITHIN(x, 3.14));
    cr_assert(WITHIN(y, 2.71828));
    cr_assert(WITHIN(sum, 5.858280000));
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
}

Test(argparse, store_multi_float_override, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--value1", "5.2",   "--value2", "4.2",
                    "--sum",  "9.4",      "--sum", "11.1"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 9, argv, "Storing multiple floats", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    float x = 9999, y = 9999, sum = -1;
    argparse_arg_t arg1 =
        ARGPARSE_OPTION(FLOAT, 'x', "--value1", &x, "value to add");
    argparse_arg_t arg2 =
        ARGPARSE_OPTION(FLOAT, 'y', "--value2", &y, "another value to add");
    argparse_arg_t arg3 = ARGPARSE_OPTION(FLOAT, 's', "--sum", &sum, "sum");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert(WITHIN(x, 5.2));
    cr_assert(WITHIN(y, 4.2));
    cr_assert(WITHIN(sum, 11.1));
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 2);
}

Test(argparse, store_float_missing_required, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "-x", "52", "-y", "42"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 5, argv, "Missing a required float", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 4);
    cr_assert_str_eq(parser.argv_[0], "-x");
    cr_assert_str_eq(parser.argv_[1], "52");
    cr_assert_str_eq(parser.argv_[2], "-y");
    cr_assert_str_eq(parser.argv_[3], "42");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    float x = 9999, y = 9999, sum = -1;
    argparse_arg_t arg1 =
        ARGPARSE_OPTION(FLOAT, 'x', "--value1", &x, "value to add");
    argparse_arg_t arg2 =
        ARGPARSE_OPTION(FLOAT, 'y', "--value2", &y, "another value to add");
    argparse_arg_t arg3 =
        ARGPARSE_OPTION_REQUIRED(FLOAT, 's', "--sum", &sum, "the sum");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_MISSING_ARGUMENT_ERROR);
    cr_assert(WITHIN(x, 52));
    cr_assert(WITHIN(y, 42));
    cr_assert(WITHIN(sum, -1));
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 0);
}

Test(argparse, store_single_positional_float, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "54.321"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 2, argv, "Storing a positional float", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    float value = 9999;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(FLOAT, "value", &value, "a float with no flag");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, &arg1);
    cr_assert_eq(parser.positional_args_->next_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert(WITHIN(value, 54.321));
    cr_assert_eq(arg1.count_, 1);
}

Test(argparse, store_multi_positional_float, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "54.321", "1.23", "0.0001"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 4, argv, "Storing multiple positional floats", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    float height = 9999, width = 9999, error = 999;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(FLOAT, "width", &width, "a float with no flag");
    argparse_arg_t arg2 = ARGPARSE_POSITIONAL(FLOAT, "height", &height,
                                              "another float with no flag");
    argparse_arg_t arg3 = ARGPARSE_POSITIONAL(FLOAT, "error", &error,
                                              "one more float with no flag");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert(WITHIN(width, 54.321));
    cr_assert(WITHIN(height, 1.23));
    cr_assert(WITHIN(error, 0.0001));
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
}

Test(argparse, store_multi_positional_float_with_choices, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "54321", "123", "78.9"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 4, argv, "Storing multiple positional ints", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    float height = 9999, width = 9999, angle = 999;
    float angle_choices[] = {12.3, 45.6, 78.9};
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(FLOAT, "width", &width, "a float with no flag");
    argparse_arg_t arg2 =
        ARGPARSE_POSITIONAL(FLOAT, "height", &height, "a float with no flag");
    argparse_arg_t arg3 = ARGPARSE_POSITIONAL_WITH_CHOICES(
        FLOAT, "angle", &angle, "one more float with choices", angle_choices,
        4);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert(WITHIN(width, 54321));
    cr_assert(WITHIN(height, 123));
    cr_assert(WITHIN(angle, 78.9));
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
}

Test(argparse, store_float_flags_and_positionals, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "54321.5", "123.75", "--angle", "90.0"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 5, argv,
            "Storing combination of flag and positional floats", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    float height = 9999.9, width = 9999.9, angle = 999.9;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(FLOAT, "width", &width, "a float with no flag");
    argparse_arg_t arg2 = ARGPARSE_POSITIONAL(FLOAT, "height", &height,
                                              "another float with no flag");
    argparse_arg_t arg3 =
        ARGPARSE_OPTION(FLOAT, 'a', "--angle", &angle, "float with flag");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert(WITHIN(width, 54321.5));
    cr_assert(WITHIN(height, 123.75));
    cr_assert(WITHIN(angle, 90.0));
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
}

Test(argparse, store_float_with_choices, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--value", "54.5"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 3, argv, "Storing a float with choices", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    float value = 9999.9;
    float choices[] = {50.0, 51.5, 53.0, 54.5, 56.0, 57.5};
    argparse_arg_t arg1 = ARGPARSE_OPTION_WITH_CHOICES(
        FLOAT, 'v', "--value", &value, "a value", choices, 6);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert(WITHIN(value, 54.5));
    cr_assert_eq(arg1.count_, 1);
}

Test(argparse, store_multi_float_with_choices, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "-x", "54.5", "-y", "50.0"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 5, argv, "Storing multiple floats with choices",
                     NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    float x = 9999, y = 9999;
    float choices[] = {50.0, 51.5, 53.0, 54.5, 56.0, 57.5};
    argparse_arg_t arg1 = ARGPARSE_OPTION_WITH_CHOICES(FLOAT, 'x', "--x", &x,
                                                       "a value", choices, 6);
    argparse_arg_t arg2 = ARGPARSE_OPTION_WITH_CHOICES(FLOAT, 'y', "--y", &y,
                                                       "a value", choices, 6);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert(WITHIN(x, 54.5));
    cr_assert(WITHIN(y, 50.0));
    cr_assert_eq(arg1.count_, 1);
}

Test(argparse, store_single_float_invalid_choice, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--value", "52.5"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 3, argv, "Storing an invalid float", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    float value = 9999;
    float choices[] = {51.0, 52.0, 53.0};
    argparse_arg_t arg1 = ARGPARSE_OPTION_WITH_CHOICES(
        FLOAT, 'v', "--value", &value, "a value", choices, 3);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_INVALID_CHOICE_ERROR);
}

// STORING STRINGS
Test(argparse, store_single_string, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--value", "52"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 3, argv, "Storing a string", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    const char *value;
    argparse_arg_t arg1 =
        ARGPARSE_OPTION(STRING, 'v', "--value", &value, "a value");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_str_eq(value, "52");
    cr_assert_eq(arg1.count_, 1);
}

Test(argparse, store_multi_string, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--value1", "52", "--value2",
                    "42",     "--sum",    "94"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 7, argv, "Storing multiple strings", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    const char *x, *y, *sum;
    argparse_arg_t arg1 =
        ARGPARSE_OPTION(STRING, 'x', "--value1", &x, "value to add");
    argparse_arg_t arg2 =
        ARGPARSE_OPTION(STRING, 'y', "--value2", &y, "another value to add");
    argparse_arg_t arg3 = ARGPARSE_OPTION(STRING, 's', "--sum", &sum, "sum");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_str_eq(x, "52");
    cr_assert_str_eq(y, "42");
    cr_assert_str_eq(sum, "94");
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
}

Test(argparse, store_multi_string_override, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--value1", "52",    "--value2", "42",
                    "--sum",  "94",       "--sum", "111"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 9, argv, "Storing multiple strings", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    const char *x, *y, *sum;
    argparse_arg_t arg1 =
        ARGPARSE_OPTION(STRING, 'x', "--value1", &x, "value to add");
    argparse_arg_t arg2 =
        ARGPARSE_OPTION(STRING, 'y', "--value2", &y, "another value to add");
    argparse_arg_t arg3 = ARGPARSE_OPTION(STRING, 's', "--sum", &sum, "sum");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_str_eq(x, "52");
    cr_assert_str_eq(y, "42");
    cr_assert_str_eq(sum, "111");
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 2);
}

Test(argparse, store_multi_string_with_optional, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "-x", "52", "-y", "42", "--sum", "94"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 7, argv, "Storing multiple strings", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 6);
    cr_assert_str_eq(parser.argv_[0], "-x");
    cr_assert_str_eq(parser.argv_[1], "52");
    cr_assert_str_eq(parser.argv_[2], "-y");
    cr_assert_str_eq(parser.argv_[3], "42");
    cr_assert_str_eq(parser.argv_[4], "--sum");
    cr_assert_str_eq(parser.argv_[5], "94");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    const char *x, *y, *z, *sum;
    argparse_arg_t arg1 =
        ARGPARSE_OPTION(STRING, 'x', "--value1", &x, "value to add");
    argparse_arg_t arg2 =
        ARGPARSE_OPTION(STRING, 'y', "--value2", &y, "another value to add");
    argparse_arg_t arg3 = ARGPARSE_OPTION(STRING, 'z', "--value3", &z,
                                          "yet another value to add");
    argparse_arg_t arg4 =
        ARGPARSE_OPTION(STRING, 's', "--sum", &sum, "the sum");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg4)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_str_eq(x, "52");
    cr_assert_str_eq(y, "42");
    cr_assert_str_eq(sum, "94");
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 0);
    cr_assert_eq(arg4.count_, 1);
}

Test(argparse, store_string_missing_required, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "-x", "52", "-y", "42"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 5, argv, "Missing a required string", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    const char *x, *y, *sum;
    argparse_arg_t arg1 =
        ARGPARSE_OPTION(STRING, 'x', "--value1", &x, "value to add");
    argparse_arg_t arg2 =
        ARGPARSE_OPTION(STRING, 'y', "--value2", &y, "another value to add");
    argparse_arg_t arg3 =
        ARGPARSE_OPTION_REQUIRED(STRING, 's', "--sum", &sum, "the sum");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_MISSING_ARGUMENT_ERROR);
    cr_assert_str_eq(x, "52");
    cr_assert_str_eq(y, "42");
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 0);
}

Test(argparse, store_single_positional_string, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "54321"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 2, argv, "Storing a positional string", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    const char *value;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(STRING, "value", &value, "a string with no flag");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_str_eq(value, "54321");
    cr_assert_eq(arg1.count_, 1);
}

Test(argparse, store_multi_positional_string, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "54321", "123", "90"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 4, argv, "Storing multiple positional strings",
                     NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    const char *width, *height, *angle;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(STRING, "width", &width, "a string with no flag");
    argparse_arg_t arg2 = ARGPARSE_POSITIONAL(STRING, "height", &height,
                                              "another string with no flag");
    argparse_arg_t arg3 = ARGPARSE_POSITIONAL(STRING, "angle", &angle,
                                              "one more string with no flag");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_str_eq(width, "54321");
    cr_assert_str_eq(height, "123");
    cr_assert_str_eq(angle, "90");
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
}

Test(argparse, store_multi_positional_string_with_choices, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "54321", "123", "90"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 4, argv, "Storing multiple positional strings",
                     NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    const char *width, *height, *angle;
    const char *angle_choices[] = {"0", "90", "180", "270"};
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(STRING, "width", &width, "a string with no flag");
    argparse_arg_t arg2 = ARGPARSE_POSITIONAL(STRING, "height", &height,
                                              "another string with no flag");
    argparse_arg_t arg3 = ARGPARSE_POSITIONAL_WITH_CHOICES(
        STRING, "angle", &angle, "one more string with choices", angle_choices,
        4);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_str_eq(width, "54321");
    cr_assert_str_eq(height, "123");
    cr_assert_str_eq(angle, "90");
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
}

Test(argparse, store_string_flags_and_positionals, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "54321", "123", "--angle", "90"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 5, argv,
            "Storing combination of flag and positional strings", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    const char *width, *height, *angle;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(STRING, "width", &width, "a string with no flag");
    argparse_arg_t arg2 = ARGPARSE_POSITIONAL(STRING, "height", &height,
                                              "another string with no flag");
    argparse_arg_t arg3 =
        ARGPARSE_OPTION(STRING, 'a', "--angle", &angle, "string with flag");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_str_eq(width, "54321");
    cr_assert_str_eq(height, "123");
    cr_assert_str_eq(angle, "90");
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
}

Test(argparse, store_multi_string_flags_and_positionals, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "54321",       "123", "--angle",
                    "90",     "--thickness", "2"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 7, argv,
            "Storing combination of multiple flag and positional strings",
            NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    const char *height, *width, *angle, *thickness;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(STRING, "width", &width, "a string with no flag");
    argparse_arg_t arg2 = ARGPARSE_POSITIONAL(STRING, "height", &height,
                                              "another string with no flag");
    argparse_arg_t arg3 = ARGPARSE_OPTION_REQUIRED(STRING, 'a', "--angle",
                                                   &angle, "string with flag");
    argparse_arg_t arg4 = ARGPARSE_OPTION(STRING, 't', "--thickness",
                                          &thickness, "another int with flag");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg4)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_str_eq(width, "54321");
    cr_assert_str_eq(height, "123");
    cr_assert_str_eq(angle, "90");
    cr_assert_str_eq(thickness, "2");
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
    cr_assert_eq(arg4.count_, 1);
}

Test(argparse, store_single_string_with_choices, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--value", "52"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 3, argv, "Storing a string with choices", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    const char *value;
    const char *choices[] = {"50", "51", "52", "53", "54", "55"};
    argparse_arg_t arg1 = ARGPARSE_OPTION_WITH_CHOICES(
        STRING, 'v', "--value", &value, "a value", choices, 6);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_str_eq(value, "52");
    cr_assert_eq(arg1.count_, 1);
}

Test(argparse, store_multi_string_with_choices, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "-x", "52", "-y", "50"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 5, argv, "Storing a string with choices", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    const char *x, *y;
    const char *choices[] = {"50", "51", "52", "53", "54", "55"};
    argparse_arg_t arg1 = ARGPARSE_OPTION_WITH_CHOICES(STRING, 'x', "--x", &x,
                                                       "a value", choices, 6);
    argparse_arg_t arg2 = ARGPARSE_OPTION_WITH_CHOICES(
        STRING, 'y', "--y", &y, "another value", choices, 6);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_str_eq(x, "52");
    cr_assert_str_eq(y, "50");
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
}

Test(argparse, store_single_string_invalid_choice, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--value", "52"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 3, argv,
                     "Storing a string with an invalid choice", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 2);
    cr_assert_str_eq(parser.argv_[0], "--value");
    cr_assert_str_eq(parser.argv_[1], "52");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    const char *value;
    const char *choices[] = {"1", "2", "3", "4", "5"};
    argparse_arg_t arg1 = ARGPARSE_OPTION_WITH_CHOICES(
        STRING, 'v', "--value", &value, "a value", choices, 5);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_INVALID_CHOICE_ERROR);
}

// SPECIAL ACTIONS
Test(argparse, flag_true, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--verbose"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 2, argv, "Storing a store true flag", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 1);
    cr_assert_str_eq(parser.argv_[0], "--verbose");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    bool verbose = false;
    argparse_arg_t arg1 =
        ARGPARSE_FLAG_TRUE('v', "--verbose", &verbose, "add more prints");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(verbose, true);
    cr_assert_eq(arg1.count_, 1);
}

Test(argparse, flag_true_repeat, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--verbose", "--verbose"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 3, argv, "Storing a store true flag twice", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 2);
    cr_assert_str_eq(parser.argv_[0], "--verbose");
    cr_assert_str_eq(parser.argv_[1], "--verbose");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    bool verbose = false;
    argparse_arg_t arg1 =
        ARGPARSE_FLAG_TRUE('v', "--verbose", &verbose, "add more prints");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(verbose, true);
    cr_assert_eq(arg1.count_, 2);
}

Test(argparse, flag_false, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--disable-logging"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 2, argv, "Storing a store false flag", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 1);
    cr_assert_str_eq(parser.argv_[0], "--disable-logging");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    bool logging;
    argparse_arg_t arg1 =
        ARGPARSE_FLAG_FALSE('l', "--disable-logging", &logging, "no prints");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(logging, false);
    cr_assert_eq(arg1.count_, 1);
}

Test(argparse, flag_false_override, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--disable-logging", "--enable-logging"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 3, argv, "Store true and false flags", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 2);
    cr_assert_str_eq(parser.argv_[0], "--disable-logging");
    cr_assert_str_eq(parser.argv_[1], "--enable-logging");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    bool logging;
    argparse_arg_t arg1 =
        ARGPARSE_FLAG_FALSE('d', "--disable-logging", &logging, "no prints");
    argparse_arg_t arg2 =
        ARGPARSE_FLAG_TRUE('e', "--enable-logging", &logging, "yes prints");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, &arg2);
    cr_assert_eq(parser.options_->next_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(logging, true);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
}

Test(argparse, toggle_on, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--verbose"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 2, argv, "Toggling verbose", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 1);
    cr_assert_str_eq(parser.argv_[0], "--verbose");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    bool verbose = false;
    argparse_arg_t arg1 =
        ARGPARSE_TOGGLE('v', "--verbose", &verbose, "verbose");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(verbose, true);
    cr_assert_eq(arg1.count_, 1);
}

Test(argparse, toggle_off, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--no-verbose"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 2, argv, "Toggling verbose", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 1);
    cr_assert_str_eq(parser.argv_[0], "--no-verbose");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    bool verbose = true;
    argparse_arg_t arg1 =
        ARGPARSE_TOGGLE('v', "--verbose", &verbose, "verbose");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(verbose, false);
    cr_assert_eq(arg1.count_, 1);
}

Test(argparse, toggle_with_unset, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--verbose"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 2, argv, "Toggling verbose", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 1);
    cr_assert_str_eq(parser.argv_[0], "--verbose");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    bool verbose, in_place = false;
    argparse_arg_t arg1 =
        ARGPARSE_TOGGLE('v', "--verbose", &verbose, "verbose");
    argparse_arg_t arg2 =
        ARGPARSE_TOGGLE('p', "--in-place", &in_place, "make changes in place");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, &arg2);
    cr_assert_eq(parser.options_->next_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(verbose, true);
    cr_assert_eq(in_place, false);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 0);
}

Test(argparse, toggle_on_off, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--verbose", "--no-verbose"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 3, argv, "Toggling verbose on and off", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 2);
    cr_assert_str_eq(parser.argv_[0], "--verbose");
    cr_assert_str_eq(parser.argv_[1], "--no-verbose");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    bool verbose = true;
    argparse_arg_t arg1 =
        ARGPARSE_TOGGLE('v', "--verbose", &verbose, "verbose");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(verbose, false);
    cr_assert_eq(arg1.count_, 2);
}

Test(argparse, toggle_on_off_on, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--verbose", "--no-verbose", "--verbose"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 4, argv,
                     "Toggling verbose on and off and on again", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.argc_, 3);
    cr_assert_str_eq(parser.argv_[0], "--verbose");
    cr_assert_str_eq(parser.argv_[1], "--no-verbose");
    cr_assert_str_eq(parser.argv_[2], "--verbose");
    cr_assert_eq(parser.options_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    bool verbose = true;
    argparse_arg_t arg1 =
        ARGPARSE_TOGGLE('v', "--verbose", &verbose, "verbose");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(parser.options_, &arg1);
    cr_assert_eq(parser.options_->next_, NULL);
    cr_assert_eq(parser.positional_args_, NULL);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(verbose, true);
    cr_assert_eq(arg1.count_, 3);
}

Test(argparse, count_single_long, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--verbose"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 2, argv, "Verbosity level 1", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    int verbosity = -1000;
    argparse_arg_t arg1 =
        ARGPARSE_COUNT('v', "--verbose", &verbosity, "verbose");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(verbosity, 1);
    cr_assert_eq(arg1.count_, 1);
}

Test(argparse, count_multi_long, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog",    "--verbose", "--verbose",
                    "--verbose", "--verbose", "--verbose"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 6, argv, "Verbosity level 5", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    int verbosity = -1000;
    argparse_arg_t arg1 =
        ARGPARSE_COUNT('v', "--verbose", &verbosity, "verbose");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(verbosity, 5);
    cr_assert_eq(arg1.count_, 5);
}

Test(argparse, count_single_short, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "-v"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 2, argv, "Verbosity level 1 short", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    int verbosity = -1000;
    argparse_arg_t arg1 =
        ARGPARSE_COUNT('v', "--verbose", &verbosity, "verbose");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(verbosity, 1);
    cr_assert_eq(arg1.count_, 1);
}

Test(argparse, count_multi_short, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "-v", "-v", "-v", "-v", "-v"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 6, argv, "Verbosity level 5 short", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    int verbosity = -1000;
    argparse_arg_t arg1 =
        ARGPARSE_COUNT('v', "--verbose", &verbosity, "verbose");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(verbosity, 5);
    cr_assert_eq(arg1.count_, 5);
}

Test(argparse, count_multi_mixed, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog",    "--verbose", "-v",
                    "--verbose", "-v",        "--verbose"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 6, argv, "Verbosity level 5 mixed", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    int verbosity = -1000;
    argparse_arg_t arg1 =
        ARGPARSE_COUNT('v', "--verbose", &verbosity, "verbose");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(verbosity, 5);
    cr_assert_eq(arg1.count_, 5);
}

Test(argparse, count_single_grouped, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "-vvv"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 2, argv, "Verbosity level 3 grouped", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    int verbosity = -1000;
    argparse_arg_t arg1 =
        ARGPARSE_COUNT('v', "--verbose", &verbosity, "verbose");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(verbosity, 3);
    cr_assert_eq(arg1.count_, 3);
}

Test(argparse, count_multi_grouped, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "-vvv", "-vvvvv"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 3, argv, "Verbosity level 8 grouped and mixed",
                     NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    int verbosity = -1000;
    argparse_arg_t arg1 =
        ARGPARSE_COUNT('v', "--verbose", &verbosity, "verbose");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(verbosity, 8);
    cr_assert_eq(arg1.count_, 8);
}

Test(argparse, count_multi_grouped_diff, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "-ab"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 2, argv, "Verbosity group different", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    bool a = false, b = false;
    argparse_arg_t arg1 = ARGPARSE_TOGGLE('a', "--apple", &a, "apple");
    argparse_arg_t arg2 = ARGPARSE_TOGGLE('b', "--banana", &b, "banana");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(a, true);
    cr_assert_eq(b, true);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
}

Test(argparse, count_multi_grouped_diff_option_0, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "-ab", "3"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 3, argv, "Verbosity group two different", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    bool a = false;
    int b = 999;
    argparse_arg_t arg1 = ARGPARSE_TOGGLE('a', "--apple", &a, "apple");
    argparse_arg_t arg2 = ARGPARSE_OPTION(INT, 'b', "--banana", &b, "banana");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(a, true);
    cr_assert_eq(b, 3);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
}

Test(argparse, count_multi_grouped_diff_option_1, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "-ab", "3", "--carrot", "321"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 5, argv, "Verbosity group two again", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    bool a = false;
    int b = 999, c = 999;
    argparse_arg_t arg1 = ARGPARSE_TOGGLE('a', "--apple", &a, "apple");
    argparse_arg_t arg2 = ARGPARSE_OPTION(INT, 'b', "--banana", &b, "banana");
    argparse_arg_t arg3 = ARGPARSE_OPTION(INT, 'c', "--carrot", &c, "carrot");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(a, true);
    cr_assert_eq(b, 3);
    cr_assert_eq(c, 321);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
}

Test(argparse, count_multi_grouped_with_ungrouped, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "-vvv", "-v", "--verbose"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 4, argv,
                     "Verbosity level 5 grouped and ungrouped", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    int verbosity = -1000;
    argparse_arg_t arg1 =
        ARGPARSE_COUNT('v', "--verbose", &verbosity, "verbose");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(verbosity, 5);
    cr_assert_eq(arg1.count_, 5);
}

// MIXED
Test(argparse, store_mixed_0, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--value", "42", "--name", "Alice"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 5, argv, "Store int and string", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    int value;
    const char *name;
    argparse_arg_t arg1 =
        ARGPARSE_OPTION(INT, 'v', "--value", &value, "an integer value");
    argparse_arg_t arg2 =
        ARGPARSE_OPTION(STRING, 'n', "--name", &name, "a string value");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(value, 42);
    cr_assert_str_eq(name, "Alice");
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
}

Test(argparse, store_mixed_1, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "123", "--verbose"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 3, argv, "Store positional and flag", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    const char *positional;
    bool verbose = false;
    argparse_arg_t arg1 = ARGPARSE_POSITIONAL(STRING, "positional", &positional,
                                              "a positional argument");
    argparse_arg_t arg2 =
        ARGPARSE_FLAG_TRUE('v', "--verbose", &verbose, "enable verbose mode");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_str_eq(positional, "123");
    cr_assert_eq(verbose, true);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
}

Test(argparse, store_mixed_2, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--level", "3", "--debug"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 4, argv, "Store int and toggle", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    int level;
    bool debug = false;
    argparse_arg_t arg1 =
        ARGPARSE_TOGGLE('d', "--debug", &debug, "toggle debug mode");
    argparse_arg_t arg2 =
        ARGPARSE_OPTION(INT, 'l', "--level", &level, "set level");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(level, 3);
    cr_assert_eq(debug, true);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
}

Test(argparse, store_mixed_3, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--username", "admin", "--password", "secret"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 5, argv, "Store required and optional", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    const char *username, *password = NULL;
    argparse_arg_t arg1 = ARGPARSE_OPTION_REQUIRED(
        STRING, 'u', "--username", &username, "username (required)");
    argparse_arg_t arg2 =
        ARGPARSE_OPTION(STRING, 'p', "--password", &password, "password");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_str_eq(username, "admin");
    cr_assert_str_eq(password, "secret");
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
}

Test(argparse, store_mixed_4, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "Alice", "42"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 3, argv, "Positional string and int", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    const char *name;
    int age;
    argparse_arg_t arg1 = ARGPARSE_POSITIONAL(STRING, "name", &name, "a name");
    argparse_arg_t arg2 = ARGPARSE_POSITIONAL(INT, "age", &age, "an age");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_str_eq(name, "Alice");
    cr_assert_eq(age, 42);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
}

Test(argparse, store_mixed_5, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "radius", "3.14"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 3, argv, "Positional float and string", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    const char *label;
    float pi;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(STRING, "label", &label, "a string label");
    argparse_arg_t arg2 =
        ARGPARSE_POSITIONAL(FLOAT, "pi", &pi, "a float value");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_str_eq(label, "radius");
    cr_assert(WITHIN(pi, 3.14));
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
}

Test(argparse, store_mixed_6, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "true", "87.9", "sample"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 4, argv, "Positional mixed types", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    float percentage;
    const char *is_valid, *text;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(STRING, "valid", &is_valid, "boolean value");
    argparse_arg_t arg2 =
        ARGPARSE_POSITIONAL(FLOAT, "percent", &percentage, "float value");
    argparse_arg_t arg3 =
        ARGPARSE_POSITIONAL(STRING, "label", &text, "text label");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_str_eq(is_valid, "true");
    cr_assert_float_eq(percentage, 87.9, 0.001);
    cr_assert_str_eq(text, "sample");
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
}

Test(argparse, store_mixed_7, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "alpha", "99", "--enable"};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 4, argv, "Positional with optional flag", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    const char *label;
    int score;
    bool enable = false;
    argparse_arg_t arg1 =
        ARGPARSE_POSITIONAL(STRING, "label", &label, "string label");
    argparse_arg_t arg2 =
        ARGPARSE_POSITIONAL(INT, "score", &score, "integer score");
    argparse_arg_t arg3 =
        ARGPARSE_FLAG_TRUE('e', "--enable", &enable, "enable flag");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg3)),
                 ARGPARSE_NO_ERROR);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_str_eq(label, "alpha");
    cr_assert_eq(score, 99);
    cr_assert_eq(enable, true);
    cr_assert_eq(arg1.count_, 1);
    cr_assert_eq(arg2.count_, 1);
    cr_assert_eq(arg3.count_, 1);
}

// ERRORS
Test(errors, invalid_flag, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", ""};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 2, argv, "Positional with optional flag", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    int value;
    argparse_arg_t arg1 = ARGPARSE_OPTION(INT, 0, "", &value, "empty");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_INVALID_FLAG_OR_NAME_ERROR);
}

Test(errors, invalid_name, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", ""};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 2, argv, "positional with optional flag", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    int value;
    argparse_arg_t arg1 = ARGPARSE_OPTION(INT, 0, "--", &value, "empty");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_INVALID_FLAG_OR_NAME_ERROR);
}

Test(errors, unsupported_choices, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", ""};
    cr_assert_eq(
        argparse_check_error(argparse_init(
            &parser, 2, argv, "Positional with optional flag", NO_EPILOG)),
        ARGPARSE_NO_ERROR);

    int value;
    bool choices[] = {true, false};
    argparse_arg_t arg1 = ARGPARSE_TOGGLE('i', "--igloo", &value, "igloo");
    arg1.choices_ = choices;
    arg1.num_choices_ = 2;
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_UNSUPPORTED_CHOICES_ERROR);

    arg1.action_ = ARGPARSE_STORE_TRUE_ACTION;
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_UNSUPPORTED_CHOICES_ERROR);

    arg1.action_ = ARGPARSE_STORE_FALSE_ACTION;
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_UNSUPPORTED_CHOICES_ERROR);

    argparse_arg_t arg2 = ARGPARSE_COUNT('i', "--igloo", &value, "igloo");
    arg2.choices_ = choices;
    arg2.num_choices_ = 2;
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_UNSUPPORTED_CHOICES_ERROR);
}

Test(errors, conflicting_options_0, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", ""};
    cr_assert_eq(argparse_check_error(argparse_init(&parser, 2, argv,
                                                    NO_DESCRIPTION, NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    int value;
    argparse_arg_t arg1 =
        ARGPARSE_OPTION(INT, 'v', "--value1", &value, "empty");
    argparse_arg_t arg2 =
        ARGPARSE_OPTION(INT, 'v', "--value2", &value, "empty");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_CONFLICTING_OPTIONS_ERROR);
}

Test(errors, conflicting_options_1, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", ""};
    cr_assert_eq(argparse_check_error(argparse_init(&parser, 2, argv,
                                                    NO_DESCRIPTION, NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    int value;
    argparse_arg_t arg1 = ARGPARSE_OPTION(INT, 'x', "--value", &value, "empty");
    argparse_arg_t arg2 = ARGPARSE_OPTION(INT, 'y', "--value", &value, "empty");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg2)),
                 ARGPARSE_CONFLICTING_OPTIONS_ERROR);
}

Test(errors, conflicting_options_2, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", ""};
    cr_assert_eq(argparse_check_error(argparse_init(&parser, 2, argv,
                                                    NO_DESCRIPTION, NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    int value;
    argparse_arg_t arg1 = ARGPARSE_OPTION(INT, 'h', "--hello", &value, "empty");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_CONFLICTING_OPTIONS_ERROR);
}

Test(errors, conflicting_options_3, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", ""};
    cr_assert_eq(argparse_check_error(argparse_init(&parser, 1, argv,
                                                    NO_DESCRIPTION, NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    int value;
    argparse_arg_t arg1 = ARGPARSE_OPTION(INT, 'a', "--help", &value, "empty");
    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_CONFLICTING_OPTIONS_ERROR);
}

Test(errors, unknown_argument_0, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--verbose"};
    cr_assert_eq(argparse_check_error(argparse_init(&parser, 2, argv,
                                                    NO_DESCRIPTION, NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    bool verbose;
    argparse_arg_t arg1 =
        ARGPARSE_TOGGLE('v', "--verbose", &verbose, "verbose");

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_ARGUMENT_UNKNOWN_ERROR);

    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
}

Test(errors, unknown_argument_1, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--verbos"};
    cr_assert_eq(argparse_check_error(argparse_init(&parser, 2, argv,
                                                    NO_DESCRIPTION, NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    bool verbose;
    argparse_arg_t arg1 =
        ARGPARSE_TOGGLE('v', "--verbose", &verbose, "verbose");

    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_ARGUMENT_UNKNOWN_ERROR);
}

Test(errors, missing_value, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--value"};
    cr_assert_eq(argparse_check_error(argparse_init(&parser, 2, argv,
                                                    NO_DESCRIPTION, NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    int value;
    argparse_arg_t arg1 = ARGPARSE_OPTION(INT, 'v', "--value", &value, "value");

    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_ARGUMENT_MISSING_VALUE_ERROR);
}

Test(errors, range_exceeded, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "--value", "12345678901"};
    cr_assert_eq(argparse_check_error(argparse_init(&parser, 3, argv,
                                                    NO_DESCRIPTION, NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    int value;
    argparse_arg_t arg1 = ARGPARSE_OPTION(INT, 'v', "--value", &value, "value");

    cr_assert_eq(argparse_check_error(argparse_add_argument(&parser, &arg1)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_INT_RANGE_EXCEEDED_ERROR);
}

Test(argparse, add_arguments, .init = newlines) {
    argument_parser_t parser;

    char *argv[] = {"./prog", "-x", "52", "-y", "42", "--sum", "94"};
    cr_assert_eq(argparse_check_error(argparse_init(
                     &parser, 7, argv, "Storing multiple ints", NO_EPILOG)),
                 ARGPARSE_NO_ERROR);

    int x = 9999, y = 9999, z = 23, sum = -1;

    argparse_arg_t args[] = {
        ARGPARSE_OPTION(INT, 'x', "--value1", &x, "value to add"),
        ARGPARSE_OPTION(INT, 'y', "--value2", &y, "another value to add"),
        ARGPARSE_OPTION(INT, 'z', "--value3", &z, "yet another value to add"),
        ARGPARSE_OPTION(INT, 's', "--sum", &sum, "the sum")
    };
    cr_assert_eq(argparse_check_error(argparse_add_arguments(&parser, args, 4)),
                 ARGPARSE_NO_ERROR);

    if (TESTS_PRINT_HELP)
        argparse_print_help(&parser);

    cr_assert_eq(argparse_check_error(argparse_parse_args(&parser)),
                 ARGPARSE_NO_ERROR);
    cr_assert_eq(x, 52);
    cr_assert_eq(y, 42);
    cr_assert_eq(z, 23);
    cr_assert_eq(sum, 94);
    cr_assert_eq(args[0].count_, 1);
    cr_assert_eq(args[1].count_, 1);
    cr_assert_eq(args[2].count_, 0);
    cr_assert_eq(args[3].count_, 1);
}