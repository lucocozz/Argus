# Integration tests
integration_tests = [
  ['parsing', 'test_parsing.c'],
  ['subcommands', 'test_subcommands.c'],
  ['validators', 'test_validators.c'],
  ['post_validation', 'test_post_validation.c'],
  ['validation', 'test_validation.c'],
  ['edge_case', 'test_edge_case.c'],
  ['positional_edge_case', 'test_positional_edge_case.c'],
  ['subcommand_edge_case', 'test_subcommand_edge_case.c'],
  ['variadic_positional', 'test_variadic_positional.c']
]

foreach test : integration_tests
  test_exe = executable(
    'test_' + test[0],
    test[1],
    dependencies: [criterion_dep, argus_dep],
    include_directories: test_includes,
    c_args: test_args
  )
  
  test(
    'integration_' + test[0],
    test_exe,
    env: test_env,
    timeout: 60
  )
endforeach
