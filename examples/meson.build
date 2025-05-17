# List of example programs
example_programs = [
  'basic_usage',
  'subcommands',
  'positional_args',
  'custom_handlers',
  'advanced_options',
  'nested_commands',
  'validators',
  'multi_values',
  'env_vars',
  'regex',
]

fs = import('fs')

# Build each example
foreach example : example_programs
  src_file = meson.current_source_dir() / (example + '.c')
  if fs.exists(src_file)
    executable(
      example,
      example + '.c',
      dependencies: argus_dep,
      install: false
    )
  endif
endforeach
