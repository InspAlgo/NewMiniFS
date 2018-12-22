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
        throw NMFSWarningException("参数错误！");

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
        throw NMFSErrorException("mkdir 未知异常！");
    }
}

void MkdirCommand::Help() const
{
    NMFSConsole::Log("在当前文件夹下新建一个文件夹\n");
    NMFSConsole::Log("指令格式: mkdir [floder_name] ([floder_name]:8B)\n");
}