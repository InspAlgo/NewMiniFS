#include "../../include/command/rmdir_command.h"
#include "../../include/io/nmfs_console.h"

RmdirCommand::RmdirCommand()
{

}

bool RmdirCommand::Accept(const std::string &string) const
{
    if (string == "rmdir")
        return true;
    else
        return false;
}

void RmdirCommand::Action(const std::vector<std::string> &argv) const
{
    if (argv.size() != 1)
        throw NMFSWarningException("��������");

    try
    {
        NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();
        space->RemoveDirectory(argv[0]);
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
        throw NMFSErrorException("rmdir δ֪�쳣��");
    }
}

void RmdirCommand::Help() const
{
    NMFSConsole::Log("ɾ����ǰ�ļ�����һ���ļ���\n");
    NMFSConsole::Log("ָ���ʽ: rmdir [floder_name] ([floder_name]:8B)\n");
}