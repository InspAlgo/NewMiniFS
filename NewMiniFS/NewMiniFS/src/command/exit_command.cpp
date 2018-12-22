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
        throw NMFSWarningException("参数错误！");

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
        throw NMFSErrorException("exit 未知异常！");
    }

}

void ExitCommand::Help() const
{
    NMFSConsole::Log("关闭已打开的空间\n");
    NMFSConsole::Log("指令格式: exit\n");
}