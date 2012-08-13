/**
 * @file Preprocessor.h
 * Contains the actual HELPreprocessor (as the Preprocessor class).
 * @author Tim Beyne
 */

#ifndef _HELPREPROCESSOR_GUARD
#define _HELPREPROCESSOR_GUARD

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>

#include <boost/regex.hpp>

using boost::regex;

#include "HelpAssert.h"

/**
 * Removes all whitespace from the begin and end of a given string.
 * @param s the string to be trimmed as a reference
 */
void trim(std::string& s);

std::string removeWs(const std::string& s);

void replace_all(std::string& s, const std::string& tr, const std::string& r);

class Alias {
    std::string to_replace;
    std::string replacement;
    regex regular;
public:
    Alias(const std::string& tr, const std::string& r)
        : to_replace(tr), replacement(r), regular(tr) {}

    std::string getRegexString() const
    {
        return to_replace;
    }

    std::string getReplacement() const
    {
        return replacement;
    }

    regex getRegex() const
    {
        return regular;
    }
};

/**
 * Represents the HELPreprocessor.
 */
class Preprocessor {
    using Error = std::runtime_error;
    using AliasMap = std::vector<Alias>;

    std::string file_name;
    std::vector<std::string> lines;
    std::string source;
    AliasMap aliases;
    bool rm_ws, rd_ws;

    void parseEscape(std::string& s, const std::string& escp, const std::string& repl);

    void parseEscapes(std::string& s);

    /**
     * @pre lines.size() == 0
     * @pre source.empty() == true
     */
    void readFile();

    void writeFile();

    /**
     * @pre lines.size() == 0
     */
    void processSource();

    /**
     * @pre it != lines.end()
     */
    int mergeLines(std::vector<std::string>::iterator& it);

    void handleSpecialMacro(std::string line);

    void addMacro(const std::string& line);

    /**
     * @pre aliases.size() == 0
     */
    void addMacros();

    void applyMacro(const regex& ex, std::string replacement);

    void applyMacros();

    void cleanUp();
public:
    Preprocessor(const std::string& f);

    /**
     * @pre lines.size() == 0
     */
    void process();
};

#endif // _HELPREPROCESSOR_GUARD
