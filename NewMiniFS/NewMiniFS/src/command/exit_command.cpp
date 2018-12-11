#include "../../include/command/exit_command.h"

ExitCommand::ExitCommand()
{

}

bool ExitCommand::Accept(const std::string &string) const
{
    if (string == "exit")
        return true;
    else
        return false;
}

void ExitCommand::Action(const std::vector<std::string> &argv) const
{

}

void ExitCommand::Help() const
{

}