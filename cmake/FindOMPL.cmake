# - Try to find the OMPL library
# Once done this will define:
#
# OMPL_FOUND - OMPL was found
# OMPL_INCLUDE_DIRS - The OMPL include directory
# OMPL_LIBRARIES - The OMPL library
# OMPLAPP_LIBRARIES - The OMPL.app libraries
# OMPL_VERSION - The OMPL version in the form <major>.<minor>.<patchlevel>
# OMPL_MAJOR_VERSION - Major version
# OMPL_MINOR_VERSION - Minor version
# OMPL_PATCH_VERSION - Patch version

include(FindPackageHandleStandardArgs)

# user can set OMPL_PREFIX to specify the prefix path of the OMPL library
# and include directory, either as an environment variable or as an
# argument to cmake ("cmake -DOMPL_PREFIX=...")
if (NOT OMPL_PREFIX)
    set(OMPL_PREFIX $ENV{OMPL_PREFIX})
endif()

if (OMPL_FIND_VERSION)
    set(OMPL_SUFFIX "-${OMPL_VERSION}")
else()
    set(OMPL_SUFFIX "")
endif()

# user can set OMPL_LIB_PATH to specify the path for the OMPL library
# (analogous to OMPL_PREFIX)
if (NOT OMPL_LIB_PATH)
    set(OMPL_LIB_PATH $ENV{OMPL_LIB_PATH})
    if (NOT OMPL_LIB_PATH)
        set(OMPL_LIB_PATH ${OMPL_PREFIX})
    endif()
endif()

# user can set OMPL_INCLUDE_PATH to specify the path for the OMPL include
# directory (analogous to OMPL_PREFIX)
if (NOT OMPL_INCLUDE_PATH)
    set(OMPL_INCLUDE_PATH $ENV{OMPL_INCLUDE_PATH})
    if (NOT OMPL_INCLUDE_PATH)
        set(OMPL_INCLUDE_PATH ${OMPL_PREFIX})
    endif()
endif()

# find the OMPL library
find_library(OMPL_LIBRARY ompl
        PATHS ${OMPL_LIB_PATH}
        PATH_SUFFIXES lib build/lib)
if (OMPL_LIBRARY)
    if (OMPL_FIND_VERSION)
        get_filename_component(libpath ${OMPL_LIBRARY} PATH)
        file(GLOB OMPL_LIBS "${libpath}/libompl.${OMPL_FIND_VERSION}.*")
        list(GET OMPL_LIBS -1 OMPL_LIBRARY)
    endif()
    set(OMPL_LIBRARIES "${OMPL_LIBRARY}" CACHE FILEPATH "Path to OMPL library")
endif()
# find the OMPL.app libraries
find_library(OMPLAPPBASE_LIBRARY ompl_app_base
        PATHS ${OMPL_LIB_PATH}
        PATH_SUFFIXES lib build/lib)
find_library(OMPLAPP_LIBRARY ompl_app
        PATHS ${OMPL_LIB_PATH}
        PATH_SUFFIXES lib build/lib)
if (OMPLAPPBASE_LIBRARY AND OMPLAPP_LIBRARY)
    if (OMPL_FIND_VERSION)
        get_filename_component(libpath ${OMPLAPPBASE_LIBRARY} PATH)
        file(GLOB OMPLAPPBASE_LIBS "${libpath}/libompl_app_base.${OMPL_FIND_VERSION}.*")
        list(GET OMPLAPPBASE_LIBS -1 OMPLAPPBASE_LIBRARY)
        get_filename_component(libpath ${OMPLAPP_LIBRARY} PATH)
        file(GLOB OMPLAPP_LIBS "${libpath}/libompl_app.${OMPL_FIND_VERSION}.*")
        list(GET OMPLAPP_LIBS -1 OMPLAPP_LIBRARY)
    endif()
    set(OMPLAPP_LIBRARIES "${OMPLAPPBASE_LIBRARY};${OMPLAPP_LIBRARY}" CACHE STRING "Paths to OMPL.app libraries")
endif()