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

    static void SplitString(const std::string &s, std::vector<std::string> &v, const std::string &c);

    std::list<NMFSCommand*>*_cmds;

private:
    NMFSConsole();
};