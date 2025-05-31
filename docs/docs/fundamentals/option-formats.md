# Option Formats

Learn all the ways users can specify options when running your Argus-powered programs.

## Overview

Argus supports flexible command-line formats, allowing users to specify options in multiple ways. This flexibility accommodates different user preferences and command-line conventions.

## Basic Format Patterns

| Pattern | Description | Example |
|---------|-------------|---------|
| **Long options** | Double dash + name | `--verbose`, `--output=file.txt` |
| **Short options** | Single dash + character | `-v`, `-o file.txt` |
| **Combined short** | Multiple flags together | `-vxf` (same as `-v -x -f`) |
| **Positional** | No prefix, order matters | `input.txt output.txt` |
| **Option terminator** | `--` ends option parsing | `program -- --not-an-option` |

## Long Options Format

**Long options** use descriptive names with double dashes (`--`).

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs>
<TabItem value="space" label="Space Separated" default>

```bash
# Value after space
./program --output result.txt
./program --port 8080
./program --quality 0.8
./program --debug true
```

**When to use:** Most readable, works with all option types.

</TabItem>
<TabItem value="equals" label="Equals Syntax">

```bash
# Value after equals sign
./program --output=result.txt
./program --port=8080
./program --quality=0.8
./program --debug=true
```

**When to use:** Prevents ambiguity, especially with optional arguments.

</TabItem>
<TabItem value="flags" label="Flags Only">

```bash
# Flags need no values
./program --verbose --force --dry-run
./program --verbose --force --dry-run input.txt
```

**Note:** Flags never take values - their presence activates them.

</TabItem>
</Tabs>

## Short Options Format

**Short options** use single characters with single dashes (`-`).

<Tabs>
<TabItem value="separate" label="Separate Arguments" default>

```bash
# Each option separate
./program -v -o result.txt -p 8080
./program -v -f -o output.txt input.txt
```

**When to use:** Clear separation, works with all types.

</TabItem>
<TabItem value="attached" label="Attached Values">

```bash
# Value attached directly (no space)
./program -oresult.txt -p8080
./program -vf -ooutput.txt input.txt
```

**When to use:** Compact syntax, common in Unix tools.

</TabItem>
<TabItem value="combined" label="Combined Flags">

```bash
# Multiple flags combined
./program -vxf           # Same as -v -x -f
./program -vxo output.txt # Same as -v -x -o output.txt
./program -vxooutput.txt  # Same as -v -x -o output.txt
```

**Rules:**
- Only **flags** can be combined
- **Last option** can take a value
- Value can be attached or separate

</TabItem>
</Tabs>

## Mixed Format Examples

Users can mix different formats in the same command:

```bash
# All equivalent commands
./program --verbose --output=result.txt input.txt
./program -v --output result.txt input.txt  
./program --verbose -o result.txt input.txt
./program -v -oresult.txt input.txt
./program -vo result.txt input.txt
./program -voresult.txt input.txt
```

## Positional Arguments

**Positional arguments** are provided without option prefixes, processed in order.

### Basic Positional Usage

```bash
# Required positionals
./program input.txt output.txt

# Mixed with options (any order)
./program --verbose input.txt --output-format json output.txt
./program input.txt --verbose output.txt --format json
```

### Optional Positionals

```bash
# Optional positionals can be omitted
./program input.txt              # Uses default for optional args
./program input.txt output.txt   # Provides optional arg
./program input.txt output.txt 1024  # Provides all args
```

## Special Cases and Edge Cases

### Negative Numbers

Argus intelligently handles negative numbers vs options:

```bash
# These are treated as values, not options
./program --threshold -5.2
./program --coordinates -10 -20
./program input.txt -42    # Positional negative number
```

**Rule:** If a positional option expects a number, `-123` is treated as a negative number, not an option.

### Option Terminator (`--`)

The double dash `--` stops option parsing - everything after is positional:

```bash
# Everything after -- is positional
./program --verbose -- --not-an-option file-with-dashes.txt
./program -v -- -x -y -z   # -x -y -z are positional arguments
```

**Use cases:**
- Files starting with dashes
- Passing arguments to sub-processes
- Avoiding option conflicts

### Ambiguous Cases

When input could be interpreted multiple ways:

```bash
# Ambiguous: is "file.txt" a value for --output or a positional?
./program --output file.txt another.txt

# Unambiguous with equals
./program --output=file.txt another.txt

# Unambiguous with --
./program --output -- file.txt another.txt
```

## Collection Formats

For array and map options, Argus supports multiple input methods:

### Array Options

<Tabs>
<TabItem value="multiple" label="Multiple Occurrences" default>

```bash
# Each occurrence adds to array
./program --tags web --tags api --tags backend
./program -t web -t api -t backend
```

</TabItem>
<TabItem value="comma" label="Comma Separated">

```bash
# Comma-separated in single option
./program --tags=web,api,backend
./program -tweb,api,backend
```

</TabItem>
<TabItem value="mixed" label="Mixed Approach">

```bash
# Combine both methods
./program --tags=web,api --tags=backend
./program -tweb,api -tbackend
```

</TabItem>
<TabItem value="ranges" label="Integer Ranges">

```bash
# Special syntax for integer arrays
./program --ids=1-5,10,15-20
# Expands to: 1,2,3,4,5,10,15,16,17,18,19,20

./program --ports=8000-8005,9000
# Expands to: 8000,8001,8002,8003,8004,8005,9000
```

</TabItem>
</Tabs>

### Map Options

<Tabs>
<TabItem value="kv-multiple" label="Multiple Key-Value" default>

```bash
# Each occurrence adds key-value pair
./program --env USER=alice --env HOME=/home/alice
./program -e USER=alice -e HOME=/home/alice
```

</TabItem>
<TabItem value="kv-comma" label="Comma Separated">

```bash
# Comma-separated pairs
./program --env=USER=alice,HOME=/home/alice,TERM=xterm
./program -eUSER=alice,HOME=/home/alice
```

</TabItem>
<TabItem value="kv-types" label="Different Value Types">

```bash
# String map
./program --env=DEBUG=true,LOG_LEVEL=info

# Integer map  
./program --ports=http=80,https=443,ftp=21

# Boolean map (accepts various formats)
./program --features=debug=true,cache=no,logging=1,metrics=off
```

</TabItem>
</Tabs>

## Real-World Examples

Here are complete command-line examples showing different patterns:

### Simple File Processor

```bash
# Basic usage
./file-processor input.txt

# With options  
./file-processor --verbose --output=/tmp input.txt
./file-processor -v -o /tmp input.txt
./file-processor -vo /tmp input.txt

# Complex example
./file-processor -v --format=json --threads=4 input.txt output.txt
```

### Git-Style Tool

```bash
# Subcommands with options
./tool add --force file.txt
./tool commit --message="Fix bug" --author="John Doe"
./tool push --tags --dry-run origin main

# Mixed formats
./tool add -f file.txt
./tool commit -m "Fix bug" --author="John Doe"
```

### Build Tool

```bash
# Array and map options
./builder --targets=lib,bin,tests --env=DEBUG=1,OPT=2
./builder -t lib,bin -t tests -e DEBUG=1 -e OPT=2

# With option terminator
./builder --verbose -- --enable-debug --target=all
```

## Format Validation

Argus automatically validates option formats and provides helpful errors:

```bash
# Missing value
$ ./program --output
program: Missing value for option: '--output'

# Invalid format  
$ ./program --port=abc
program: Invalid value 'abc': expected number

# Unknown option
$ ./program --unknow
program: Unknown option: '--unknow'
```

## Best Practices for Users

### Recommended Patterns

```bash
# ✅ Clear and readable
./program --verbose --output=result.txt input.txt

# ✅ Compact but clear
./program -vo result.txt input.txt

# ✅ Unambiguous with complex args
./program --config=app.conf -- input-with-dashes.txt
```

### Avoid Confusion

```bash
# ❌ Potentially confusing
./program --output result.txt result2.txt  # Which is positional?

# ✅ Better alternatives
./program --output=result.txt result2.txt
./program --output result.txt -- result2.txt
```

## Compatibility Notes

Argus format support is designed to be compatible with common CLI conventions:

- **GNU-style long options**: `--option=value`
- **POSIX-style short options**: `-o value`, `-ovalue`
- **BSD-style flag combining**: `-abc`
- **Git-style subcommands**: `program command --option`

This ensures users familiar with standard Unix tools will find Argus programs intuitive to use.

## What's Next?

Now that you understand how users can format their commands, learn how to:

- **[Accessing Values](accessing-values)** - Retrieve user input in your code
- **[Validation](../features/validation)** - Ensure users provide correct input  
- **[Subcommands](../features/subcommands)** - Build Git-like command hierarchies
- **[Collections](../features/collections)** - Handle arrays and maps in detail

:::tip Pro Tip
The format flexibility is automatic - you don't need to code for different formats. Define your options once, and Argus handles all these input variations!
:::
