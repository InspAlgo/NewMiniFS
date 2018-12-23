#include "../../include/command/read_command.h"
#include "../../include/io/nmfs_console.h"

ReadCommand::ReadCommand()
{

}

bool ReadCommand::Accept(const std::string &string) const
{
    if (string == "read")
        return true;
    else
        return false;
}

void ReadCommand::Action(const std::vector<std::string> &argv) const
{
    if (argv.size() != 0)
        throw NMFSWarningException("参数错误！");

    try
    {
        NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

        unsigned char *output = nullptr;
        space->Read(output);

        if (output != nullptr)
            NMFSConsole::Log((char*)output);
        else
            NMFSConsole::Log("\n");

        delete[] output;
        output = nullptr;
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
        throw NMFSErrorException("read 未知异常！");
    }
}

void ReadCommand::Help() const
{
    NMFSConsole::Log("读已打开的文件内容并输出到控制台上\n");
    NMFSConsole::Log("指令格式: read\n");
}