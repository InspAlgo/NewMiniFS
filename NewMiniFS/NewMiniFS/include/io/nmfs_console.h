#pragma once

#include <string>
#include <vector>
#include <list>

#include "../command.h"
#include "../commands.h"

class NMFSConsole
{
public:
    static NMFSConsole &Instance();

    ~NMFSConsole();

    static void Log(const std::string &output);
    static void Log(const unsigned char *&output);
    static void Log(const char *&output);

    static std::vector<std::string> Split(const std::string& in, const std::string& delim);

    std::list<NMFSCommand*>*_cmds;

private:
    NMFSConsole();
};