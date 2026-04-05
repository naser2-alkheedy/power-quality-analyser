cmake_minimum_required(VERSION 3.15)
project(power_quality_analyser C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Enable all warnings for clean, professional code
add_compile_options(-Wall -Wextra -Wpedantic)

# Define the executable and list all source files
add_executable(power_quality_analyser
    main.c
    waveform.c
    io.c
)

# Link the math library (required for sqrt() in waveform.c)
target_link_libraries(power_quality_analyser m)
