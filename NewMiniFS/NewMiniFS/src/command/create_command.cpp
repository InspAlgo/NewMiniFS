#include "../../include/command/create_command.h"

CreateCommand::CreateCommand()
{

}

bool CreateCommand::Accept(const std::string &string) const
{
    if (string == "create")
        return true;
    else
        return false;
}

void CreateCommand::Action(const std::vector<std::string> &argv) const
{

}

void CreateCommand::Help() const
{

}