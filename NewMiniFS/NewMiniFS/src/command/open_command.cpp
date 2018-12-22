#include "../../include/command/open_command.h"
#include "../../include/io/nmfs_console.h"

OpenCommand::OpenCommand()
{

}

bool OpenCommand::Accept(const std::string &string) const
{
    if (string == "open")
        return true;
    else
        return false;
}

void OpenCommand::Action(const std::vector<std::string> &argv) const
{
    if (argv.size() > 2 || argv.size() < 1)
        throw NMFSWarningException("��������");

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
        file_type = "    ";
    }

    if (file_name.length() > 8 || file_type.length() > 4)
        throw NMFSWarningException("�������ȴ���");

    try
    {
        NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();
        space->Open(file_name, file_type);
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
        throw NMFSErrorException("open δ֪�쳣��");
    }
}

void OpenCommand::Help() const
{
    NMFSConsole::Log("�ڵ�ǰ�ļ����´�һ���ļ�\n");
    NMFSConsole::Log("ָ���ʽ: open [file_name] [file_type] ([file_name]:8B [file_type]:4B)\n");
}