# A script that gets version information from specified
# git repositiry and writes that down as a header file.

# Get the description (tag-no_of_commits-dirty)
execute_process(
  COMMAND git describe --tags --abbrev=0 --dirty --candidates=0
  WORKING_DIRECTORY ${GIT_DIR}
  OUTPUT_VARIABLE GIT_DESCRIPTION
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the current working branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${GIT_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${GIT_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

string(REGEX MATCH "^release-([0-9]+)\\.([0-9]+)\\.([0-9]+)-?([^-]*)?-?([^-]*)?$" MULTY_CORE_VERSION_STRING "${GIT_DESCRIPTION}")
if (CMAKE_MATCH_COUNT)
    set(MULTY_CORE_VERSION_MAJOR ${CMAKE_MATCH_1})
    set(MULTY_CORE_VERSION_MINOR ${CMAKE_MATCH_2})
    set(MULTY_CORE_VERSION_BUILD ${CMAKE_MATCH_3})
    set(MULTY_CORE_VERSION_NOTE ${CMAKE_MATCH_4})
    set(MULTY_CORE_VERSION_STATUS ${CMAKE_MATCH_5})
else()
    message(WARNING "Failed to obtain version information from git, using dummy values.")
    set(MULTY_CORE_VERSION_MAJOR 0)
    set(MULTY_CORE_VERSION_MINOR 0)
    set(MULTY_CORE_VERSION_BUILD 0)
    set(MULTY_CORE_VERSION_NOTE "dummy")
    set(MULTY_CORE_VERSION_STATUS "?")
endif()

set(MULTY_CORE_VERSION_COMMIT "${GIT_BRANCH}:${GIT_COMMIT_HASH}")
if(MULTY_CORE_VERSION_STATUS)
    string(CONCAT MULTY_CORE_VERSION_COMMIT ${MULTY_CORE_VERSION_COMMIT} "-" ${MULTY_CORE_VERSION_STATUS})
endif()

set(MULTY_CORE_VERSION
    "${MULTY_CORE_VERSION_MAJOR}.${MULTY_CORE_VERSION_MINOR}.${MULTY_CORE_VERSION_BUILD}-${MULTY_CORE_VERSION_NOTE} on ${MULTY_CORE_VERSION_COMMIT}"
)

if (IN_FILE AND OUT_FILE)
    configure_file(
        "${IN_FILE}"
        "${OUT_FILE}"
    )
endif()
