# Copyright (c) 2013-2014 Antonio Valentino <antonio.valentino@tiscali.it>


# sources
set(SOURCES epr_api.c
            epr_core.c
            epr_dddb.c
            epr_product.c
            epr_dataset.c
            epr_string.c
            epr_ptrarray.c
            epr_field.c
            epr_record.c
            epr_param.c
            epr_dsd.c
            epr_msph.c
            epr_swap.c
            epr_band.c
            epr_bitmask.c
            epr_dump.c
            epr_typconv.c
)

if(NOT DISABLE_SYMBOL_CONTROL)
    #if(CMAKE_C_COMPILER_ID STREQUAL GNU)
    if(NOT APPLE AND NOT MSVC)
        # Assume to work with the GNU linker
        # On some GNU/Linux distribution it is possible to use both GCC and
        # Clang as compilers
        set(SYMBOL_LIST ${CMAKE_CURRENT_SOURCE_DIR}/epr_api.ver)
        set(CMAKE_SHARED_LINKER_FLAGS
            "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--version-script=${SYMBOL_LIST}")
    endif(NOT APPLE AND NOT MSVC)

    if(APPLE)
        set(SYMBOL_LIST ${CMAKE_CURRENT_SOURCE_DIR}/epr_api.exported)
        set(CMAKE_SHARED_LINKER_FLAGS
            "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-exported_symbols_list,${SYMBOL_LIST}")
    endif(APPLE)

    #if(MSVC)
    #    set(SYMBOL_LIST ${CMAKE_CURRENT_SOURCE_DIR}/epr_api.def)
    #    set(CMAKE_SHARED_LINKER_FLAGS
    #        ${CMAKE_SHARED_LINKER_FLAGS} /DEF:${SYMBOL_LIST})
    #endif(MSVC)
endif(NOT DISABLE_SYMBOL_CONTROL)

if(CMAKE_C_COMPILER_ID STREQUAL GNU OR CMAKE_C_COMPILER_ID STREQUAL Clang)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c89")
endif(CMAKE_C_COMPILER_ID STREQUAL GNU OR CMAKE_C_COMPILER_ID STREQUAL Clang)

# targets
add_library(epr_api SHARED ${SOURCES})
set_target_properties(epr_api PROPERTIES
    VERSION ${EPR_API_VERSION_STRING}
    SOVERSION ${EPR_API_SOVERSION})
if(NOT MSVC)
    target_link_libraries(epr_api m)
endif()


if(BUILD_STATIC_LIB)
    add_library(epr_api_static STATIC ${SOURCES})
    set_target_properties(epr_api_static PROPERTIES OUTPUT_NAME epr_api)
endif(BUILD_STATIC_LIB)


# sub-directories
if(BUILD_TESTS)
    add_subdirectory(api-test)
    add_subdirectory(test)
endif(BUILD_TESTS)


# install
install(FILES epr_api.h epr_ptrarray.h DESTINATION include COMPONENT lib)
install(TARGETS epr_api DESTINATION lib COMPONENT dev)
if(BUILD_STATIC_LIB)
    install(TARGETS epr_api_static ARCHIVE DESTINATION lib COMPONENT dev)
endif(BUILD_STATIC_LIB)
