#include "../../include/exceptions/warning_exception.h"

NMFSWarningException::NMFSWarningException(const std::string &message)
    :NMFSException("Warning: " + message)
{

}