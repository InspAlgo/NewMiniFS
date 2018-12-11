#pragma once

#include "../command.h"

class MkdirCommand :virtual public NMFSCommand
{
public:
    bool Accept(const std::string & string) const;
    void Action(const std::vector<std::string> & argv) const;
    void Help() const;

    MkdirCommand();
};