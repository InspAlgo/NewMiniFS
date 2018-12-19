#pragma once

#include <string>

class NMFSException
{
public:
    NMFSException(const std::string &message);
    NMFSException();

    std::string GetExceptMessage() const noexcept;

protected:
    void SetExceptMessage(const std::string &message) noexcept;

private:
    std::string _msg;
};