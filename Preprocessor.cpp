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

std::string removeWs(const std::string& s)
{
    std::string r;
    for(char c : s) {
        if(!std::isspace(c))
            r += c;
    }
    return r;
}

std::string reduceWs(const std::string& s)
{
    std::string r;
    for(char c : s) {
        if(!std::isspace(c))
            r += c;
        else if(!std::isspace(r[r.size() - 1]))
            r += ' ';
    }
    return r;
}

// Preprocessor implementation starts here

Preprocessor::Preprocessor(const std::string& f)
    : file_name(f), lines(), source(), aliases(), rm_ws(false), rd_ws(false) {}

void Preprocessor::parseEscape(std::string& s, const std::string& escp, const std::string& repl)
{
    size_t pos = 0;
    while((pos = s.find(escp, pos)) != std::string::npos) {
        if(s[pos - 1] == '\\') {
            pos += escp.length();
            continue;
        }
        s.replace(pos, escp.length(), repl);
    }
}

void Preprocessor::parseEscapes(std::string& s)
{
    parseEscape(s, "\\n", "\n");
    parseEscape(s, "\\s", " ");
    parseEscape(s, "\\t", "\t");
    parseEscape(s, "\\v", "\v");
}

void Preprocessor::readFile()
{
    HELP_ASSERT(lines.size() == 0);
    std::ifstream ifs(file_name);
    if(!ifs.is_open())
        throw Error("cannot open file " + file_name);
    ifs.seekg(0, std::ios::end);
    size_t len = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    char* buffer = new char[len];
    ifs.read(buffer, len);
    source = buffer;

    delete buffer;
    ifs.close();
}

void Preprocessor::writeFile()
{
    std::ofstream ofs(file_name + ".out");
    ofs << source;
    ofs.flush();
    ofs.close();
}

void Preprocessor::processSource()
{
    HELP_ASSERT(lines.size() == 0);
    std::istringstream iss(source);
    while(iss.good()) {
        std::string line;
        std::getline(iss, line);
        lines.push_back(line);
    }
    addMacros();
    applyMacros();
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

void Preprocessor::handleSpecialMacro(std::string line)
{
    trim(line);
    if(line == "skip whitespace")
        rm_ws = true;
    else if(line == "reduce whitespace")
        rd_ws = true;
}

void Preprocessor::addMacro(const std::string& line)
{
    size_t pos = line.find(":=");
    if(pos == std::string::npos)
        handleSpecialMacro(line);

    std::string name = line.substr(0, pos);
    trim(name);
    std::string replacement = line.substr(pos + 2);
    trim(replacement);
    parseEscapes(replacement);
    aliases.push_back(Alias(name, replacement));
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

void Preprocessor::applyMacro(const regex& ex, std::string replacement)
{
    source = boost::regex_replace(source, ex, replacement);
}

void Preprocessor::applyMacros()
{
    if(!source.empty())
        source.clear();
    for(std::string s : lines)
        source += s + '\n';
    trim(source);
    // replaces
    for(auto alias : aliases) {
        regex pattern(alias.getRegex());
        applyMacro(pattern, alias.getReplacement());
    }
}

void Preprocessor::cleanUp()
{
    lines.clear();
    aliases.clear();
}

void Preprocessor::process()
{
    HELP_ASSERT(lines.size() == 0);
    readFile();
    std::string old_src;
    do {
        old_src = source;
        processSource();
        cleanUp();
    } while(old_src != source); // continue processing until nothing left to process

    if(rd_ws)
        source = reduceWs(source);
    source = rm_ws ? removeWs(source) : source;

    writeFile();
}
