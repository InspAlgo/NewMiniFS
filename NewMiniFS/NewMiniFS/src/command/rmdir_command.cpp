#include "../../include/command/rmdir_command.h"

RmdirCommand::RmdirCommand()
{

}

bool RmdirCommand::Accept(const std::string &string) const
{
    if (string == "rmdir")
        return true;
    else
        return false;
}

void RmdirCommand::Action(const std::vector<std::string> &argv) const
{

}

void RmdirCommand::Help() const
{

}