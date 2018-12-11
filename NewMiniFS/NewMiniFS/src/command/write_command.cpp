#include "../../include/command/write_command.h"

WriteCommand::WriteCommand()
{

}

bool WriteCommand::Accept(const std::string &string) const
{
    if (string == "write")
        return true;
    else
        return false;
}

void WriteCommand::Action(const std::vector<std::string> &argv) const
{

}

void WriteCommand::Help() const
{

}