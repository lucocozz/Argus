project(
    'argus', 'c',
    version: '1.0.0',
    license: 'MIT',
    default_options: [
        'warning_level=3',
        'werror=true',
        'c_std=gnu11',
        'buildtype=debugoptimized',
    ],
    meson_version: '>=1.0.0',
)

cc = meson.get_compiler('c')

if cc.get_id() == 'clang'
    add_project_arguments('-Wno-gnu-zero-variadic-macro-arguments', language: 'c')
elif cc.get_id() == 'gcc'
    add_project_arguments('-Wno-variadic-macros', language: 'c')
endif

is_windows = host_machine.system() == 'windows'
if is_windows
    add_project_arguments('-DARGUS_BUILDING', language: 'c')
    if cc.get_id() == 'gcc'
        add_project_link_arguments('-static-libgcc', language: 'c')
    endif
endif


# Include directories
inc_dirs = include_directories('includes')

# Build library
subdir('source')

# Handle regex dependency
regex = get_option('regex')
if not regex
    pcre2_dep = dependency('', required: false)
else
    add_project_arguments('-DARGUS_REGEX', language: 'c')
    
    # Custom PCRE2 path handling (useful for Windows/vcpkg)
    pcre2_include_dir = get_option('pcre2_include_dir')
    pcre2_lib_dir = get_option('pcre2_lib_dir')
    
    if pcre2_include_dir != '' and pcre2_lib_dir != ''
        message('Using explicitly provided PCRE2 paths')
        message('Include dir: ' + pcre2_include_dir)
        message('Library dir: ' + pcre2_lib_dir)
        pcre2_lib = cc.find_library('pcre2-8', dirs: pcre2_lib_dir)
        if pcre2_lib.found()
            pcre2_dep = declare_dependency(
                compile_args: ['-I' + pcre2_include_dir],
                dependencies: pcre2_lib
            )
        else
            error('PCRE2 library not found in specified directory')
        endif
    else
        # Try standard dependency resolution
        pcre2_dep = dependency(['libpcre2-8', 'pcre2'], required: false, version: '>=10.00')
        if not pcre2_dep.found()
            error('PCRE2 dependency is required for regex support.')
        endif
    endif
endif

# Create both static and shared libraries from sources
argus_lib = both_libraries(
    'argus',
    argus_sources,
    include_directories: inc_dirs,
    dependencies: regex ? [pcre2_dep] : [],
    version: meson.project_version(),
    soversion: '1',
    install: true,
)

# Create dependency object for internal usage
argus_dep = declare_dependency(
    link_with: argus_lib,
    include_directories: inc_dirs,
    dependencies: regex ? [pcre2_dep] : [],
)

# Install headers (public API only)
install_headers('includes/argus.h')
install_subdir(
    'includes/argus',
    install_dir: get_option('includedir'),
    exclude_directories: ['internal']
)

# Tests if enabled
if get_option('tests')
    subdir('tests')
endif

# Examples if enabled
if get_option('examples')
    subdir('examples')
endif

# Benchmarks if enabled
if get_option('benchmarks')
    subdir('benchmarks')
endif
