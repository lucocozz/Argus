name := "argus"
build_dir := ".build"
static_lib := "lib" + name + ".a"
shared_lib := "lib" + name + ".so"

# Default values for variables
tests := "false"
examples := "false"
benchmarks := "false"
build_type := "debugoptimized"
regex := "true"

# Set default recipe to run when just is called without arguments
default: build compile

# Main tasks
build: configure compile

configure:
    @meson setup -Dtests={{tests}} -Dexamples={{examples}} -Dbenchmarks={{benchmarks}} -Dbuildtype={{build_type}} -Dregex={{regex}} {{build_dir}}
    @ln -sf {{build_dir}}/{{static_lib}} {{static_lib}} 2>/dev/null || true
    @ln -sf {{build_dir}}/{{shared_lib}} {{shared_lib}} 2>/dev/null || true

reconfigure:
    @meson setup --reconfigure -Dtests={{tests}} -Dexamples={{examples}} -Dbenchmarks={{benchmarks}} -Dbuildtype={{build_type}} -Dregex={{regex}} {{build_dir}}

compile:
    @meson compile -C {{build_dir}}

# Clean tasks
clean:
    @rm -f {{static_lib}} {{shared_lib}}
    @echo "{{static_lib}} and {{shared_lib}} removed ✅"

fclean: clean
    @rm -rf {{build_dir}}
    @echo "Build directory cleaned ✅"

re: reconfigure compile

# Build variants
debug:
    @just build_type="debug" reconfigure compile

release:
    @just build_type="release" reconfigure compile

# =====================
# Test related commands
# =====================

# Main test command - configure, build and run all tests
test: setup-tests
    @meson test -C {{build_dir}} -v
    @echo "\033[1;32mAll tests completed ✅\033[0m"

# Setup tests (reconfigure with tests enabled and compile)
setup-tests:
    @just tests="true" reconfigure compile

# Run only specific test suites
test-unit: setup-tests
    @meson test -C {{build_dir}} --suite unit -v
    @echo "\033[1;32mUnit tests completed ✅\033[0m"

test-integration: setup-tests
    @meson test -C {{build_dir}} --suite integration -v
    @echo "\033[1;32mIntegration tests completed ✅\033[0m"

test-functional: setup-tests
    @meson test -C {{build_dir}} --suite functional -v
    @echo "\033[1;32mFunctional tests completed ✅\033[0m"

# Run a specific test by name (e.g. just test-one unit_strings)
test-one suite_test_name="": setup-tests
    @meson test -C {{build_dir}} {{suite_test_name}} -v
    @echo "\033[1;32mTest {{suite_test_name}} completed ✅\033[0m"

# Run tests with detailed output
test-verbose: setup-tests
    @meson test -C {{build_dir}} -v --print-errorlogs
    @echo "\033[1;32mAll tests completed with detailed output ✅\033[0m"

# Generate test coverage report (requires gcovr)
test-coverage: setup-tests
    @meson configure -Db_coverage=true {{build_dir}}
    @ninja -C {{build_dir}} coverage
    @echo "\033[1;32mTest coverage report generated ✅\033[0m"
    @echo "Coverage report available at: {{build_dir}}/meson-logs/coverage/"

# List all available tests
test-list:
    @meson test -C {{build_dir}} --list
    @echo "\033[1;34m═════════════════════════════════════════════════\033[0m"
    @echo "\033[1;33mHow to run a specific test:\033[0m"
    @echo "  \033[1;37mjust test-one unit_strings\033[0m"
    @echo "\033[1;34m═════════════════════════════════════════════════\033[0m"

# =====================
# Benchmark commands
# =====================

# Build and run benchmarks
benchmark:
    @just benchmarks="true" reconfigure compile
    @echo "\n\033[1;34m═══════════════════════════════════════════════════════\033[0m"
    @echo "\033[1;33m          ARGUS PERFORMANCE BENCHMARKS\033[0m"
    @echo "\033[1;34m═══════════════════════════════════════════════════════\033[0m"
    
    @bash -c '\
        benchmark_exes=$(find {{build_dir}}/benchmarks -type f -executable -not -path "*.p/*"); \
        if [ -z "$benchmark_exes" ]; then \
            echo "No benchmark executables found! ❌"; \
            exit 1; \
        fi; \
        for bench in $benchmark_exes; do \
            name=$(basename $bench); \
            echo "⏱️  Running benchmark: $name"; \
            echo "    Mode comparison (both normal and release):"; \
            $bench 2; \
            echo ""; \
        done \
    '
    
    @echo "\n\033[1;34m═══════════════════════════════════════════════════════\033[0m"
    @echo "\033[1;32m                 BENCHMARKS COMPLETE ✅\033[0m"
    @echo "\033[1;34m═══════════════════════════════════════════════════════\033[0m\n"

# Run specific benchmark modes
benchmark-normal:
    @just benchmarks="true" reconfigure compile
    @bash -c '\
        for bench in $(find {{build_dir}}/benchmarks -type f -executable -not -path "*.p/*"); do \
            name=$(basename $bench); \
            echo "\n\033[1;36m⏱️  Running benchmark in normal mode: $name\033[0m\n"; \
            $bench 0; \
        done \
    '

benchmark-release:
    @just benchmarks="true" reconfigure compile
    @bash -c '\
        for bench in $(find {{build_dir}}/benchmarks -type f -executable -not -path "*.p/*"); do \
            name=$(basename $bench); \
            echo "\n\033[1;36m⏱️  Running benchmark in release mode: $name\033[0m\n"; \
            $bench 1; \
        done \
    '

# =====================
# Example commands
# =====================

# Build and list examples
examples:
    @just examples="true" reconfigure compile
    @echo ""
    @echo "\033[1;32mExamples built successfully ✅\033[0m"
    @echo "\033[1;34m═════════════════════════════════════════════════\033[0m"
    @echo "\033[1;33mAvailable Examples:\033[0m"
    @echo ""
    @bash -c 'find {{build_dir}}/examples -type f -executable -not -name "*.p" | sort | while read example; do \
        name=$(basename $example); \
        case $name in \
            basic_usage)          desc="Basic flags, string, and integer options" ;; \
            subcommands)          desc="Git-like subcommands implementation" ;; \
            positional_args)      desc="Handling positional arguments" ;; \
            variadic_positional)  desc="Handling variadic positional arguments" ;; \
            custom_handlers)      desc="Creating custom option handlers" ;; \
            advanced_options)     desc="Dependencies, conflicts, and exclusive groups" ;; \
            nested_commands)      desc="Complex nested subcommands (like git or docker)" ;; \
            validators)           desc="Built-in and custom validators for options" ;; \
            multi_values)         desc="Handling multiple values for an option" ;; \
            env_vars)             desc="Reading options from environment variables" ;; \
            regex)                desc="Using regular expressions for option values" ;; \
            *)                    desc="Example program" ;; \
        esac; \
        printf "  \033[1;36m%-20s\033[0m %s\n" "$name" "$desc"; \
    done'
    @echo ""
    @echo "\033[1;33mHow to run:\033[0m"
    @echo "  \033[1;37m{{build_dir}}/examples/basic_usage --help\033[0m"
    @echo "  \033[1;37m{{build_dir}}/examples/nested_commands service create --help\033[0m"
    @echo "\033[1;34m═════════════════════════════════════════════════\033[0m"

# Code quality
format:
    @echo "Formatting code with clang-format..."
    @find source includes -name "*.c" -o -name "*.h" | xargs clang-format -i -style=file
    @echo "Code formatting complete ✅"

# Static analysis with clang-tidy
lint:
    @echo "Running static analysis with clang-tidy..."
    @find source -name "*.c" | xargs clang-tidy -p {{build_dir}} --config-file=.clang-tidy --quiet
    @echo "Static analysis complete ✅"

# Generate a detailed report of clang-tidy findings
lint-report:
    @echo "Generating detailed clang-tidy report..."
    @mkdir -p reports
    @find source -name "*.c" | xargs clang-tidy -p {{build_dir}} --config-file=.clang-tidy --export-fixes=reports/clang-tidy-fixes.yml
    @echo "\033[1;32mReport generated as reports/clang-tidy-fixes.yml ✅\033[0m"

# Apply automated fixes for clang-tidy issues
fix:
    @echo "Applying automated fixes with clang-tidy..."
    @find source -name "*.c" | xargs clang-tidy -p {{build_dir}} --config-file=.clang-tidy --fix
    @find source includes -name "*.c" -o -name "*.h" | xargs sed -i 's|<cstddef>|<stddef.h>|g'
    @find source includes -name "*.c" -o -name "*.h" | xargs sed -i 's|<cstdlib>|<stdlib.h>|g'
    @find source includes -name "*.c" -o -name "*.h" | xargs sed -i 's|<cstring>|<string.h>|g'
    @echo "\033[1;32mAutomated fixes applied ✅\033[0m"
    @echo "Note: Some fixes may require manual review."

# Check code quality without making changes
check: format
    @echo "Checking code quality..."
    @find source -name "*.c" | xargs clang-tidy -p {{build_dir}} --config-file=.clang-tidy --quiet --warnings-as-errors="*"
    @echo "\033[1;32mCode quality check passed ✅\033[0m"

# Installation
install:
    @meson install -C {{build_dir}}
    @echo "Installation complete ✅"

uninstall:
    @meson --internal uninstall -C {{build_dir}}
    @echo "Uninstallation complete ✅"

# Help function that lists available recipes
help:
    @just --list
