//! \file version.cc SIMLIB version number
#include "simlib.h"
#include "internal.h"

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

const unsigned SIMLIB_version = __SIMLIB__;  //!< library version

//! Get version of SIMLIB
//! \returns string with version of SIMLIB/C++
const char *SIMLIB_version_string() {
    static const char s[] = "SIMLIB " SIMLIB_VERSION " (" __DATE__ " " __TIME__ ")";
    return s;
}

} // namespace

