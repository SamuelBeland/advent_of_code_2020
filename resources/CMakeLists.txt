set(RESOURCES_DIR "${CMAKE_CURRENT_BINARY_DIR}")

file(COPY "days" DESTINATION "${RESOURCES_DIR}")
file(GLOB FILES
    "${RESOURCES_DIR}/days/*"
)

foreach(FILE ${FILES})
    get_filename_component(FILE_NAME "${FILE}" NAME)
    get_filename_component(FILE_NAME_WE "${FILE}" NAME_WE)

    # days input files
    set(ENTRY "static constexpr char const * ${FILE_NAME_WE} = \"${RESOURCES_DIR}/days/${FILE_NAME}\";")
    if(DAYS_INPUTS)
        set(DAYS_INPUTS "${DAYS_INPUTS}\n    ${ENTRY}")
    else()
        set(DAYS_INPUTS "${ENTRY}")
    endif()

    # days functions
    string(TOLOWER "${FILE_NAME_WE}" LOWER_FILE_NAME_WE)
    set(ENTRY "void ${LOWER_FILE_NAME_WE}(char const * input);")
    if(DAYS_FUNCTIONS)
        set(DAYS_FUNCTIONS "${DAYS_FUNCTIONS}\n${ENTRY}")
    else()
        set(DAYS_FUNCTIONS "${ENTRY}")
    endif()
endforeach()

file(COPY "tests" DESTINATION "${RESOURCES_DIR}")
file(GLOB FILES "${RESOURCES_DIR}/tests/*")
foreach(FILE ${FILES})
    get_filename_component(FILE_NAME "${FILE}" NAME)
    get_filename_component(FILE_NAME_WE "${FILE}" NAME_WE)

    # tests input files
    set(ENTRY "static constexpr char const * ${FILE_NAME_WE} = \"${RESOURCES_DIR}/tests/${FILE_NAME}\";")
    if(TESTS_INPUTS)
        set(TESTS_INPUTS "${TESTS_INPUTS}\n    ${ENTRY}")
    else()
        set(TESTS_INPUTS "${ENTRY}")
    endif()
endforeach()

configure_file(resources.hpp.in resources.hpp @ONLY)