#include "../../include/command/mkdir_command.h"

MkdirCommand::MkdirCommand()
{

}

bool MkdirCommand::Accept(const std::string &string) const
{
    if (string == "mkdir")
        return true;
    else
        return false;
}

void MkdirCommand::Action(const std::vector<std::string> &argv) const
{

}

void MkdirCommand::Help() const
{

}