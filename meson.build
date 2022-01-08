project('concept', 'cpp',
        version : '0.0.1',
        default_options : ['cpp_std=c++20', 'warning_level=3'])

# Dependencies
fmt_dep = dependency('fmt')

# Executables
sources = ['main.cpp', 'src/bitset.cpp', 'src/csv.cpp', 'src/concept.cpp']
executable('concept', sources,
           dependencies: fmt_dep)