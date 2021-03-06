include(ExternalProject)

if(NOT DJV_THIRD_PARTY_DISABLE_BUILD)
    ExternalProject_Add(
        OpenNURBSThirdParty
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/OpenNURBS
        DEPENDS ZLIBThirdParty
        GIT_REPOSITORY https://github.com/darbyjohnston/opennurbs
        GIT_TAG origin/zlib
        CMAKE_ARGS
            -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
            -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
            -DBUILD_SHARED_LIBS=${OpenNURBS_SHARED_LIBS}
            -DZLIB_SHARED_LIBS=${ZLIB_SHARED_LIBS})
endif()

set(OpenNURBS_FOUND TRUE)
set(OpenNURBS_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/opennurbs)
set(OpenNURBS_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/opennurbs${CMAKE_STATIC_LIBRARY_SUFFIX})
set(OpenNURBS_LIBRARIES ${OpenNURBS_LIBRARY} ${ZLIB_LIBRARIES})

if(OpenNURBS_FOUND AND NOT TARGET OpenNURBS::OpenNURBS)
    add_library(OpenNURBS::OpenNURBS UNKNOWN IMPORTED)
    add_dependencies(OpenNURBS::OpenNURBS OpenNURBSThirdParty)
    set_target_properties(OpenNURBS::OpenNURBS PROPERTIES
        IMPORTED_LOCATION "${OpenNURBS_LIBRARY}"
        INTERFACE_LINK_LIBRARIES "ZLIB;Shlwapi"
        INTERFACE_INCLUDE_DIRECTORIES "${OpenNURBS_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "OpenNURBS_FOUND;ON_CPLUSPLUS")
endif()
if(OpenNURBS_FOUND AND NOT TARGET OpenNURBS)
    add_library(OpenNURBS INTERFACE)
    target_link_libraries(OpenNURBS INTERFACE OpenNURBS::OpenNURBS)
endif()
