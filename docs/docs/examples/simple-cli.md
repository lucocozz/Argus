# > Simple CLI_

Basic file processor demonstrating core Argus features.

## // Complete Example

```c title="simple_cli.c"
#include <argus.h>
#include <stdio.h>

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("result.txt")),
    OPTION_INT('c', "count", HELP("Processing iterations"), 
               VALIDATOR(V_RANGE(1, 100)), DEFAULT(1)),
    OPTION_STRING('f', "format", HELP("Output format"),
                  VALIDATOR(V_CHOICE_STR("text", "json", "xml")), DEFAULT("text")),

    POSITIONAL_STRING("input", HELP("Input file to process")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "simple_cli", "1.0.0");
    argus.description = "A simple file processing example";
    
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;
    
    // Access parsed values
    const char *input = argus_get(&argus, "input").as_string;
    const char *output = argus_get(&argus, "output").as_string;
    const char *format = argus_get(&argus, "format").as_string;
    int count = argus_get(&argus, "count").as_int;
    bool verbose = argus_get(&argus, "verbose").as_bool;
    
    if (verbose) {
        printf("Processing %s -> %s (%s format, %d iterations)\n", 
               input, output, format, count);
    }
    
    // Simulate processing
    FILE *in = fopen(input, "r");
    if (!in) {
        fprintf(stderr, "Error: Cannot open '%s'\n", input);
        return 1;
    }
    
    FILE *out = fopen(output, "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create '%s'\n", output);
        fclose(in);
        return 1;
    }
    
    // Process based on format
    for (int i = 0; i < count; i++) {
        rewind(in);
        
        if (strcmp(format, "json") == 0) {
            fprintf(out, "{\"iteration\":%d,\"content\":\"", i + 1);
            int c;
            while ((c = fgetc(in)) != EOF) {
                if (c == '"') fprintf(out, "\\\"");
                else if (c == '\n') fprintf(out, "\\n");
                else fputc(c, out);
            }
            fprintf(out, "\"}\n");
        } else if (strcmp(format, "xml") == 0) {
            fprintf(out, "<iteration num=\"%d\">", i + 1);
            int c;
            while ((c = fgetc(in)) != EOF) {
                if (c == '<') fprintf(out, "&lt;");
                else if (c == '>') fprintf(out, "&gt;");
                else fputc(c, out);
            }
            fprintf(out, "</iteration>\n");
        } else {
            fprintf(out, "=== Iteration %d ===\n", i + 1);
            int c;
            while ((c = fgetc(in)) != EOF) {
                fputc(c, out);
            }
            fprintf(out, "\n");
        }
    }
    
    fclose(in);
    fclose(out);
    
    printf("Processing complete: %s\n", output);
    
    argus_free(&argus);
    return 0;
}
```

## // Usage

```bash
# Build
gcc simple_cli.c -o simple_cli -largus

# Basic usage
echo "Hello World" > input.txt
./simple_cli input.txt

# With options
./simple_cli --verbose --format json --count 3 input.txt

# Help
./simple_cli --help
```

## // Generated Help

```
simple_cli v1.0.0

A simple file processing example

Usage: simple_cli [OPTIONS] <input>

Arguments:
  <input>                - Input file to process

Options:
  -h, --help             - Display this help message (exit)
  -V, --version          - Display version information (exit)
  -v, --verbose          - Enable verbose output
  -o, --output <STR>     - Output file (default: "result.txt")
  -c, --count <1-100>    - Processing iterations (default: 1)
  -f, --format <text|json|xml>
                         - Output format (default: "text")
```

## // Key Features

- **Flag**: `--verbose` (boolean toggle)
- **String with default**: `--output` 
- **Integer with validation**: `--count` (range 1-100)
- **Choice validation**: `--format` (text/json/xml only)
- **Required positional**: input file
- **Auto-generated help**: from option definitions

## // Error Examples

```bash
$ ./simple_cli --count 150 input.txt
simple_cli: Value 150 is out of range [1, 100]

$ ./simple_cli --format pdf input.txt  
simple_cli: The 'format' option cannot be set to 'pdf'. Choose from ["text", "json", "xml"]

$ ./simple_cli
simple_cli: Required positional argument missing: 'input'
```
