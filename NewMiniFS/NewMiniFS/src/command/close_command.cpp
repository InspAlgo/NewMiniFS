#include "../../include/command/close_command.h"
#include "../../include/io/nmfs_console.h"

CloseCommand::CloseCommand()
{

}

bool CloseCommand::Accept(const std::string &string) const
{
    if (string == "close")
        return true;
    else
        return false;
}

void CloseCommand::Action(const std::vector<std::string> &argv) const
{
    if (argv.size() > 0)
        throw NMFSWarningException("��������");

    try
    {
        NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();
        space->Close();
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
        throw NMFSErrorException("close δ֪�쳣��");
    }
}

void CloseCommand::Help() const
{
    NMFSConsole::Log("�ر��Ѵ򿪵��ļ�\n");
    NMFSConsole::Log("ָ���ʽ: close\n");
}