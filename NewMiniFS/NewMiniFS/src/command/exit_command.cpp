#include "../../include/command/exit_command.h"
#include "../../include/io/nmfs_console.h"

ExitCommand::ExitCommand()
{

}

bool ExitCommand::Accept(const std::string &string) const
{
    if (string == "exit")
        return true;
    else
        return false;
}

void ExitCommand::Action(const std::vector<std::string> &argv) const
{
    if (argv.size() > 0)
        throw NMFSWarningException("��������");

    try
    {
        NMFSSpace::Exit();
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
        throw NMFSErrorException("exit δ֪�쳣��");
    }

}

void ExitCommand::Help() const
{
    NMFSConsole::Log("�ر��Ѵ򿪵Ŀռ�\n");
    NMFSConsole::Log("ָ���ʽ: exit\n");
}