# argparse

### Overview
`argparse` is a simple and flexible C library for parsing command-line options and arguments. Largely inspired by Python's `argparse` module, it simplifies the process of handling command-line inputs by providing macros and functions to define, parse and validate arguments with ease.

### Features
- Handles integer, float, boolean and string argument types
- Support for positional arguments and options
- Supports flag-based, long-form and grouped short options
- Provides easy-to-use macros for defining arguments
- Built-in `-h`/`--help` options that automatically generates and prints a customisable usage message
- Ability to enforce required arguments
- Enables predefined choices for arguments to restrict values to a specified set
- Validates arguments and returns errors for invalid arguments
- Compatible with C++

### Usage

#### Initialising parser
The `argparse` library is built around the `argument_parser_t` structure, which serves as a container for argument specifications and manages parsing arguments and options:
```
argument_parser_t parser;
argparse_init(&parser, argc, argv, "Program description", "Epilog text");
```

#### Creating arguments
`argparse` provides a set of convenient macros to simplify the creation of arguments. These macros define positional arguments, options that accept values, boolean flags and count actions:
```
int verbosity;
float distance;
const char *route_file;
bool reroute;

argparse_arg_t arg1 = ARGPARSE_COUNT(
    'v', "--verbose", &verbosity, "verbosity level"
);  // count action

argparse_arg_t arg2 = ARGPARSE_OPTION(
    FLOAT, 'd', "--distance", &distance, "maximum distance for the trip"
);  // option that takes float value

argparse_arg_t arg3 = ARGPARSE_TOGGLE(
    'r', "--reroute", &reroute, "whether the trip should be rerouted"
);  // boolean toggle

argparse_arg_t arg4 = ARGPARSE_POSITIONAL(
    STRING, "--file", &route_file, "name of file containing routes"
);  // positional argument
```

#### Adding arguments
After the arguments are created, the `argparse_add_argument` function adds individual argument specifications to the parser:

```
argparse_add_argument(&parser, &arg1);
argparse_add_argument(&parser, &arg2);
argparse_add_argument(&parser, &arg3);
argparse_add_argument(&parser, &arg4);
```

The `argparse_add_arguments` function can be used to combine the addition of multiple arguments:
```
argparse_arg_t args[] = {
    ARGPARSE_COUNT('v', "--verbose", &verbosity, "verbosity level"),
    ARGPARSE_OPTION(FLOAT, 'd', "--distance", &distance, "maximum distance for the trip"),
    ARGPARSE_TOGGLE('r', "--reroute", &reroute, "whether the trip should be rerouted"),
    ARGPARSE_POSITIONAL(STRING, "--file", &route_file, "name of file containing routes")
};
argparse_add_arguments(&parser, args, 4);
```

#### Parsing arguments
The `argparse_parse_args` function runs the parser and updates the respective variables with the extracted data:
```
argparse_parse_args(&parser);
```

#### Displaying usage message
The `-h` and `--help` options are built-in; when either option is encountered during parsing, a usage message is automatically generated and printed. This usage message provides information on all the defined options and positional arguments, their flags and names, a brief user-provided description, whether the argument is required, and the valid choices for the argument. The `argparse_print_help` function also displays the usage message without needing to parse any `-h` or `--help` arguments:
```
argparse_print_help(&parser);
```

#### Running tests
The implementation tests are written using the Criterion library. In the `tests/` directory, the tests can be run using `make test_argparse && ./test_argparse`.