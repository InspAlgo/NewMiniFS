#include "../../include/command/ls_command.h"

LsCommand::LsCommand()
{

}

bool LsCommand::Accept(const std::string &string) const
{
    if (string == "ls")
        return true;
    else
        return false;
}

void LsCommand::Action(const std::vector<std::string> &argv) const
{

}

void LsCommand::Help() const
{

}