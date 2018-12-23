#include "../../include/command/create_command.h"
#include "../../include/io/nmfs_console.h"

CreateCommand::CreateCommand()
{

}

bool CreateCommand::Accept(const std::string &string) const
{
    if (string == "create")
        return true;
    else
        return false;
}

void CreateCommand::Action(const std::vector<std::string> &argv) const
{
    if (argv.size() > 2 || argv.size() < 1)
        throw NMFSWarningException("参数错误！");

    std::string file_name;
    std::string file_type;

    if (argv.size() == 2)
    {
        file_name = argv[0];
        file_type = argv[1];
    }
    if (argv.size() == 1)
    {
        file_name = argv[0];
        file_type = "";
    }

    if (file_name.length() > 8 || file_type.length() > 4)
        throw NMFSWarningException("参数长度错误！");

    try
    {
        NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();
        space->Create(file_name, file_type);
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
        throw NMFSErrorException("create 未知异常！");
    }
}

void CreateCommand::Help() const
{
    NMFSConsole::Log("在当前文件夹新建一个文件\n");
    NMFSConsole::Log("指令格式: create [file_name] [file_type] ([file_name]:8B [file_type]:4B)\n");
}