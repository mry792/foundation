include(GNUInstallDirs)

###
# Library definition:
###

add_library(${PROJECT_NAME} INTERFACE)
file(
    GLOB_RECURSE
    source_files
    RELATIVE .
    CONFIGURE_DEPENDS
    "."
)
target_sources(${PROJECT_NAME} INTERFACE ${source_files})
target_include_directories(
    ${PROJECT_NAME}
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

###
# Install library:
###

# For when concumed as a subdirectory.
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
