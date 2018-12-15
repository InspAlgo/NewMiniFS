#include "../include/nmfs_space.h"

NMFSSpace* volatile NMFSSpace::_global_space_pointer = nullptr;

NMFSSpace*  NMFSSpace::NewNMFSSpace(const std::string &space_name)
{
    _global_space_pointer = new NMFSSpace(space_name);

    return _global_space_pointer;
}

NMFSSpace*  NMFSSpace::GetActiveNMFSSpace()
{
    return _global_space_pointer;
}

NMFSSpace::NMFSSpace(const std::string &space_name)
{
    _space_block = new std::array<unsigned char *, 512>();
}