build config="release":
    cmake -B build/{{config}} -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE={{capitalize(config)}}
    cmake --build build/{{config}} --parallel

test:
    cmake -B build -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Debug -DAETHER_SANITIZE=ON
    cmake --build build --parallel
    ctest --test-dir build --output-on-failure -V
    python3 scripts/uci_e2e.py "{{justfile_directory()}}/bin/Debug/aether"

lint:
    clang-format -i $(find src tests -name '*.[ch]' | sort)
    cppcheck -j$(nproc) --enable=portability src/*.c tests/*.c
