#include "../../include/command/write_command.h"
#include "../../include/io/nmfs_console.h"

#include <iostream>

WriteCommand::WriteCommand()
{

}

bool WriteCommand::Accept(const std::string &string) const
{
    if (string == "write")
        return true;
    else
        return false;
}

void WriteCommand::Action(const std::vector<std::string> &argv) const
{
    if (argv.size() != 0)
        throw NMFSWarningException("参数错误！");

    try
    {
        NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

        std::string in = "";
        std::string temp = "";

        while (std::getline(std::cin, temp))
            in += temp + "\n";

        unsigned char *input = new unsigned char[in.length() + 1]{ '\0' };
        for (int i = 0; i < in.length(); i++)
            input[i] = static_cast<unsigned char>(in[i]);
        unsigned __int32 size = in.length();

        space->Write(input, size);
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
        throw NMFSErrorException("write 未知异常！");
    }
}

void WriteCommand::Help() const
{
    NMFSConsole::Log("向已打开的文件覆盖写入\n");
    NMFSConsole::Log("指令格式: write (EOF结束输入)\n");
}