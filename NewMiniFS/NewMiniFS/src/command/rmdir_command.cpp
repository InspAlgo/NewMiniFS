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
        throw NMFSWarningException("参数错误！");

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
        throw NMFSErrorException("rmdir 未知异常！");
    }
}

void RmdirCommand::Help() const
{
    NMFSConsole::Log("删除当前文件夹下一个文件夹\n");
    NMFSConsole::Log("指令格式: rmdir [floder_name] ([floder_name]:8B)\n");
}