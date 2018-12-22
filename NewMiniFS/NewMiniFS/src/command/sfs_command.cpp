#include "../../include/command/sfs_command.h"
#include "../../include/io/nmfs_console.h"

SfsCommand::SfsCommand()
{

}

bool SfsCommand::Accept(const std::string &string) const
{
    if (string == "sfs")
        return true;
    else
        return false;
}

void SfsCommand::Action(const std::vector<std::string> &argv) const
{
    if (argv.size() != 1)
        throw NMFSWarningException("��������");

    try
    {
        NMFSSpace::System(argv[0]);
    }
    catch (NMFSWarningException)
    {
        throw;
    }
    catch (NMFSErrorException)
    {
        throw;
    }
    catch (...)
    {
        throw NMFSErrorException("sfs δ֪�쳣��");
    }
}

void SfsCommand::Help() const
{
    NMFSConsole::Log("��װ�ռ�\n");
    NMFSConsole::Log("ָ���ʽ: sys [space_name]\n");
}