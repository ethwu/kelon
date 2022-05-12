
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
    ./run.sh

# Configure `cmake`.
configure:
    ./configure.sh

# Format sources.
format dir=src:
    for i in $(fd -e h -e hpp -e c -e cpp '' "{{dir}}") ; do clang-format -i "$i" ; done
alias fmt := format

# Update `alloinit` and `al_ext`.
update:
    ./alloinit -l . -u

# Reset `alloinit` and `al_ext`.
reset-deps:
    rm -rf allolib al_ext
    ./alloinit -l . -L

# Clean build artifacts.
clean:
    rm -rf bin build
