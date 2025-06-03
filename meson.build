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
    meson_version: '>=1.1.0',
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
    # if cc.get_id() == 'gcc'
    #     add_project_link_arguments('-static-libgcc', language: 'c')
    # endif
endif

inc_dirs = include_directories('includes')

subdir('source')

regex = get_option('regex')
if not regex
    pcre2_dep = dependency('', required: false)
else
    add_project_arguments('-DARGUS_REGEX', language: 'c')
    
    # Essayer d'abord de trouver PCRE2 via pkg-config (système)
    pcre2_dep = dependency('libpcre2-8', required: false)
    
    # Si non trouvé, utiliser le subproject
    if not pcre2_dep.found()
        pcre2_project = subproject('pcre2', default_options: ['werror=false'])
        pcre2_dep = pcre2_project.get_variable('libpcre2_8')
    endif
endif

argus_lib = both_libraries(
    'argus',
    argus_sources,
    include_directories: inc_dirs,
    dependencies: regex ? [pcre2_dep] : [],
    version: meson.project_version(),
    soversion: '1',
    install: true,
    vs_module_defs: 'argus.def',
)

argus_dep = declare_dependency(
    link_with: argus_lib,
    include_directories: inc_dirs,
    dependencies: regex ? [pcre2_dep] : [],
)

install_headers('includes/argus.h')
install_subdir(
    'includes/argus',
    install_dir: get_option('includedir'),
    exclude_directories: ['internal']
)

if get_option('tests')
    subdir('tests')
endif

if get_option('examples')
    subdir('examples')
endif

if get_option('benchmarks')
    subdir('benchmarks')
endif
