#include "../../include/command/ls_command.h"
#include "../../include/io/nmfs_console.h"

LsCommand::LsCommand()
{

}

bool LsCommand::Accept(const std::string &string) const
{
    if (string == "ls")
        return true;
    else
        return false;
}

void LsCommand::Action(const std::vector<std::string> &argv) const
{
    if (argv.size() > 1)
        throw NMFSWarningException("参数错误！");
    if (argv.size() > 1 && argv[0] != "-t" && argv[0] != "-rt")
        throw NMFSWarningException("参数错误！");

    bool is_root = true;
    if (argv.size() == 0 || (argv.size() == 1 && argv[0] == "-t"))
        is_root = false;

    std::list<TreeNode>tree;

    try
    {
        NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();
        space->List(tree, is_root);

        std::string blank;
        for (auto i : tree)
        {
            blank = "";

            for (unsigned __int16 blank_num = static_cast<unsigned __int16>(0);
                blank_num < i.index; blank_num++)
                blank += "  ";

            bool flag1 = true;
            for (int k = 0; k < 8; k++)
            {
                if (i.name[k] != 0x00)
                    flag1 = false;
            }

            if (flag1)
                blank += NMFSSpace::GetSpaceName();
            else
            {
                char cname[9] = { '\0' };
                memcpy(cname, i.name, 8);
                char ctype[5] = { '\0' };
                memcpy(ctype, i.type, 4);
                std::string stype = std::string(ctype);

                stype = "." + stype;

                if (i.type[0] == 0x00 && i.type[1] == 0x00
                    && i.type[2] == 0x00 && i.type[3] == 0x00)
                    stype = "";

                if (i.type[0] == 0xff && i.type[1] == 0xff
                    && i.type[2] == 0xff && i.type[3] == 0xff)
                    stype = " *";

                blank += " " + std::string(cname) + stype;
            }

            NMFSConsole::Log(blank + "\n");
        }
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
        throw NMFSErrorException("ls 未知异常！");
    }
}

void LsCommand::Help() const
{
    NMFSConsole::Log("显示文件夹信息\n");
    NMFSConsole::Log("指令格式: ls [m] ([m]可以省略,否则为 -t(当前目录) 或 -rt(根目录),带*的为文件夹)\n");
}