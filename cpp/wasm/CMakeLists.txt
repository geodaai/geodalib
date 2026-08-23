set (WASM_SOURCE_FILES
        ${GEODA_SOURCE_FILES}
        ${CLAPACK_SRC}
        wasm.cpp)

set (CORE_WASM_FILES
        ${CORE_SOURCE_FILES}
        wasm-core.cpp)

set (LISA_WASM_FILES
        ${LISA_SOURCE_FILES}
        ${WEIGHTS_SOURCE_FILES}
        wasm-lisa.cpp)

set (REGRESSION_WASM_FILES
        ${REGRESSION_SOURCE_FILES}
        ${WEIGHTS_SOURCE_FILES}
        ${CLAPACK_SRC}
        wasm-regression.cpp)

if(USE_EMCC)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    set(CMAKE_C_COMPILER emcc)
    set(CMAKE_CPP_COMPILER em++)
    set(CMAKE_CXX_COMPILER em++)
    unset(_CMAKE_APPLE_ARCHS_DEFAULT)

    #set(CMAKE_CXX_FLAGS "-g --bind -s ASSERTIONS=1 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1")
    set(CMAKE_CXX_FLAGS "-O3 -DNDEBUG")
    #set(CMAKE_CXX_FLAGS "-g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_BOOST_HEADERS=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --bind -s WASM=1 -s MAXIMUM_MEMORY=3072MB -s ALLOW_MEMORY_GROWTH=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s NO_FILESYSTEM=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s MODULARIZE=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s 'EXPORT_NAME=geoda'")
    # Avoid emitting BigInt literals so the generated glue stays compatible with the
    # es2017 esbuild target used to bundle the WASM into the JS package.
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s WASM_BIGINT=0")
endif()

# Function to configure common target settings
function(configure_wasm_target target_name source_files)
    add_executable(${target_name} ${source_files})
    target_compile_definitions(${target_name} PRIVATE __EMCC__=1)
    target_compile_definitions(${target_name} PRIVATE __JSGEODA__=1)
    target_compile_definitions(${target_name} PRIVATE __NO_THREAD__=1)
    target_compile_definitions(${target_name} PRIVATE EMCC_DEBUG=0)
    target_include_directories(${target_name} PUBLIC ${CMAKE_SOURCE_DIR}/cpp ${CLAPACK_SRC_DIR}/INCLUDE)
endfunction()

# Configure main geoda target
set(TARGET_NAME geoda)
configure_wasm_target(${TARGET_NAME} "${WASM_SOURCE_FILES}")

# Configure core target
set(CORE_TARGET_NAME geoda-core)
configure_wasm_target(${CORE_TARGET_NAME} "${CORE_WASM_FILES}")

# Configure lisa target
set(LISA_TARGET_NAME geoda-lisa)
configure_wasm_target(${LISA_TARGET_NAME} "${LISA_WASM_FILES}")

# Configure regression target
set(REGRESSION_TARGET_NAME geoda-regression)
configure_wasm_target(${REGRESSION_TARGET_NAME} "${REGRESSION_WASM_FILES}")
