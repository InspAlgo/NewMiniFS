#include "../../include/command/new_command.h"
#include "../../include/io/nmfs_console.h"

NewCommand::NewCommand()
{

}

bool NewCommand::Accept(const std::string &string) const
{
    if (string == "new")
        return true;
    else
        return false;
}

void NewCommand::Action(const std::vector<std::string> &argv) const
{
    if (argv.size() != 1)
        throw NMFSWarningException("参数错误！");

    try
    {
        NMFSSpace::NewNMFSSpace(argv[0]);
        NMFSSpace::Exit();
        NMFSConsole::Log("\'" + argv[0] + "\' 创建成功!\n");
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
        throw NMFSErrorException("new 未知异常！");
    }
}

void NewCommand::Help() const
{
    NMFSConsole::Log("新建一个空间\n");
    NMFSConsole::Log("指令格式: new [space_name]\n");
}