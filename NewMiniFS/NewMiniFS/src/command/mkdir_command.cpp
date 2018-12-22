#include "../../include/command/mkdir_command.h"
#include "../../include/io/nmfs_console.h"

MkdirCommand::MkdirCommand()
{

}

bool MkdirCommand::Accept(const std::string &string) const
{
    if (string == "mkdir")
        return true;
    else
        return false;
}

void MkdirCommand::Action(const std::vector<std::string> &argv) const
{
    if (argv.size() != 1)
        throw NMFSWarningException("��������");

    try
    {
        NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();
        space->MakeDirectory(argv[0]);
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
        throw NMFSErrorException("mkdir δ֪�쳣��");
    }
}

void MkdirCommand::Help() const
{
    NMFSConsole::Log("�ڵ�ǰ�ļ������½�һ���ļ���\n");
    NMFSConsole::Log("ָ���ʽ: mkdir [floder_name] ([floder_name]:8B)\n");
}