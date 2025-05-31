# Subcommands

Build Git-like command-line interfaces with hierarchical commands, each having their own options and behavior.

## Basic Concept

Transform your program from a single-purpose tool into a multi-command application:

```
my_program [global options] command [command options] [arguments]
```

Examples: `git commit`, `docker run`, `kubectl apply`

## Basic Subcommand Setup

Define action handlers, subcommand options, and link them together:

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs>
<TabItem value="basic" label="Basic Structure" default>

```c
#include "argus.h"

// 1. Define action handlers (functions called when subcommand executes)
int add_command(argus_t *argus, void *data);
int remove_command(argus_t *argus, void *data);

// 2. Define options for each subcommand
ARGUS_OPTIONS(
    add_options,
    HELP_OPTION(),
    OPTION_FLAG('f', "force", HELP("Force add operation")),
    POSITIONAL_STRING("file", HELP("File to add")),
)

ARGUS_OPTIONS(
    remove_options,
    HELP_OPTION(),
    OPTION_FLAG('r', "recursive", HELP("Remove directories recursively")),
    POSITIONAL_STRING("file", HELP("File to remove")),
)

// 3. Define main options with subcommands
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    
    // Global options (available to all subcommands)
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    
    // Define subcommands
    SUBCOMMAND("add", add_options, HELP("Add files to index"), ACTION(add_command)),
    SUBCOMMAND("remove", remove_options, HELP("Remove files from index"), ACTION(remove_command)),
)
```

</TabItem>
<TabItem value="handlers" label="Action Handlers">

```c
// Implementation of subcommand handlers
int add_command(argus_t *argus, void *data)
{
    (void)data; // Unused parameter
    
    // Access global options with dot prefix
    bool verbose = argus_get(*argus, ".verbose").as_bool;
    
    // Access subcommand options by name
    const char *file = argus_get(*argus, "file").as_string;
    bool force = argus_get(*argus, "force").as_bool;

    printf("Adding file: %s\n", file);
    if (verbose) printf("  verbose mode enabled\n");
    if (force) printf("  with force option\n");

    return 0;
}

int remove_command(argus_t *argus, void *data)
{
    (void)data;
    
    bool verbose = argus_get(*argus, ".verbose").as_bool;
    const char *file = argus_get(*argus, "file").as_string;
    bool recursive = argus_get(*argus, "recursive").as_bool;

    printf("Removing file: %s\n", file);
    if (verbose) printf("  verbose mode enabled\n");
    if (recursive) printf("  recursively\n");

    return 0;
}
```

</TabItem>
<TabItem value="main" label="Main Function">

```c
int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "vcs_tool", "1.0.0");
    argus.description = "Version control system example";

    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS)
        return status;

    // Check if a subcommand was specified and execute it
    if (argus_has_command(argus)) {
        status = argus_exec(&argus, NULL);
    } else {
        printf("No command specified. Use --help to see available commands.\n");
    }

    argus_free(&argus);
    return status;
}
```

</TabItem>
</Tabs>

## Help Integration

Argus automatically generates contextual help for subcommands:

<Tabs>
<TabItem value="main-help" label="Main Help" default>

```bash
$ ./vcs_tool --help
vcs_tool v1.0.0

Version control system example

Usage: vcs_tool [OPTIONS] COMMAND

Options:
  -h, --help             - Display this help message (exit)
  -V, --version          - Display version information (exit)
  -v, --verbose          - Enable verbose output

Commands:
  add                    - Add files to index
  remove                 - Remove files from index

Run 'vcs_tool COMMAND --help' for more information on a command.
```

</TabItem>
<TabItem value="subcommand-help" label="Subcommand Help">

```bash
$ ./vcs_tool add --help
vcs_tool v1.0.0

Usage: vcs_tool add [OPTIONS] <file>

Add files to index

Arguments:
  <file>                 - File to add

Options:
  -h, --help             - Display this help message (exit)
  -f, --force            - Force add operation
```

</TabItem>
</Tabs>

## Accessing Values in Subcommands

Use different path formats to access options within subcommand handlers:

<Tabs>
<TabItem value="relative" label="Relative Paths" default>

```c
int add_command(argus_t *argus, void *data)
{
    // Relative paths (within current subcommand context)
    const char *file = argus_get(*argus, "file").as_string;
    bool force = argus_get(*argus, "force").as_bool;
    
    return 0;
}
```

**Usage:** Access options defined in the current subcommand.

</TabItem>
<TabItem value="absolute" label="Absolute Paths">

```c
int add_command(argus_t *argus, void *data)
{
    // Absolute paths (full path from root)
    const char *file = argus_get(*argus, "add.file").as_string;
    bool force = argus_get(*argus, "add.force").as_bool;
    
    return 0;
}
```

**Usage:** Explicit full path, useful for disambiguation.

</TabItem>
<TabItem value="global" label="Global Options">

```c
int add_command(argus_t *argus, void *data)
{
    // Global options (dot prefix for root level)
    bool verbose = argus_get(*argus, ".verbose").as_bool;
    
    // Mix global and local options
    const char *file = argus_get(*argus, "file").as_string;
    
    return 0;
}
```

**Usage:** Access options defined at the root level from any subcommand.

</TabItem>
</Tabs>

## Nested Subcommands

Create multi-level command hierarchies like `docker container run`:

<Tabs>
<TabItem value="nested-structure" label="Structure" default>

```c
// Third level: service create options
ARGUS_OPTIONS(
    service_create_options,
    HELP_OPTION(),
    OPTION_STRING('n', "name", HELP("Service name"), FLAGS(FLAG_REQUIRED)),
    OPTION_STRING('i', "image", HELP("Container image"), FLAGS(FLAG_REQUIRED)),
)

// Second level: service command options
ARGUS_OPTIONS(
    service_options,
    HELP_OPTION(),
    SUBCOMMAND("create", service_create_options, 
               HELP("Create a new service"), ACTION(service_create_action)),
    SUBCOMMAND("list", service_list_options, 
               HELP("List services"), ACTION(service_list_action)),
)

// First level: main options
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    OPTION_FLAG('d', "debug", HELP("Enable debug mode")),
    
    SUBCOMMAND("service", service_options, HELP("Service management commands")),
    SUBCOMMAND("config", config_options, HELP("Configuration commands")),
)
```

</TabItem>
<TabItem value="nested-paths" label="Nested Paths">

```c
int service_create_action(argus_t *argus, void *data)
{
    // Different ways to access nested values
    
    // 1. Relative path (current context)
    const char *name = argus_get(*argus, "name").as_string;
    
    // 2. Absolute path (full hierarchy)
    const char *image = argus_get(*argus, "service.create.image").as_string;
    
    // 3. Root-level option
    bool debug = argus_get(*argus, ".debug").as_bool;
    
    printf("Creating service '%s' with image '%s'\n", name, image);
    if (debug) printf("Debug mode enabled\n");
    
    return 0;
}
```

</TabItem>
<TabItem value="nested-usage" label="Usage Examples">

```bash
# Nested command usage
$ ./tool service create --name web --image nginx:latest

# Help at each level
$ ./tool --help                    # Main help
$ ./tool service --help            # Service subcommand help
$ ./tool service create --help     # Create subcommand help

# Global options work at any level
$ ./tool --debug service create --name web --image nginx
```

</TabItem>
</Tabs>

## Command Abbreviation

Argus supports Git-style command abbreviation - users can type shortened command names:

```c
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    SUBCOMMAND("install", install_options, HELP("Install packages")),
    SUBCOMMAND("init", init_options, HELP("Initialize project")),
    SUBCOMMAND("status", status_options, HELP("Show status")),
)
```

**Abbreviation examples:**
```bash
$ ./tool install package.tar.gz    # Full command
$ ./tool inst package.tar.gz       # Abbreviated (unique)
$ ./tool ins package.tar.gz        # Abbreviated (unique)
$ ./tool i package.tar.gz          # Error: ambiguous (install vs init)

$ ./tool status                    # Full command
$ ./tool stat                      # Abbreviated
$ ./tool st                        # Abbreviated
```

**Rules:**
- Must be unambiguous (unique prefix)
- Case-sensitive matching
- Works at all nesting levels

## Advanced Features

<Tabs>
<TabItem value="context-data" label="Shared Context" default>

Pass application state and resources between main and subcommand handlers:

```c
typedef struct {
    FILE *log_file;
    sqlite3 *database;
    config_t *app_config;
    thread_pool_t *workers;
} app_context_t;

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;
    
    // Initialize application resources
    app_context_t context = {
        .log_file = fopen("app.log", "a"),
        .database = init_database("app.db"),
        .app_config = load_config("config.json"),
        .workers = create_thread_pool(4)
    };
    
    if (argus_has_command(argus)) {
        // Pass shared resources to subcommand
        int status = argus_exec(&argus, &context);
        
        // Clean up resources
        cleanup_context(&context);
        return status;
    }
    
    argus_free(&argus);
    return 0;
}

int service_create_action(argus_t *argus, void *data)
{
    app_context_t *ctx = (app_context_t *)data;
    
    // Access command-line options normally
    const char *name = argus_get(*argus, "name").as_string;
    
    // Use shared application resources
    log_info(ctx->log_file, "Creating service: %s", name);
    save_to_database(ctx->database, name);
    schedule_task(ctx->workers, create_service_task(name));
    
    return 0;
}
```

**Use cases:** Database connections, configuration objects, logging, thread pools, network clients.

</TabItem>
<TabItem value="checking-commands" label="Command Detection">

Check which commands are active:

```c
int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "myapp", "1.0.0");
    argus_parse(&argus, argc, argv);
    
    // Check top-level command
    if (argus_is_set(argus, "service"))
    {
        printf("Service command detected\n");

        // Check nested command
        if (argus_is_set(argus, "service.create"))
            printf("Service create command\n");
        else if (argus_is_set(argus, "service.list"))
            printf("Service list command\n");
    }
    else if (argus_is_set(argus, "config"))
        printf("Config command detected\n");

    if (argus_has_command(argus))
        argus_exec(&argus, NULL);

    argus_free(&argus);
    return 0;
}
```

</TabItem>
<TabItem value="environment" label="Environment Variables">

Subcommands work with environment variables:

```c
ARGUS_OPTIONS(
    deploy_options,
    HELP_OPTION(),
    OPTION_STRING('e', "environment", HELP("Target environment"),
                  ENV_VAR("DEPLOY_ENV"), DEFAULT("staging")),
    OPTION_BOOL('\0', "dry-run", HELP("Dry run mode"),
                ENV_VAR("DRY_RUN"), DEFAULT(false)),
)

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", ENV_VAR("VERBOSE")),
    SUBCOMMAND("deploy", deploy_options, ACTION(deploy_action)),
)
```

**Environment usage:**
```bash
export MYAPP_VERBOSE=true
export MYAPP_DEPLOY_ENV=production
export MYAPP_DRY_RUN=false

./myapp deploy  # Uses environment values
```

</TabItem>
</Tabs>

## Real-World Examples

<Tabs>
<TabItem value="git-like" label="Git-like Tool" default>

```c
#include "argus.h"

// Action handlers
int add_action(argus_t *argus, void *data);
int commit_action(argus_t *argus, void *data);
int push_action(argus_t *argus, void *data);

// Add command options
ARGUS_OPTIONS(
    add_options,
    HELP_OPTION(),
    OPTION_FLAG('A', "all", HELP("Add all files")),
    OPTION_FLAG('f', "force", HELP("Force add ignored files")),
    POSITIONAL_STRING("files", HELP("Files to add"), FLAGS(FLAG_OPTIONAL)),
)

// Commit command options
ARGUS_OPTIONS(
    commit_options,
    HELP_OPTION(),
    OPTION_STRING('m', "message", HELP("Commit message"), FLAGS(FLAG_REQUIRED)),
    OPTION_STRING('\0', "author", HELP("Override author")),
    OPTION_FLAG('\0', "amend", HELP("Amend previous commit")),
)

// Push command options
ARGUS_OPTIONS(
    push_options,
    HELP_OPTION(),
    OPTION_STRING('\0', "remote", HELP("Remote name"), DEFAULT("origin")),
    OPTION_STRING('\0', "branch", HELP("Branch name")),
    OPTION_FLAG('f', "force", HELP("Force push")),
    OPTION_FLAG('\0', "tags", HELP("Push tags")),
)

// Main options
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('C', "directory", HELP("Run in directory"), HINT("DIR")),
    
    SUBCOMMAND("add", add_options, HELP("Add files to staging area"), ACTION(add_action)),
    SUBCOMMAND("commit", commit_options, HELP("Record changes"), ACTION(commit_action)),
    SUBCOMMAND("push", push_options, HELP("Push changes to remote"), ACTION(push_action)),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "mygit", "1.0.0");
    argus.description = "A simple version control system";
    
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;
    
    if (argus_has_command(argus)) {
        return argus_exec(&argus, NULL);
    } else {
        printf("No command specified. Use --help to see available commands.\n");
        return 1;
    }
    
    argus_free(&argus);
}
```

</TabItem>
<TabItem value="docker-like" label="Container Tool">

```c
// Container management tool
ARGUS_OPTIONS(
    container_run_options,
    HELP_OPTION(),
    OPTION_STRING('n', "name", HELP("Container name")),
    OPTION_MAP_STRING('e', "env", HELP("Environment variables")),
    OPTION_ARRAY_STRING('p', "port", HELP("Port mappings")),
    OPTION_FLAG('d', "detach", HELP("Run in background")),
    POSITIONAL_STRING("image", HELP("Container image")),
)

ARGUS_OPTIONS(
    container_options,
    HELP_OPTION(),
    SUBCOMMAND("run", container_run_options, ACTION(container_run_action)),
    SUBCOMMAND("stop", container_stop_options, ACTION(container_stop_action)),
    SUBCOMMAND("list", container_list_options, ACTION(container_list_action)),
)

ARGUS_OPTIONS(
    image_options,
    HELP_OPTION(),
    SUBCOMMAND("pull", image_pull_options, ACTION(image_pull_action)),
    SUBCOMMAND("build", image_build_options, ACTION(image_build_action)),
    SUBCOMMAND("list", image_list_options, ACTION(image_list_action)),
)

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    
    SUBCOMMAND("container", container_options, HELP("Container management")),
    SUBCOMMAND("image", image_options, HELP("Image management")),
)
```

**Usage examples:**
```bash
# Container commands
./container-tool container run --name web -p 8080:80 nginx
./container-tool container list
./container-tool container stop web

# Image commands  
./container-tool image pull nginx:latest
./container-tool image build -t myapp .
./container-tool image list

# Help at each level
./container-tool --help
./container-tool container --help
./container-tool container run --help
```

</TabItem>
</Tabs>

## Best Practices

### ✅ Good Practices

```c
// 1. Use descriptive command names
SUBCOMMAND("deploy", deploy_options, HELP("Deploy to production"))
SUBCOMMAND("rollback", rollback_options, HELP("Rollback deployment"))

// 2. Group related commands
SUBCOMMAND("database", db_options, HELP("Database operations"))
// db_options contains: create, migrate, backup, restore

// 3. Use consistent option naming across commands
// Both commands use --force for similar behavior
OPTION_FLAG('f', "force", HELP("Force operation"))

// 4. Provide clear help text
SUBCOMMAND("migrate", migrate_options, 
           HELP("Run database migrations"), ACTION(migrate_action))

// 5. Use global options for cross-cutting concerns
OPTION_FLAG('v', "verbose", HELP("Enable verbose output"))
OPTION_FLAG('q', "quiet", HELP("Suppress non-error output"))
```

### ❌ Avoid These Patterns

```c
// ❌ Don't use cryptic command names
SUBCOMMAND("x", extract_options, HELP("Extract archive"))

// ❌ Don't nest too deeply (3+ levels)
SUBCOMMAND("service", service_options, ...)
// service -> container -> instance -> logs (too deep)

// ❌ Don't ignore error handling in actions
int bad_action(argus_t *argus, void *data)
{
    // Missing error handling
    perform_operation();
    return 0; // Always returns success
}
```

## Error Handling in Actions

```c
int deploy_action(argus_t *argus, void *data)
{
    const char *env = argus_get(*argus, "environment").as_string;
    
    // Validate deployment target
    if (strcmp(env, "production") == 0) {
        printf("Are you sure you want to deploy to production? (y/N): ");
        char response;
        scanf(" %c", &response);
        if (response != 'y' && response != 'Y') {
            printf("Deployment cancelled.\n");
            return 1;
        }
    }
    
    // Perform deployment
    if (perform_deployment(env) != 0) {
        fprintf(stderr, "Deployment failed\n");
        return 1;
    }
    
    printf("Deployment successful\n");
    return 0;
}
```

## What's Next?

- **[Collections](collections)** - Work with arrays and maps in subcommands
- **[Custom Handlers](../advanced/custom-handlers)** - Process complex subcommand data
- **[Nested Commands](../advanced/nested-commands)** - Advanced multi-level hierarchies
- **[Validation](validation)** - Validate subcommand options

:::tip Command Design
Design your command structure around user workflows, not internal code organization. Users should be able to discover and complete tasks intuitively.
:::
