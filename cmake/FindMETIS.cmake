# - Try to find the METIS graph partitioning library
# Once done, this will define:
#
# METIS_FOUND -- boolean that indicates whether METIS was found
# METIS_INCLUDE_DIRS -- the include path for METIS
# METIS_LIBRARIES -- the METIS libraries to link against

## Try to find the include directory
find_path(METIS_INCLUDE_DIRS
    NAMES metis.h
    PATHS /usr/include /usr/local/include)

## Try to find the METIS library
find_library(METIS_LIBRARIES
    NAMES metis
    PATHS /usr/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    METIS DEFAULT_MSG METIS_INCLUDE_DIRS METIS_LIBRARIES)
mark_as_advanced(METIS_INCLUDE_DIRS)
mark_as_advanced(METIS_LIBRARIES)
