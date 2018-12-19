#include "../../include/exceptions/error_exception.h"

NMFSErrorException::NMFSErrorException(const std::string &message)
    :NMFSException("Error: " + message)
{

}