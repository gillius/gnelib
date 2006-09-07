CMAKE_MINIMUM_REQUIRED( VERSION 2.2 )    
PROJECT( GNE CXX C )

#Set up GNE version information
SET( GNE_MAJOR_VERSION 0 )
SET( GNE_MINOR_VERSION 75 )
SET( GNE_MICRO_VERSION 0 )

SET( GNE_VERSION "${GNE_MAJOR_VERSION}.${GNE_MINOR_VERSION}.${GNE_MICRO_VERSION}" )

SET( GNE_VERSION_STR "GNE ${GNE_VERSION}-Development" )

MESSAGE( STATUS "GNE Version ${GNE_VERSION_STR}" )

SET( GNE_PACKAGE_NAME "libgnelib-dev" )

#Set up build type and build options, including build flags
IF( NOT CMAKE_BUILD_TYPE )
    SET( CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE )
ENDIF( NOT CMAKE_BUILD_TYPE )

OPTION( GNE_EXTRA_DEBUG_MODE
    "If on, sets on GNE's debugging mode, which adds extra checks and asserts"
    OFF )

#Right now we support this only for GCC
IF ( CMAKE_COMPILER_IS_GNUCXX )
  OPTION( GNE_STRIP_RESULTS
    "If on, strips debugging symbols from generated binaries when linking"
    OFF )
ENDIF( CMAKE_COMPILER_IS_GNUCXX )

OPTION( GNE_BUILD_EXAMPLES
    "If on, builds the example programs, to be installed to /share/${GNE_PACKAGE_NAME}/examples"
    OFF )

SET( GNE_COMMON_FLAGS "" ) #make sure this is defined

#Set compile flags to be used in common across all of our code.
IF( CMAKE_COMPILER_IS_GNUCXX )
    SET( GNE_COMMON_FLAGS "-Wall" )
ENDIF( CMAKE_COMPILER_IS_GNUCXX )
IF( MSVC )
    SET( GNE_COMMON_FLAGS "/W3 /D_CRT_SECURE_NO_DEPRECATE" )
ENDIF( MSVC )

IF( GNE_EXTRA_DEBUG_MODE )
    ADD_DEFINITIONS( -D_DEBUG )
ENDIF( GNE_EXTRA_DEBUG_MODE )

#This is probably only required on GCC or UNIX or anything using POSIX
#threads, but to be safe I always define it, since it shouldn't hurt on
#compilers that don't listen to this.
IF( NOT MSVC )
  ADD_DEFINITIONS( -D_REENTRANT )
ENDIF( NOT MSVC )

SET( GNE_LINKER_FLAGS "" ) #make sure this is defined

IF( GNE_STRIP_RESULTS )
    IF( CMAKE_COMPILER_IS_GNUCXX )
        SET( GNE_LINKER_FLAGS "-s" )
    ENDIF( CMAKE_COMPILER_IS_GNUCXX )
ENDIF( GNE_STRIP_RESULTS )

# Documentation Section -- Find and use Doxygen and Dot
# More documentation config can be found in src/CMakeLists.txt
FIND_PACKAGE( Doxygen )

IF( NOT DEFINED GNE_BUILD_DOCS )
    IF ( DOXYGEN )
        MESSAGE( STATUS "Doxygen found at ${DOXYGEN}, so GNE_BUILD_DOCS set to ON" )
        SET( GNE_BUILD_DOCS_DEFAULT ON )
    ELSE( DOXYGEN )
        MESSAGE( STATUS "Doxygen was not found, so GNE_BUILD_DOCS set to OFF" )
        SET( GNE_BUILD_DOCS_DEFAULT OFF )
    ENDIF( DOXYGEN )

    OPTION( GNE_BUILD_DOCS
        "If on, builds the GNE documentation.  Requires Doxygen to do this."
        ${GNE_BUILD_DOCS_DEFAULT} )

ENDIF( NOT DEFINED GNE_BUILD_DOCS )

IF( NOT DEFINED GNE_USE_DOT )
    IF( DOT )
        MESSAGE( STATUS "DOT tool was detected, so it will be used if GNE_BUILD_DOCS is set" )
        SET( GNE_USE_DOT_DEFAULT ON )
    ELSE( DOT )
        MESSAGE( STATUS "DOT tool was not detected, so it will not be used if GNE_BUILD_DOCS is set" )
        SET( GNE_USE_DOT_DEFAULT OFF )
    ENDIF( DOT )

    OPTION( GNE_USE_DOT
        "If set, uses dot when the documentation is built. Only matters if GNE_BUILD_DOCS is ON"
        ${GNE_USE_DOT_DEFAULT} )
ENDIF( NOT DEFINED GNE_USE_DOT )

#Library detection section: HawkNL, curses, and Boost

IF( UNIX )
    SET( LIB_SEARCH_PATHS /lib /usr/lib /usr/local/lib )
    SET( INCLUDE_SEARCH_PATHS /include /usr/include /usr/local/include )
ENDIF( UNIX )

FIND_LIBRARY( HAWKNL_LIBRARY
    NAMES hawknl NL
    PATHS ${LIB_SEARCH_PATHS} )

FIND_PATH( HAWKNL_INCLUDE_PATH
    nl.h
    ${INCLUDE_SEARCH_PATHS}
    DOC "Path that contains the HawkNL include file, nl.h" )

IF( NOT HAWKNL_LIBRARY OR NOT HAWKNL_INCLUDE_PATH )
    MESSAGE( SEND_ERROR "The library HawkNL version 1.68 or higher was not found. You will need to set HAWKNL_LIBRARY and HAWKNL_INCLUDE_PATH manually." )
ENDIF( NOT HAWKNL_LIBRARY OR NOT HAWKNL_INCLUDE_PATH )

MESSAGE( STATUS "HawkNL: ${HAWKNL_LIBRARY} ${HAWKNL_INCLUDE_PATH}/nl.h" )

#Boost detection -- FindBoost is 2.4 or later only :(
FIND_PATH( BOOST_INCLUDE_PATH
    boost/config.hpp
    ${INCLUDE_SEARCH_PATHS}
    DOC "Path that contains Boost." )

IF( NOT BOOST_INCLUDE_PATH )
    MESSAGE( SEND_ERROR "The Boost library was not found.  Version 1.32 or later required.  Please set BOOST_INCLUDE_PATH manually." )
ENDIF( NOT BOOST_INCLUDE_PATH )

MESSAGE( STATUS "Boost found at: ${BOOST_INCLUDE_PATH}" )

#Curses detection
IF( UNIX )
    FIND_PACKAGE( Curses REQUIRED )
ENDIF( UNIX )

#Set up global include paths
SET( GNE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include )
INCLUDE_DIRECTORIES( ${GNE_INCLUDE_DIR} ${BOOST_INCLUDE_PATH} ${HAWKNL_INCLUDE_PATH} ${CURSES_INCLUDE_PATH} )


SUBDIRS( src include )

IF( GNE_BUILD_EXAMPLES )
    SUBDIRS( examples )
ENDIF( GNE_BUILD_EXAMPLES )
