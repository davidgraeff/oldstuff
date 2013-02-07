# - Find library containing dlopen()
# The following variables are set if dlopen is found. If dlopen is not
# found, DLOPEN_FOUND is set to false.
#  DLOPEN_FOUND     - System has dlopen.
#  DLOPEN_LIB - Link these to use dlopen.

# Copyright (c) 2007 Erik Johansson <erik@ejohansson.se>
# Redistribution and use is allowed according to the terms of the BSD license.

include(CheckLibraryExists)

# Assume dlopen is not found.
set(DLOPEN_FOUND FALSE)

foreach (library dl c c_r libdl)
  if (NOT DLOPEN_FOUND)
    check_library_exists(${library} dlopen "" DlopenIN)

    if (${DlopenIN})
      set(DLOPEN_LIB ${library} CACHE STRING "Library containing dlopen")
      set(DLOPEN_FOUND TRUE)
    endif (${DlopenIN})

  endif (NOT DLOPEN_FOUND)
endforeach (library)

if (DLOPEN_FOUND)
  if (NOT DLOPEN_FIND_QUIETLY)
    message(STATUS "Found dlopen in: ${DLOPEN_LIB}")
  endif (NOT DLOPEN_FIND_QUIETLY)
else (DLOPEN_FOUND)
  if (DLOPEN_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find the library containing dlopen")
  endif (DLOPEN_FIND_REQUIRED)
endif (DLOPEN_FOUND)

mark_as_advanced(DLOPEN_LIB)
