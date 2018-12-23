#include <iostream>
#include <sstream>
#include <vector>

#include "./include/io/nmfs_console.h"

int main()
{
    NMFSConsole &console = NMFSConsole::Instance();

    std::string input;
    std::vector<std::string> input_v;
    std::vector<std::string> cmd;

    while (true)
    {
        try
        {
            NMFSConsole::Log("NMFS@" + NMFSSpace::GetSpaceName());
            NMFSConsole::Log(NMFSSpace::GetCurPath());
            NMFSConsole::Log(">");

            input_v.clear();

            std::string line;
            std::getline(std::cin, line);
            
            std::istringstream iss(line);
            
            while (iss >> input)
                input_v.push_back(input);

            cmd.clear();
            for (auto i : input_v)
                if (i.length() > 0 && i[0] != 0x1a)
                    cmd.push_back(i);

            if (cmd.size() < 1)
                continue;

            bool find_cmd = false;
            for (auto i : *console._cmds)
            {
                if (i->Accept(cmd[0]))
                {
                    find_cmd = true;
                    cmd.erase(cmd.begin());

                    if (cmd.size() == 1 && cmd[0] == "-help")
                    {
                        i->Help();
                        break;
                    }

                    i->Action(cmd);
                    break;
                }
            }

            if (!find_cmd)
                NMFSConsole::Log("\'" + cmd[0] + "\' 不是正确的指令！\n");
        }
        catch (NMFSWarningException &ex)
        {
            NMFSConsole::Log(ex.GetExceptMessage() + "\n");
        }
        catch (NMFSErrorException &ex)
        {
            NMFSConsole::Log(ex.GetExceptMessage() + "\n");
        }
        catch (...)
        {
            NMFSConsole::Log("未识别的异常！\n");
        }
    }

    return 0;
}