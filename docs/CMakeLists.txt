# Copyright (c) 2013 Antonio Valentino <antonio.valentino@tiscali.it>


find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_command(
        OUTPUT doxygen_main_content.html footer.html header.html images ${CMAKE_BINARY_DIR}/src/epr_api.h
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/doxygen_main_content.html .
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/footer.html .
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/header.html .
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/images images
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/src/epr_api.h ${CMAKE_BINARY_DIR}/src/epr_api.h
        COMMENT "Copy template files and images")

    add_custom_target(doc ALL
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.txt
        DEPENDS ${CMAKE_BINARY_DIR}/src/epr_api.h doxygen_main_content.html footer.html header.html images
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )

    add_custom_target(
        clean_doc
        COMMAND ${CMAKE_COMMAND} -E remove_directory epr_c_api
        COMMENT "Clean HTML doc")

    #add_dependencies(clean clean_doc)

    install(
        DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/epr_c_api/
        DESTINATION share/doc/epr-api/html
        COMPONENT doc)
    install(
        DIRECTORY images
        DESTINATION share/doc/epr-api
        COMPONENT doc)

endif(DOXYGEN_FOUND)
