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
elif cc.get_id() == 'msvc'
    add_project_arguments('/wd4204', language: 'c')  # non-constant aggregate initializer
    add_project_arguments('/wd4028', language: 'c')  # formal parameter different from declaration
    add_project_arguments('/wd4996', language: 'c')  # deprecated functions
    add_project_arguments('/D_CRT_SECURE_NO_WARNINGS', language: 'c')
    
    if get_option('c_std') in ['c99', 'c11', 'gnu99', 'gnu11']
        add_project_arguments('/std:c11', language: 'c')
    endif
endif

is_windows = host_machine.system() == 'windows'
if is_windows
    add_project_arguments('-DARGUS_BUILD_SHARED', language: 'c')
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
    if is_windows
        # Try to find PCRE2 on Windows
        pcre2_dep = dependency('libpcre2-8', required: false, version: '>=10.00')
        if not pcre2_dep.found()
            pcre2_dep = dependency('pcre2', required: false, version: '>=10.00')
            if not pcre2_dep.found()
                # On Windows, allow fallback to disable regex if not found
                warning('PCRE2 not found on Windows, disabling regex support')
                regex = false
                pcre2_dep = dependency('', required: false)
            endif
        endif
    else
        # On Unix systems, require PCRE2 if regex is enabled
        pcre2_dep = dependency('libpcre2-8', required: true, version: '>=10.00')
        if not pcre2_dep.found()
            pcre2_dep = dependency('pcre2', required: true, version: '>=10.00')
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
    soversion: '0',
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
