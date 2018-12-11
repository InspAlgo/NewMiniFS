#include "../../include/command/delete_command.h"

DeleteCommand::DeleteCommand()
{

}

bool DeleteCommand::Accept(const std::string &string) const
{
    if (string == "delete")
        return true;
    else
        return false;
}

void DeleteCommand::Action(const std::vector<std::string> &argv) const
{

}

void DeleteCommand::Help() const
{

}