#include "../../include/command/sfs_command.h"

SfsCommand::SfsCommand()
{

}

bool SfsCommand::Accept(const std::string &string) const
{
    if (string == "sfs")
        return true;
    else
        return false;
}

void SfsCommand::Action(const std::vector<std::string> &argv) const
{

}

void SfsCommand::Help() const
{

}