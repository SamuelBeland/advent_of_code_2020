set(INPUTS_DIR "${CMAKE_BINARY_DIR}/inputs")

file(COPY "days" DESTINATION "${INPUTS_DIR}")
file(GLOB INPUT_DAYS_FILES
    "${INPUTS_DIR}/days/*"
)

foreach(INPUT_DAY_FILE ${INPUT_DAYS_FILES})
    get_filename_component(DEFAULT_INPUT_FILE "${INPUT_DAY_FILE}" NAME)
    set(DEFAULT_INPUT_FILE "inputs/days/${DEFAULT_INPUT_FILE}")
    get_filename_component(FUNCTION_NAME "${INPUT_DAY_FILE}" NAME_WE)
    string(TOLOWER "day_${FUNCTION_NAME}" FUNCTION_NAME)

    # days functions
    set(ENTRY "std::string ${FUNCTION_NAME}(char const * input);")
    if(DAYS_FUNCTIONS)
        set(DAYS_FUNCTIONS "${DAYS_FUNCTIONS}\n${ENTRY}")
    else()
        set(DAYS_FUNCTIONS "${ENTRY}")
    endif()

    # days functions array
    set(ENTRY "    {\n        \"${FUNCTION_NAME}\",\n        ${FUNCTION_NAME},\n        \"${DEFAULT_INPUT_FILE}\"\n    }")
    if (DAYS_DATA)
        set(DAYS_DATA "${DAYS_DATA}, ${ENTRY}")
    else()
        list(LENGTH INPUT_DAYS_FILES DAYS_FUNCTION_COUNT)
        set(DAYS_DATA "static constexpr std::array<Day, ${DAYS_FUNCTION_COUNT}> DAYS_ALL{\n${ENTRY}")
    endif()
endforeach()
set(DAYS_DATA "${DAYS_DATA} };")

file(COPY "tests" DESTINATION "${INPUTS_DIR}")
file(GLOB FILES "${INPUTS_DIR}/tests/*")
foreach(FILE ${FILES})
    get_filename_component(FILE_NAME "${FILE}" NAME)
    get_filename_component(FILE_NAME_WE "${FILE}" NAME_WE)

    # tests input files
    string(TOUPPER "${FILE_NAME_WE}" VAR_NAME)
    set(ENTRY "static constexpr char const * ${VAR_NAME} = \"${RESOURCES_DIR}/tests/${FILE_NAME}\";")
    if(TESTS_INPUTS)
        set(TESTS_INPUTS "${TESTS_INPUTS}\n    ${ENTRY}")
    else()
        set(TESTS_INPUTS "${ENTRY}")
    endif()
endforeach()

configure_file(resources.hpp.in resources.hpp @ONLY)