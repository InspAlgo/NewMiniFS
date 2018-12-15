#pragma once

#include <string>

class NMFSConsole
{
public:
    static NMFSConsole &Instance();

    void Log(const std::string &string);

private:
    NMFSConsole();
    NMFSConsole(const NMFSConsole &another);
};