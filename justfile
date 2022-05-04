
set shell := [
    'bash',
    '-o', 'errexit',
    '-o', 'nounset',
    '-o', 'pipefail',
    '-O', 'globstar',
    '-O', 'nullglob',
    '-c',
]

src := 'src'

# Run the application.
run:
    exec ./run.sh

# Configure `cmake`.
configure:
    exec ./configure.sh

# Format sources.
format dir=src:
    clang-format -i "{{dir}}"/*.{h,c,hpp,cpp}
alias fmt := format

# Clean build artifacts.
clean:
    rm -rf build
