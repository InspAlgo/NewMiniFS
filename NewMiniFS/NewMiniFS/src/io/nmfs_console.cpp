#include "../../include/io/nmfs_console.h"
#include "../../include/io/nmfs_console.h"

#include <iostream>
#include <sstream>

NMFSConsole & NMFSConsole::Instance()
{
    static NMFSConsole instance;
    return instance;
}

NMFSConsole::NMFSConsole()
{
    _cmds = new std::list<NMFSCommand*>();

    NMFSCommand *cd = new CdCommand();
    _cmds->push_back(cd);

    NMFSCommand *close = new CloseCommand();
    _cmds->push_back(close);

    NMFSCommand *create = new CreateCommand();
    _cmds->push_back(create);

    NMFSCommand *del = new DeleteCommand();
    _cmds->push_back(del);

    NMFSCommand *exit = new ExitCommand();
    _cmds->push_back(exit);

    NMFSCommand *ls = new LsCommand();
    _cmds->push_back(ls);

    NMFSCommand *mkdir = new MkdirCommand();
    _cmds->push_back(mkdir);

    NMFSCommand *newc = new NewCommand();
    _cmds->push_back(newc);

    NMFSCommand *open = new OpenCommand();
    _cmds->push_back(open);

    NMFSCommand *read = new ReadCommand();
    _cmds->push_back(read);

    NMFSCommand *rmdir = new RmdirCommand();
    _cmds->push_back(rmdir);

    NMFSCommand *sfs = new SfsCommand();
    _cmds->push_back(sfs);

    NMFSCommand *write = new WriteCommand();
    _cmds->push_back(write);
}

NMFSConsole::~NMFSConsole()
{
    for (auto &i : *_cmds)
    {
        delete i;
        i = nullptr;
    }

    delete _cmds;
    _cmds = nullptr;
}

void NMFSConsole::Log(const std::string &output)
{
    std::cout << output << std::flush;
    std::cout.clear();
}

void NMFSConsole::Log(const unsigned char *&output)
{
    std::cout << output << std::flush;
    std::cout.clear();
}

void NMFSConsole::Log(const char *&output)
{
    std::cout << output << std::flush;
    std::cout.clear();
}

std::vector<std::string> NMFSConsole::Split(const std::string& in, const std::string& delim)
{
    if (delim.length() < 1)
        return std::vector<std::string>();

    char ch = delim[0];

    std::string in_copy(in);
    for (int i = 0; i < delim.length(); i++)
    {
        for (int j = 0; j < in_copy.length(); j++)
            if (in_copy[j] == delim[i])
                in_copy[j] = ch;
    }

    std::vector<std::string> command_vector;
    std::istringstream iss(in_copy);
    std::string temp;

    while (std::getline(iss, temp, ch))
        command_vector.emplace_back(std::move(temp));

    return command_vector;
}
