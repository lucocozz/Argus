# > Welcome to Argus_

Modern, declarative argument parsing for C developers who value their time.

## // From Tedious to Elegant

You know the drill. Every CLI tool starts the same way:

```c
int opt, verbose = 0, port = 8080;
while ((opt = getopt(argc, argv, "vo:p:")) != -1) {
    switch (opt) {
        case 'v': verbose = 1; break;
        case 'o': output = optarg; break;
        case 'p': port = atoi(optarg); /* crossing fingers */ break;
        case '?': /* sigh... error handling */ break;
    }
}
// TODO: validate port range, write help text, handle edge cases...
```

Then hours later, you're deep in validation logic and help text formatting.

## // What if you could just describe what you want?

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("output.txt")),
    OPTION_INT('p', "port", HELP("Port number"), 
               VALIDATOR(V_RANGE(1, 65535)), DEFAULT(8080)),
)

int main(int argc, char **argv) {
    argus_t argus = argus_init(options, "my_tool", "1.0.0");
    
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;
    
    // Everything is parsed, validated, and ready to use
    bool verbose = argus_get(&argus, "verbose").as_bool;
    const char *output = argus_get(&argus, "output").as_string;
    int port = argus_get(&argus, "port").as_int;
    
    argus_free(&argus);
    return 0;
}
```

## // The Philosophy

**Declare once, get everything.** Argus handles the parsing, validation, type conversion, help generation, and error messages. You focus on what your application actually does.

**Modern C shouldn't feel ancient.** While respecting C's principles, Argus brings conveniences you'd expect from modern languages—without the complexity.

**Production-ready by design.** Type safety, comprehensive validation, cross-platform support, and extensive testing are built-in, not afterthoughts.

## // When to Use Argus

**Perfect for:**
- CLI tools and utilities
- Developer tools and build systems  
- Applications with complex argument structures
- Projects where maintainability matters
- Teams that want consistent argument handling

**Consider alternatives if:**
- You're on embedded systems with tight constraints
- You're extending existing getopt-based code
- You need maximum performance over developer experience
- You're stuck with pre-C11 compilers

## // What You'll Learn

This guide walks you through building robust command-line interfaces:

1. **[Installation](./installation)** - Getting Argus into your project  
2. **[Quick Start](./quickstart)** - Your first CLI in minutes
3. **[Core Concepts](../fundamentals/basic-options)** - Options, types, and validation
4. **[Advanced Features](../features/subcommands)** - Subcommands, collections, and complex patterns

## // Ready to Get Started?

Jump to [installation](./installation) or dive into the [quickstart guide](./quickstart) if you're ready to code.

---

:::tip
Try `--help` on any Argus-powered application to see the automatically generated help. It's always up-to-date and consistently formatted.
:::
