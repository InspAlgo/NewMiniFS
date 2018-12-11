#include "../../include/command/close_command.h"

CloseCommand::CloseCommand()
{

}

bool CloseCommand::Accept(const std::string &string) const
{
    if (string == "close")
        return true;
    else
        return false;
}

void CloseCommand::Action(const std::vector<std::string> &argv) const
{

}

void CloseCommand::Help() const
{

}