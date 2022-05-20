
# Directory containing header files.
headers := include/
# Directory containing source files.
sources := src/

# Run the application.
.PHONY: run
run: configure
	scripts/run.sh

# Ensure that `cmake` has been configured.
.PHONY: configure
configure:
	# Configure the project if any source files have been added or removed.
	touch .sources && mv .sources .sources.tmp
	scripts/list.sh "${headers}" "${sources}" > .sources
	diff .sources.tmp .sources || scripts/configure.sh
	rm .sources.tmp

# Alias for `configure`.
.PHONY: conf
conf: configure

# Format sources.
.PHONY: format
format:
	scripts/format.sh "${headers}" "${sources}"

# Alias for `format`.
.PHONY: fmt
fmt: format

# Update `alloinit` and `al_ext`.
.PHONY: update
update:
	utils/alloinit -u .

# Reset `alloinit` and `al_ext`.
.PHONY: reset-deps
reset-deps:
	rm -rf allolib/ al_ext/
	utils/alloinit -S -l . -L

# Clean build artifacts.
clean:
	rm -rf bin/ build/
