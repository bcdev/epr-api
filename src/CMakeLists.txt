# Copyright (c) 2013 Antonio Valentino <antonio.valentino@tiscali.it>


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


# targets
add_library(epr_api SHARED ${SOURCES})

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
install(TARGETS epr_api epr_api_static
        LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(FILES epr_api.h epr_ptrarray.h DESTINATION include)
