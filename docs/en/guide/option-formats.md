# Supported Option Formats

argus supports multiple option formats to provide maximum flexibility for end users. This flexibility allows your program to handle different command-line conventions and accommodate various user preferences.

## Standard Options

!!! abstract "Overview"
    argus recognizes several standard formats for command-line options, including GNU-style long options, POSIX-style short options, and various combinations.

### Long Options

For options with a long name (`--option`):

=== "Space Format"
    ```bash
    --option value
    ```

=== "Equal Format"
    ```bash
    --option=value
    ```

!!! example "Example"
    ```bash
    # Both formats are equivalent
    my_program --output file.txt
    my_program --output=file.txt
    ```

### Short Options

For options with a short name (`-o`):

=== "Space Format"
    ```bash
    -o value
    ```

=== "No-space Format"
    ```bash
    -ovalue
    ```

=== "Combined Flags"
    ```bash
    -abc    # Equivalent to -a -b -c
    ```

=== "Combined with Value"
    ```bash
    -abcvalue    # Equivalent to -a -b -c value
    ```

!!! example "Example"
    ```bash
    # All formats are equivalent
    my_program -o file.txt
    my_program -ofile.txt
    
    # Flag combinations
    my_program -vxz       # Enables -v, -x, and -z options
    my_program -vxooutput.txt  # Enables -v and -x, then sets -o to "output.txt"
    ```

### Option Terminator (`--`)

!!! tip
    The double dash `--` is recognized as a terminator indicating the end of options. Everything that follows is treated as positional arguments, even if they start with `-` or `--`.

```bash
my_program --output=file.txt -- --file-with-dashes.txt
```

In this example, `--file-with-dashes.txt` is treated as a positional argument, not as an option.

## Multi-Value Collections

argus supports collection options that can hold multiple values (arrays) or key-value pairs (maps).

### Array Option Formats

For array options like `OPTION_ARRAY_STRING`, `OPTION_ARRAY_INT`, etc.:

=== "Multiple Option Occurrences"
    ```bash
    # Each option occurrence adds to the array
    ./program --names=john --names=alice --names=bob
    ```

=== "Comma-Separated Lists"
    ```bash
    # Comma-separated values
    ./program --names=john,alice,bob
    ```

=== "Combined Approaches"
    ```bash
    # Combined approaches
    ./program --names=john,alice --names=bob
    ```

=== "Short Option Format"
    ```bash
    # Short option variants
    ./program -n john -n alice -n bob
    ./program -njohn,alice,bob
    ```

!!! tip "Integer Ranges"
    Integer array options support a convenient range syntax:
    
    ```bash
    ./program --ids=1-5,10,15-20
    # Equivalent to: --ids=1,2,3,4,5,10,15,16,17,18,19,20
    ```

### Map Option Formats

For map options like `OPTION_MAP_STRING`, `OPTION_MAP_INT`, etc.:

=== "Multiple Option Occurrences"
    ```bash
    ./program --env=USER=alice --env=HOME=/home/alice
    ```

=== "Comma-Separated Key-Value Pairs"
    ```bash
    ./program --env=USER=alice,HOME=/home/alice,TERM=xterm
    ```

=== "Combined Approaches"
    ```bash
    ./program --env=USER=alice,HOME=/home/alice --env=TERM=xterm
    ```

=== "Short Option Format"
    ```bash
    ./program -e USER=alice -e HOME=/home/alice
    ./program -eUSER=alice,HOME=/home/alice
    ```

!!! tip "Boolean Maps"
    For boolean maps (`OPTION_MAP_BOOL`), values are parsed as booleans:
    
    - **True values**: "true", "yes", "1", "on", "y" (case-insensitive)
    - **False values**: "false", "no", "0", "off", "n" (case-insensitive)
    
    ```bash
    ./program --features=debug=true,logging=yes,metrics=1,cache=false
    ```

## Positional Arguments

Positional arguments are processed in the order they appear on the command line, after matching all options:

```bash
my_program file1.txt file2.txt
```

In this example, `file1.txt` and `file2.txt` are positional arguments.

### Required vs Optional Positionals

argus distinguishes between required and optional positional arguments:

```bash
my_program input.txt [output.txt]  # input.txt required, output.txt optional
```

!!! warning
    Required positional arguments must always be defined before optional ones in your `ARGUS_OPTIONS` definition.

### Positional Arguments with Options

Positional arguments can be used along with options in any order:

```bash
my_program --verbose input.txt --output=output.txt
my_program input.txt --verbose --output=output.txt
```

Both commands are equivalent, as argus identifies positional arguments after matching all options.

## Subcommands

argus supports Git/Docker-style subcommands, allowing for complex command hierarchies:

### Basic Subcommand Format

```bash
my_program [global options] command [command options] [arguments]
```

Examples:
```bash
my_program --verbose add --force file.txt
my_program add --force file.txt --verbose  # Equivalent
```

### Nested Subcommands

argus also supports nested subcommands for deeper command hierarchies:

```bash
my_program [global options] command subcommand [subcommand options] [arguments]
```

Examples:
```bash
my_program --debug service create --name myservice --image ubuntu
my_program config set server.port 8080
```

### Command Abbreviations

argus supports command name abbreviations, allowing users to type shortened versions as long as they are unambiguous:

```bash
# These are equivalent if no other command starts with "i":
my_program install package.tgz
my_program inst package.tgz
my_program i package.tgz

# But ambiguous if there are multiple matches
my_program i package.tgz  # ERROR if both "install" and "init" exist
```

## Mixing Different Formats

argus allows mixing different format styles in a single command line:

```bash
my_program -v --output=file.txt file1.txt file2.txt add --force extra.txt
```

This command includes:
- A short option flag (`-v`)
- A long option with value (`--output=file.txt`)
- Positional arguments (`file1.txt file2.txt`)
- A subcommand with its own option (`add --force extra.txt`)

## Handling Ambiguous Options

In some cases, command-line input can be ambiguous. argus resolves ambiguity using these rules:

1. An argument starting with a single dash (`-`) followed by a single character is treated as a short option
2. An argument starting with double dashes (`--`) is treated as a long option
3. An argument after `--` is always treated as a positional argument
4. An argument not starting with a dash is treated as a positional argument or subcommand (based on definition)

When parsing combined short options (`-abc`):
- Each character is treated as a separate boolean option
- If a non-boolean option is encountered, the rest of the string is treated as its value

## Command-Line Equivalent Forms

The following command forms are all equivalent with appropriate option definitions:

| Format | Example |
|--------|---------|
| Long options with equal sign | `./my_program --verbose --output=file.txt input.txt` |
| Long options with space | `./my_program --verbose --output file.txt input.txt` |
| Short options with space | `./my_program -v -o file.txt input.txt` |
| Short options no space | `./my_program -v -ofile.txt input.txt` |
| Combined short options | `./my_program -vo file.txt input.txt` |
| Combined short with value | `./my_program -vofile.txt input.txt` |
| Using `--` terminator | `./my_program -v -- --input-with-dashes.txt` |
| With subcommand | `./my_program -v add --force file.txt` |

## Important Notes

!!! warning
    - Boolean options (flags) do not require a value; their presence is enough to enable them.
    - Combined options (`-abc`) only work for boolean options.
    - For non-boolean options, the last option in a combination can take a value.

!!! info "Examples"
    - `-vo output.txt` (enables the `-v` option and sets `-o` to `output.txt`)
    - `-vooutput.txt` (enables the `-v` option and sets `-o` to `output.txt`)
    - `-vxooutput.txt` (enables the `-v` and `-x` options, and sets `-o` to `output.txt`)

!!! note
    Positional arguments are always processed in the order defined in `ARGUS_OPTIONS`.
