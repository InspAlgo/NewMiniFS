#include "../../include/command/ls_command.h"

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

// 蓝色标记文件夹，绿色标记文件
// 不加参数则显示当前文件夹下说包含的内容
// 拓展参数 -t 当前目录树；-rt 根目录树
void LsCommand::Action(const std::vector<std::string> &argv) const
{

}

void LsCommand::Help() const
{

}