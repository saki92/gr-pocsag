INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_POCSAG pocsag)

FIND_PATH(
    POCSAG_INCLUDE_DIRS
    NAMES pocsag/api.h
    HINTS $ENV{POCSAG_DIR}/include
        ${PC_POCSAG_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    POCSAG_LIBRARIES
    NAMES gnuradio-pocsag
    HINTS $ENV{POCSAG_DIR}/lib
        ${PC_POCSAG_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(POCSAG DEFAULT_MSG POCSAG_LIBRARIES POCSAG_INCLUDE_DIRS)
MARK_AS_ADVANCED(POCSAG_LIBRARIES POCSAG_INCLUDE_DIRS)

