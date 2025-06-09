# Functional tests
functional_tests = [
  ['api', 'test_api.c'],
  ['basic_usage', 'test_basic_usage.c'],
  ['multi_values', 'test_multi_values.c'],
  ['environments', 'test_env.c'],
]

foreach test : functional_tests
  test_exe = executable(
    'test_' + test[0],
    test[1],
    dependencies: [criterion_dep, argus_dep],
    include_directories: test_includes,
    c_args: test_args
  )
  
  test(
    'functional_' + test[0],
    test_exe,
    env: test_env,
    timeout: 60
  )
endforeach
