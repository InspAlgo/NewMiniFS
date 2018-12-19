#pragma once

#include "nmfs_exception.h"

class NMFSErrorException :public NMFSException
{
public:
    NMFSErrorException(const std::string &message);
};