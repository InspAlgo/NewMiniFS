#include "../../include/command/delete_command.h"
#include "../../include/io/nmfs_console.h"

DeleteCommand::DeleteCommand()
{

}

bool DeleteCommand::Accept(const std::string &string) const
{
    if (string == "delete")
        return true;
    else
        return false;
}

void DeleteCommand::Action(const std::vector<std::string> &argv) const
{
    if (argv.size() != 2)
        throw NMFSWarningException("参数错误！");

    std::string file_name = argv[0];
    std::string file_type = argv[1];

    if (file_name.length() > 8 || file_type.length() > 4)
        throw NMFSWarningException("参数长度错误！");

    try
    {
        NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();
        space->Delete(file_name, file_type);
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
        throw NMFSErrorException("delete 未知异常！");
    }
}

void DeleteCommand::Help() const
{
    NMFSConsole::Log("删除当前文件夹下一个文件\n");
    NMFSConsole::Log("指令格式: delete [file_name] [file_type] ([file_name]:8B [file_type]:4B)\n");
}