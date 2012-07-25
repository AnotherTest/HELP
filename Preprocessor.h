/**
 * @file Preprocessor.h
 * Contains the actual HELPreprocessor (as the Preprocessor class).
 * @author Tim Beyne
 */
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <map>
#include <utility>
#include <vector>

#include <boost/regex.hpp>

using boost::regex;

#include "HelpAssert.h"

/**
 * Removes all whitespace from the begin and end of a given string.
 * @param s the string to be trimmed as a reference
 */
void trim(std::string& s);

/**
 * Represents the HELPreprocessor.
 */
class Preprocessor {
    using Error = std::runtime_error;
    using AliasMap = std::map<std::string, std::string>;

    std::string file_name;
    std::vector<std::string> lines;
    std::string source;
    AliasMap aliases;

    /**
     * @pre lines.size() == 0
     */
    void readFile();

    void writeFile();

    /**
     * @pre it != lines.end()
     */
    int mergeLines(std::vector<std::string>::iterator& it);

    void addMacro(const std::string& line);

    /**
     * @pre aliases.size() == 0
     */
    void addMacros();

    void applyMacro(const regex& ex, const std::string& replacement);

    void applyMacros();

public:
    Preprocessor(const std::string& f);

    /**
     * @pre lines.size() == 0
     */
    void process();
};
