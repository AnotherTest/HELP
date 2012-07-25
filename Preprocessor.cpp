/**
 * @file Preprocessor.cpp
 * Implements Preprocessor.h
 * @author Tim Beyne
 */
#include "Preprocessor.h"

void trim(std::string& s)
{
    while(std::isspace(s[0]))
        s.erase(0, 1);
    while(std::isspace(s[s.length() - 1]))
        s.erase(s.length() - 1, 1);
}

// Preprocessor implementation starts here

Preprocessor::Preprocessor(const std::string& f)
    : file_name(f), lines(), source(), aliases() {}

void Preprocessor::readFile()
{
    HELP_ASSERT(lines.size() == 0);
    std::ifstream ifs(file_name);
    if(!ifs.is_open())
        throw Error("cannot open file " + file_name);

    while(ifs.good()) {
        std::string line;
        std::getline(ifs, line);
        lines.push_back(line);
    }
    ifs.close();
}

void Preprocessor::writeFile()
{
    std::ofstream ofs(file_name + ".out");
    ofs << source;
    ofs.flush();
    ofs.close();
}

int Preprocessor::mergeLines(std::vector<std::string>::iterator& it)
{
    HELP_ASSERT(it != lines.end());
    int n = 0;
    auto it2 = it + 1;
    while(it2 != lines.end() && it2->length() > 0 && it2->at(0) == ' ') {
        *it += '\n' + it2->substr(1);
        lines.erase(it2);
        ++n;
    }
    return n;
}

void Preprocessor::addMacro(const std::string& line)
{
    size_t pos = line.find(":=");
    std::string name = line.substr(0, pos);
    trim(name);
    std::string replacement = line.substr(pos + 2);
    aliases.insert(std::make_pair(name, replacement));
}

void Preprocessor::addMacros()
{
    HELP_ASSERT(aliases.size() == 0);
    auto it = lines.begin();
    while(it != lines.end()) {
        if(it->length() > 0 && it->at(0) == '#') {
            trim(*it);
            mergeLines(it);
            addMacro(it->substr(1));
            lines.erase(it);
        } else
            ++it;
    }
}

void Preprocessor::applyMacro(const regex& ex, const std::string& replacement)
{
    source = boost::regex_replace(source, ex, replacement);
}

void Preprocessor::applyMacros()
{
    for(std::string s : lines)
        source += s + '\n';
    trim(source);
    // replaces
    for(auto pair : aliases) {
        regex pattern(pair.first);
        applyMacro(pattern, pair.second);
    }
}

void Preprocessor::process()
{
    HELP_ASSERT(lines.size() == 0);
    readFile();
    addMacros();
    applyMacros();
    writeFile();
}
