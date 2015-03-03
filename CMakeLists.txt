cmake_minimum_required(VERSION 2.8.4)
project(fptest)

option(UF_WARNINGS "Enable highest warnings for most ocmpilers" ON)
option(UF_EVALUATION "Set the FTL_EVAL_MODE" ON)

IF(UF_WARNINGS)
    IF(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
        # Use the highest warning level for visual studio.
        IF(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
            STRING(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        ELSE(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
        ENDIF(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        IF(CMAKE_C_FLAGS MATCHES "/W[0-4]")
            STRING(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        ELSE(CMAKE_C_FLAGS MATCHES "/W[0-4]")
            SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
        ENDIF(CMAKE_C_FLAGS MATCHES "/W[0-4]")
        # Disable deprecation warnings for standard C functions in VS2005 and later
        IF(CMAKE_COMPILER_2005)
            ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
        ENDIF(CMAKE_COMPILER_2005)
    ENDIF(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")

    IF(CMAKE_BUILD_TOOL MATCHES "make")
        IF(NOT CMAKE_CXX_FLAGS MATCHES "-Wall") #C++
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
        ENDIF(NOT CMAKE_CXX_FLAGS MATCHES "-Wall")
        IF(NOT CMAKE_C_FLAGS MATCHES "-Wall")   #C
            SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
        ENDIF(NOT CMAKE_C_FLAGS MATCHES "-Wall")
    ENDIF(CMAKE_BUILD_TOOL MATCHES "make")
ENDIF(UF_WARNINGS)

IF(UF_EVALUATION)
    IF(MSVC)
        IF(MSVC_VERSION GREATER 1600)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /arch:SSE")
        ENDIF(MSVC_VERSION GREATER 1600)
    ELSEIF(MINGW)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -mfpmath=sse -ffloat-store -msse2")
    ENDIF(MSVC)
    add_definitions("-DUF_FLOAT_EVALUATION")
ENDIF(UF_EVALUATION)

add_definitions("-DCOMPILER_ARGS=\"${CMAKE_C_FLAGS}\"")
add_definitions("-DCOMPILER_NAME=\"${CMAKE_C_COMPILER}\"")

set(SOURCE_FILES main.c)
add_executable(fptest ${SOURCE_FILES})

