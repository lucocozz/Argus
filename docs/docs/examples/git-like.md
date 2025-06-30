# Git-like Tool

Subcommand-based tool demonstrating hierarchical commands, actions, and context sharing.

## Complete Example

```c title="vcs_tool.c"
#include <argus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Shared application state
typedef struct {
    sqlite3 *db;           // Database connection
    config_t *config;      // Application configuration
    log_file_t *logger;    // Logging system
} app_context_t;

// Action handlers
int add_action(argus_t *argus, void *data);
int commit_action(argus_t *argus, void *data);
int push_action(argus_t *argus, void *data);
int status_action(argus_t *argus, void *data);

// Add command options
ARGUS_OPTIONS(
    add_options,
    HELP_OPTION(),
    OPTION_FLAG('A', "all", HELP("Add all modified files")),
    OPTION_FLAG('f', "force", HELP("Force add ignored files")),
    OPTION_ARRAY_STRING('i', "include", HELP("Include specific patterns")),
    POSITIONAL_STRING("files", HELP("Files to add"), FLAGS(FLAG_OPTIONAL)),
)

// Commit command options
ARGUS_OPTIONS(
    commit_options,
    HELP_OPTION(),
    OPTION_STRING('m', "message", HELP("Commit message"), FLAGS(FLAG_REQUIRED)),
    OPTION_STRING('\0', "author", HELP("Override author")),
    OPTION_FLAG('\0', "amend", HELP("Amend previous commit")),
    OPTION_FLAG('s', "signoff", HELP("Add signed-off-by line")),
)

// Push command options
ARGUS_OPTIONS(
    push_options,
    HELP_OPTION(),
    OPTION_STRING('\0', "remote", HELP("Remote name"), DEFAULT("origin")),
    OPTION_STRING('\0', "branch", HELP("Branch to push")),
    OPTION_FLAG('f', "force", HELP("Force push")),
    OPTION_FLAG('\0', "tags", HELP("Push tags too")),
    OPTION_FLAG('n', "dry-run", HELP("Show what would be pushed")),
)

// Status command options  
ARGUS_OPTIONS(
    status_options,
    HELP_OPTION(),
    OPTION_FLAG('s', "short", HELP("Short format output")),
    OPTION_FLAG('\0', "porcelain", HELP("Machine-readable output")),
)

// Main options with subcommands
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    
    // Global options
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('C', "directory", HELP("Run in directory"), 
                  HINT("DIR"), DEFAULT(".")),
    
    // Subcommands
    SUBCOMMAND("add", add_options, HELP("Add files to staging"), ACTION(add_action)),
    SUBCOMMAND("commit", commit_options, HELP("Record changes"), ACTION(commit_action)),
    SUBCOMMAND("push", push_options, HELP("Push to remote"), ACTION(push_action)),
    SUBCOMMAND("status", status_options, HELP("Show working tree status"), ACTION(status_action)),
)

int add_action(argus_t *argus, void *data)
{
    app_context_t *ctx = (app_context_t *)data;
    
    // Access subcommand options
    bool add_all = argus_get(argus, "all").as_bool;
    bool force = argus_get(argus, "force").as_bool;
    const char *files = argus_get(argus, "files").as_string;
    
    // Access global options
    bool verbose = argus_get(argus, ".verbose").as_bool;
    const char *repo_path = argus_get(argus, ".directory").as_string;
    
    if (verbose) {
        printf("Repository: %s\n", repo_path);
        printf("Add command options:\n");
        printf("  All files: %s\n", add_all ? "yes" : "no");
        printf("  Force: %s\n", force ? "yes" : "no");
    }
    
    if (add_all) {
        printf("Adding all modified files...\n");
        // Simulate adding all files
        printf("Added: file1.c file2.h docs/readme.md\n");
    } else if (files) {
        printf("Adding specific files: %s\n", files);
    } else {
        printf("No files specified. Use --all or specify files.\n");
        return 1;
    }
    
    // Check include patterns
    if (argus_is_set(argus, "include")) {
        printf("Include patterns:\n");
        argus_array_it_t it = argus_array_it(argus, "include");
        while (argus_array_next(&it)) {
            printf("  %s\n", it.value.as_string);
        }
    }
    
    if (force) {
        printf("Force adding ignored files...\n");
    }
    
    printf("Files staged for commit.\n");
    return 0;
}

int commit_action(argus_t *argus, void *data)
{
    app_context_t *ctx = (app_context_t *)data;
    
    const char *message = argus_get(argus, "message").as_string;
    const char *author = argus_get(argus, "author").as_string;
    bool amend = argus_get(argus, "amend").as_bool;
    bool signoff = argus_get(argus, "signoff").as_bool;
    
    // Access global options
    bool verbose = argus_get(argus, ".verbose").as_bool;
    const char *repo_path = argus_get(argus, ".directory").as_string;
    
    if (verbose) {
        printf("Repository: %s\n", repo_path);
        printf("Commit message: %s\n", message);
        if (author) printf("Author override: %s\n", author);
        printf("Amend: %s\n", amend ? "yes" : "no");
        printf("Sign-off: %s\n", signoff ? "yes" : "no");
    }
    
    if (amend) {
        printf("Amending previous commit...\n");
    } else {
        printf("Creating new commit...\n");
    }
    
    printf("Commit: abc1234 \"%s\"\n", message);
    
    if (signoff) {
        printf("Signed-off-by: Current User <user@example.com>\n");
    }
    
    return 0;
}

int push_action(argus_t *argus, void *data)
{
    app_context_t *ctx = (app_context_t *)data;
    
    const char *remote = argus_get(argus, "remote").as_string;
    const char *branch = argus_get(argus, "branch").as_string;
    bool force = argus_get(argus, "force").as_bool;
    bool tags = argus_get(argus, "tags").as_bool;
    bool dry_run = argus_get(argus, "dry-run").as_bool;
    
    // Access global options
    bool verbose = argus_get(argus, ".verbose").as_bool;
    const char *repo_path = argus_get(argus, ".directory").as_string;
    
    if (verbose) {
        printf("Repository: %s\n", repo_path);
        printf("Remote: %s\n", remote);
        if (branch) printf("Branch: %s\n", branch);
    }
    
    if (dry_run) {
        printf("Would push to %s", remote);
        if (branch) printf("/%s", branch);
        printf("\n");
        
        if (tags) printf("Would also push tags\n");
        if (force) printf("Would force push\n");
        return 0;
    }
    
    printf("Pushing to %s", remote);
    if (branch) printf("/%s", branch);
    printf("...\n");
    
    if (force) {
        printf("Force pushing (this may overwrite remote changes)\n");
    }
    
    if (tags) {
        printf("Pushing tags...\n");
    }
    
    printf("Push successful!\n");
    return 0;
}

int status_action(argus_t *argus, void *data)
{
    app_context_t *ctx = (app_context_t *)data;
    
    bool short_format = argus_get(argus, "short").as_bool;
    bool porcelain = argus_get(argus, "porcelain").as_bool;
    
    // Access global options  
    bool verbose = argus_get(argus, ".verbose").as_bool;
    const char *repo_path = argus_get(argus, ".directory").as_string;
    
    if (verbose && !short_format && !porcelain) {
        printf("Repository: %s\n", repo_path);
    }
    
    if (porcelain) {
        // Machine-readable format
        printf("M  file1.c\n");
        printf("A  file2.h\n");
        printf("?? untracked.txt\n");
    } else if (short_format) {
        printf(" M file1.c\n");
        printf("A  file2.h\n");
        printf("?? untracked.txt\n");
    } else {
        printf("On branch main\n");
        printf("Changes to be committed:\n");
        printf("  (use \"vcs reset HEAD <file>...\" to unstage)\n\n");
        printf("\tnew file:   file2.h\n\n");
        
        printf("Changes not staged for commit:\n");
        printf("  (use \"vcs add <file>...\" to update what will be committed)\n\n");
        printf("\tmodified:   file1.c\n\n");
        
        printf("Untracked files:\n");
        printf("  (use \"vcs add <file>...\" to include in what will be committed)\n\n");
        printf("\tuntracked.txt\n\n");
    }
    
    return 0;
}

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "vcs", "1.0.0");
    argus.description = "A simple version control system";
    
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;
    
    // Create application context for shared resources
    app_context_t context = {
        .db = init_database("repo.db"),
        .config = load_config("vcs.conf"),
        .logger = open_log("vcs.log")
    };
    
    if (argus_has_command(&argus)) {
        // Execute the parsed subcommand
        int result = argus_exec(&argus, &context);
        argus_free(&argus);
        return result;
    } 
    else {
        // No subcommand provided
        printf("No command specified.\n\n");
        argus_print_usage(&argus);
        argus_print_help(&argus);
        argus_free(&argus);
        return 1;
    }
}
```

## Usage Examples

### Basic Commands

```bash
# Build
gcc vcs_tool.c -o vcs -largus

# Add files
./vcs add file1.c file2.h
./vcs add --all
./vcs add --force ignored_file.tmp

# Commit changes  
./vcs commit --message "Add new features"
./vcs commit -m "Fix bug" --author "John Doe"
./vcs commit -m "Update docs" --signoff

# Push to remote
./vcs push
./vcs push --remote origin --branch feature/new
./vcs push --force --tags --dry-run

# Check status
./vcs status
./vcs status --short
./vcs status --porcelain
```

### Global Options

```bash
# Verbose output for any command
./vcs --verbose add file.c
./vcs -v commit -m "Verbose commit"

# Run in different directory
./vcs -C /path/to/repo status
./vcs --directory ../other-repo add --all
```

### Help at Each Level

```bash
# Main help
./vcs --help

# Subcommand help
./vcs add --help
./vcs commit --help
./vcs push --help
```

## Generated Help

### Main Help
```
vcs v1.0.0

A simple version control system

Usage: vcs [OPTIONS] COMMAND

Options:
  -h, --help             - Display this help message (exit)
  -V, --version          - Display version information (exit)
  -v, --verbose          - Verbose output
  -C, --directory <DIR>  - Run in directory (default: ".")

Commands:
  add                    - Add files to staging
  commit                 - Record changes
  push                   - Push to remote
  status                 - Show working tree status

Run 'vcs COMMAND --help' for more information on a command.
```

### Subcommand Help
```
$ ./vcs add --help
vcs v1.0.0

Usage: vcs add [OPTIONS] [files]

Add files to staging

Arguments:
  [files]                - Files to add (optional)

Options:
  -h, --help             - Display this help message (exit)
  -A, --all              - Add all modified files
  -f, --force            - Force add ignored files
  -i, --include <STR,...> - Include specific patterns

$ ./vcs commit --help
vcs v1.0.0

Usage: vcs commit [OPTIONS]

Record changes

Options:
  -h, --help             - Display this help message (exit)
  -m, --message <STR>    - Commit message (required)
      --author <STR>     - Override author
      --amend            - Amend previous commit
  -s, --signoff          - Add signed-off-by line
```

## Key Features Demonstrated

### Subcommand Structure
- **Global options** available to all commands (`-v`, `-C`)
- **Command-specific options** for each subcommand
- **Action handlers** that receive parsed arguments and shared context
- **Contextual help** for each command level

### Value Access Patterns
```c
// In action handlers:
bool verbose = argus_get(argus, ".verbose").as_bool;     // Global option (dot prefix)
const char *msg = argus_get(argus, "message").as_string; // Local option (relative)
```

### Context Sharing
```c
typedef struct {
    bool verbose;
    const char *repo_path;
} app_context_t;

// Pass to argus_exec()
argus_exec(&argus, &context);
```

### Error Handling
```bash
$ ./vcs commit
vcs: Required option is missing: 'message' with option 'commit'

$ ./vcs unknown-command
vcs: Unknown positional: 'unknown-command'

$ ./vcs
No command specified.
Try 'vcs COMMAND --help' for command-specific help.
```

## Real-World Patterns

This example demonstrates common CLI tool patterns:
- **Git-style** command structure
- **Docker-style** global + local options
- **Kubectl-style** context passing
- **Npm-style** hierarchical help
