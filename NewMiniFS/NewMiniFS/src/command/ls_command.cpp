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

// ��ɫ����ļ��У���ɫ����ļ�
// ���Ӳ�������ʾ��ǰ�ļ�����˵����������
// ��չ���� -t ��ǰĿ¼����-rt ��Ŀ¼��
void LsCommand::Action(const std::vector<std::string> &argv) const
{

}

void LsCommand::Help() const
{

}