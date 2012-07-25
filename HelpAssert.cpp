/**
 * @file GhAssert.h
 * Implements the assert_handler function found in GhAssert.h.
 * @author Tim Beyne
 */
#include "HelpAssert.h"

int HelpAssertion::assert_handler(char const* expr, char const* file, int line)
{
    std::cerr << "HELPreprocessor assertion failed: (fatal)\n"
              << "HELP_ASSERT(" << expr << ") in " << file << " at line " << line << ".\n"
              << std::endl;
    return 1;
}

