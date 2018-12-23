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
        throw NMFSWarningException("��������");

    try
    {
        NMFSSpace::NewNMFSSpace(argv[0]);
        NMFSSpace::Exit();
        NMFSConsole::Log("\'" + argv[0] + "\' �����ɹ�!\n");
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
        throw NMFSErrorException("new δ֪�쳣��");
    }
}

void NewCommand::Help() const
{
    NMFSConsole::Log("�½�һ���ռ�\n");
    NMFSConsole::Log("ָ���ʽ: new [space_name]\n");
}