# Benchmark build configuration

# Include directories
benchmark_includes = include_directories('../includes')

# Create benchmark executable
benchmark_modes_comparison = executable(
  'benchmark_modes_comparison',
  'benchmark_release_mode.c',
  dependencies: [argus_dep],
  include_directories: benchmark_includes
)
