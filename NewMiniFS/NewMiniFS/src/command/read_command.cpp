#include "../../include/command/read_command.h"

ReadCommand::ReadCommand()
{

}

bool ReadCommand::Accept(const std::string &string) const
{
    if (string == "read")
        return true;
    else
        return false;
}

void ReadCommand::Action(const std::vector<std::string> &argv) const
{

}

void ReadCommand::Help() const
{

}