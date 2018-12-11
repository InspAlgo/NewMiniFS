#include "../../include/command/new_command.h"

NewCommand::NewCommand()
{

}

bool NewCommand::Accept(const std::string &string) const
{
    if (string == "new")
        return true;
    else
        return false;
}

void NewCommand::Action(const std::vector<std::string> &argv) const
{

}

void NewCommand::Help() const
{

}