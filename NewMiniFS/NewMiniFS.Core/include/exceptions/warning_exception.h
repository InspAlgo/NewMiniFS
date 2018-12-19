#pragma once

#include "nmfs_exception.h"

class NMFSWarningException :public NMFSException
{
public:
    NMFSWarningException(const std::string &message);
};