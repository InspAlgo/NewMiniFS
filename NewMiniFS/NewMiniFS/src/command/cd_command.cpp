#include "../../include/command/cd_command.h"

CdCommand::CdCommand()
{

}

bool CdCommand::Accept(const std::string &string) const
{
    if (string == "cd")
        return true;
    else
        return false;
}

void CdCommand::Action(const std::vector<std::string> &argv) const
{

}

void CdCommand::Help() const
{

}