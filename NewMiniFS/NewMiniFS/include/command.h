#pragma once

#include <string>
#include <vector>

#include "../../NewMiniFS.Core/include/nmfs_space.h"
#include "../../NewMiniFS.Core/include/exceptions/warning_exception.h"
#include "../../NewMiniFS.Core/include/exceptions/error_exception.h"

class NMFSCommand
{
public:
    virtual bool Accept(const std::string &string) const = 0;
    virtual void Action(const std::vector<std::string> &argv) const = 0;
    virtual void Help() const = 0;

protected:
    NMFSCommand() = default;
};