compile-flags := "-xc -Wall -std=c17 -Wextra -Wpedantic -pedantic -pedantic-errors -Wconversion -o build/test"

[private]
default: prepare-compile-flags-txt
  @just --list

prepare-compile-flags-txt:
    echo {{compile-flags}} | sed -e "s/ /\n/g" > compile_flags.txt

build-test:
    clang {{compile-flags}} http_parser.c test.c

test: build-test
    build/test
