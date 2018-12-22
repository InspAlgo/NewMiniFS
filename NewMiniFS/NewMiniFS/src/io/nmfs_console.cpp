#include "../../include/io/nmfs_console.h"
#include "../../include/io/nmfs_console.h"

#include <iostream>

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
    std::cout << output;
}

void NMFSConsole::Log(const unsigned char *&output)
{
    std::cout << output;
}

void NMFSConsole::Log(const char *&output)
{
    std::cout << output;
}

void NMFSConsole::SplitString(const std::string &s, std::vector<std::string> &v, const std::string &c)
{
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
        v.push_back(s.substr(pos1));
}