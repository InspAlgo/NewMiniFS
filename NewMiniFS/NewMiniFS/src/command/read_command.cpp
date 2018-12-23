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
        throw NMFSWarningException("��������");

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
        throw NMFSErrorException("read δ֪�쳣��");
    }
}

void ReadCommand::Help() const
{
    NMFSConsole::Log("���Ѵ򿪵��ļ����ݲ����������̨��\n");
    NMFSConsole::Log("ָ���ʽ: read\n");
}