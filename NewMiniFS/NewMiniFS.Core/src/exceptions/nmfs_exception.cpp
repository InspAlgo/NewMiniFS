#include "../../include/exceptions/nmfs_exception.h"

NMFSException::NMFSException(const std::string &message)
    : _msg(message)
{

}

NMFSException::NMFSException()
    : NMFSException("A NMFS exception occured.")
{

}

std::string NMFSException::GetExceptMessage() const noexcept
{
    return _msg;
}

void NMFSException::SetExceptMessage(const std::string &message) noexcept
{
    _msg = message;
}