// ***************************************************************************
// bamtools_global.h (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides the basic definitions for exporting & importing library symbols.
// Also provides some platform-specific rules for definitions.
// ***************************************************************************

#ifndef BAMTOOLS_GLOBAL_H
#define BAMTOOLS_GLOBAL_H

/*! \brief Library export macro
    \internal
*/
#ifndef BAMTOOLS_LIBRARY_EXPORT
#define BAMTOOLS_LIBRARY_EXPORT
#endif  // BAMTOOLS_LIBRARY_EXPORT

/*! \brief Library import macro
    \internal
*/
#ifndef BAMTOOLS_LIBRARY_IMPORT
#define BAMTOOLS_LIBRARY_IMPORT
#endif  // BAMTOOLS_LIBRARY_IMPORT

/*! \brief Platform-specific type definitions
    \internal
*/
#ifndef BAMTOOLS_LFS
#define BAMTOOLS_LFS
#ifdef _WIN32
#define ftell64(a) _ftelli64(a)
#define fseek64(a, b, c) _fseeki64(a, b, c)
#else
#define ftell64(a) ftello(a)
#define fseek64(a, b, c) fseeko(a, b, c)
#endif
#endif  // BAMTOOLS_LFS

/*! \def ftell64(a)
    \brief Platform-independent tell() operation.
    \internal
*/
/*! \def fseek64(a,b,c)
    \brief Platform-independent seek() operation.
    \internal
*/

/*! \brief Platform-specific type definitions
    \internal
*/
#ifndef BAMTOOLS_TYPES
#define BAMTOOLS_TYPES
#include <stdint.h>
#endif  // BAMTOOLS_TYPES

//! \internal
inline void bamtools_noop() {}

/*! \brief Assert definitions
    \internal
*/
#ifndef BAMTOOLS_ASSERTS
#define BAMTOOLS_ASSERTS
#ifdef NDEBUG
#define BT_ASSERT_UNREACHABLE bamtools_noop()
#define BT_ASSERT_X(condition, message) bamtools_noop()
#else
#include <cassert>
#include <stdexcept>
#define BT_ASSERT_UNREACHABLE assert(false)
#define BT_ASSERT_X(condition, message)    \
    if (!(condition)) {                    \
        throw std::runtime_error(message); \
    }
#endif
#endif  // BAMTOOLS_ASSERTS

#endif  // BAMTOOLS_GLOBAL_H
