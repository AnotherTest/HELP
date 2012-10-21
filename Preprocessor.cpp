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

std::string file_extension(const std::string& filename)
{
    size_t pos = filename.find_last_of('.');
    return (pos == std::string::npos) ? "" : filename.substr(pos + 1);
}

std::string file_directory(const std::string& filepath)
{
    size_t pos = filepath.find_last_of('/');
    return (pos == std::string::npos) ? "" : filepath.substr(0, pos + 1);
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

void Preprocessor::readFile(const std::string& name)
{
    HELP_ASSERT(lines.size() == 0);
    HELP_ASSERT(source.empty() == true);
    std::ifstream ifs(name);
    if(!ifs.is_open())
        throw Error("cannot open file " + name);
    ifs.seekg(0, std::ios::end);
    size_t len = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    char* buffer = new char[len];
    ifs.read(buffer, len);
    source.assign(buffer, len);
    ifs.close();
    delete[] buffer;
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
    analyseSource();
    applyMacros();
}

void Preprocessor::analyseSource()
{
    HELP_ASSERT(lines.size() == 0);
    std::istringstream iss(source);
    while(iss.good()) {
        std::string line;
        std::getline(iss, line);
        lines.push_back(line);
    }
    addMacros();
}

int Preprocessor::mergeLines(const LineIterator& it)
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

void Preprocessor::handleIncludeMacro(const std::string& file, const LineIterator& line)
{
    std::ifstream ifs(file);
    if(!ifs)
        throw Error("cannot not open #included file " + file);
    std::string l;
    LineIterator it = line;
    while(std::getline(ifs, l))
        it = lines.insert(it, l) + 1;
    ifs.close();
}

void Preprocessor::handleSpecialMacro(const LineIterator& line)
{
    trim(*line);
    if(*line == "skip whitespace")
        rm_ws = !rm_ws;
    else if(*line == "reduce whitespace")
        rd_ws = !rd_ws;
    else if(line->substr(0, 7) == "include")
        handleIncludeMacro(line->substr(line->find(' ') + 1), line);
}

void Preprocessor::addMacro(const LineIterator& line)
{
    *line = line->substr(1);
    size_t pos = line->find(":=");
    if(pos == std::string::npos)
        return handleSpecialMacro(line);

    std::string name = line->substr(0, pos);
    trim(name);
    std::string replacement = line->substr(pos + 2);
    trim(replacement);
    parseEscapes(replacement);
    aliases.push_back(Alias(name, replacement));
}

void Preprocessor::addMacros()
{
    auto it = lines.begin();
    while(it != lines.end()) {
        if(it->length() > 0 && it->at(0) == '#') {
            trim(*it);
            mergeLines(it);
            size_t size = lines.size();
            size_t pos = std::distance(lines.begin(), it);
            addMacro(it);
            it = lines.erase(lines.begin() + pos + lines.size() - size);
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
    for(auto alias : aliases)
        applyMacro(alias.getRegex(), alias.getReplacement());
}

void Preprocessor::cleanUp()
{
    lines.clear();
    aliases.clear();
}

void Preprocessor::applyHelpFile()
{
    std::string help_file = file_directory(file_name) + file_extension(file_name) + ".help";
    std::ifstream hfs(help_file);
    if(!hfs.is_open())
        return;
    hfs.close();
    std::cout << "Using HELP file: " << help_file << '.' << std::endl;
    readFile(help_file);
    analyseSource();
    lines.clear();
    source.clear();
}

void Preprocessor::process()
{
    HELP_ASSERT(lines.size() == 0);
    applyHelpFile();
    readFile(file_name);
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
