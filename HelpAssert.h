#define _HELP_DEBUGMODE
/**
 * @file HelpAssert.h
 * Contains the macros and functions required for HELP_ASSERT.
 * @author Tim Beyne
 */
#ifndef _GREENHOUSE_ASSERT_GUARD
#define _GREENHOUSE_ASSERT_GUARD

#include <iostream>

namespace HelpAssertion {
    int assert_handler(char const* expr, char const* file, int line);
}

#ifdef _MSC_VER
    #define _HELP_ASSERT_HALT() __debugbreak()
#else
    #include <cstdlib>
    #define _HELP_ASSERT_HALT() exit(__LINE__)
#endif

#ifdef _HELP_DEBUGMODE
    #define HELP_ASSERT(x) ((void)(!(x) && HelpAssertion::assert_handler(#x, __FILE__, __LINE__)\
            && (_HELP_ASSERT_HALT(), 1)))
#else
    #define HELP_ASSERT(x) ((void)sizeof(x))
#endif

#endif // _GREENHOUSE_ASSERT_GUARD

