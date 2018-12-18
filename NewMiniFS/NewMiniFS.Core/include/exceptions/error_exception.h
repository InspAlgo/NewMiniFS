#pragma once

#include "nmfs_exception.h"

class NMFSErrorException :public NMFSException
{
public:
    NMFSErrorException();
    NMFSErrorException(const std::string &message);
};