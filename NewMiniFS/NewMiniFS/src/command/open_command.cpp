#include "../../include/command/open_command.h"

OpenCommand::OpenCommand()
{

}

bool OpenCommand::Accept(const std::string &string) const
{
    if (string == "open")
        return true;
    else
        return false;
}

void OpenCommand::Action(const std::vector<std::string> &argv) const
{

}

void OpenCommand::Help() const
{

}