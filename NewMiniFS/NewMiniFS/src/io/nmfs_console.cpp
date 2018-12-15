#include "../../include/io/nmfs_console.h"

NMFSConsole & NMFSConsole::Instance()
{
    static NMFSConsole instance;
    return instance;
}

NMFSConsole::NMFSConsole()
{

}

NMFSConsole::NMFSConsole(const NMFSConsole &another)
{

}

void NMFSConsole::Log(const std::string &string)
{

}