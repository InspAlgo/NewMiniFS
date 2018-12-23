#include "../../include/command/cd_command.h"
#include "../../include/io/nmfs_console.h"

CdCommand::CdCommand()
{

}

bool CdCommand::Accept(const std::string &string) const
{
    if (string == "cd")
        return true;
    else
        return false;
}

void CdCommand::Action(const std::vector<std::string> &argv) const
{
    if (argv.size() != 1)
        throw NMFSWarningException("参数错误！");

    std::vector<std::string>temp_v = NMFSConsole::Split(argv[0], "\\/");

    std::queue<std::string>cd_path;
    for (auto i : temp_v)
        if (i.length() > 0)
            cd_path.push(i);

    if (cd_path.size() < 1)
        throw NMFSWarningException("参数错误！");

    try
    {
        NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();
        space->ChangeDirectory(cd_path);
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
        throw NMFSErrorException("cd 未知异常！");
    }
}

void CdCommand::Help() const
{
    NMFSConsole::Log("转移到指定的文件夹\n");
    NMFSConsole::Log("指令格式: cd [path] ([path]: ../floder1/floder2)\n");
}