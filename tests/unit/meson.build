# Unit tests
unit_tests = [
  ['strings', 'test_utils/test_strings.c'],
  ['value_utils', 'test_utils/test_value_utils.c'],
  ['option_lookup', 'test_utils/test_option_lookup.c'],
  ['multi_values', 'test_utils/test_multi_values.c'],
  ['handlers', 'test_callbacks/test_handlers.c'],
  ['validators', 'test_callbacks/test_validators.c'],
  ['smart_hint', 'test_smart_hint.c'],
]

foreach test : unit_tests
  test_exe = executable(
    'test_' + test[0],
    test[1],
    dependencies: [criterion_dep, argus_dep],
    include_directories: test_includes,
    c_args: test_args
  )
  
  test(
    'unit_' + test[0],
    test_exe,
    env: test_env,
    timeout: 30
  )
endforeach

debug_mode_test_sources = files('test_core/test_debug_mode.c')

# Build and run the test in debug mode (validation enabled)
test_debug_mode_do_validation = executable(
  'test_debug_mode_do_validation',
  debug_mode_test_sources,
  dependencies: [criterion_dep, argus_dep],
  include_directories: test_includes,
  c_args: test_args
)

test(
  'unit_debug_mode_do_validation',
  test_debug_mode_do_validation,
  env: test_env,
  timeout: 30
)

# Build and run the test in standard mode (validation disabled)
test_debug_skip_validation = executable(
  'test_debug_mode_skip_validation',
  debug_mode_test_sources,
  dependencies: [criterion_dep, argus_dep],
  include_directories: test_includes,
  c_args: test_args + ['-DARGUS_DEBUG'],
)

test(
  'unit_debug_mode_skip_validation',
  test_debug_skip_validation,
  env: test_env,
  timeout: 30
)
