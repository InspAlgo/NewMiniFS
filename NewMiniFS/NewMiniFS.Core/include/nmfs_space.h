#pragma once

#include <string>
#include <array>
#include <stack>
#include <queue>

struct Header  // �ռ�ͷ  24B
{
    unsigned char magic_num[4];  // �ռ��ļ�ͷħ�� NMFS  4B
    unsigned __int32 block_count;  // �ռ��ܿ���  4B
    unsigned char *FAT1;  // �жϿ��Ƿ����  8B
    unsigned __int16 *FAT2;  // ������  8B
};

struct File  // �ļ�(��)  24B
{
    unsigned char name[8];  // �ļ�������  8B
    unsigned __int16 last_index;  // ��һ�� FAT2 �������  2B
    unsigned __int16 cur_index;  // ���� FAT2 �������  2B
    unsigned char type[4];  // �ļ����� �����б����ļ��л����ļ��Լ���׺  4B
    unsigned __int32 file_size;  // ������ļ��У����ֵ��¼��ǰ�ļ��������������ļ�����������ļ������¼�ļ���С  4B
    unsigned __int32 block_num;  // ռ�ÿ���  4B
};

struct CurFloder
{
    unsigned char name[8];
    unsigned __int16 index;
};

class NMFSSpace
{
public:
    static void NewNMFSSpace(const std::string &space_name);  // ����һ���µļ��ļ�ϵͳ
    static NMFSSpace* GetActiveNMFSSpace();  // ��ȡ�ļ�ϵͳ�������������
    static void System(const std::string &space_name);  // ��һ�����ļ�ϵͳ
    static void Exit();  // �˳��򿪵ļ��ļ�ϵͳ

    void ChangeDirectory();  // ���ĵ�ǰĿ¼
    void Close();  // �ر��ļ�
    void Create();  // �����ļ�
    void Delete();  // ɾ���ļ�
    void List();  // ��ʾĿ¼
    void MakeDirectory();  // ������Ŀ¼
    void Open();  // ���ļ�
    void Read();  // �ر��ļ�
    void RemoveDirectory();  // ɾ����Ŀ¼
    void Write();  // д�ļ�

private:
    NMFSSpace();
    NMFSSpace(const std::string &space_name);

    void HeaderToBlock();  // ���ļ�ͷ��FAT ���һЩ������Ϣд���ڴ�ռ����
    void FileToBlock(const File *file, const unsigned __int16 &block_index, const unsigned __int8 &offset);  // ���ļ����Ϳ�д���ڴ�ռ����
    void BlockToFile(File *file, const unsigned __int16 &block_index, const unsigned __int8 &offset);  // ���ڴ�ռ��д���ļ����Ϳ���

    std::string _space_name;
    Header _header;
    unsigned char **_space_block;  // 512 ���飬ÿ���� 1KB
    File *_cur_floder;  // ��ǰ�ļ���
    File *_cur_file;  // ��ǰ�ļ�

    std::stack<CurFloder>*_path;  // ��ǰ·�����У�������Ŀ¼

    static NMFSSpace* volatile _global_space_pointer;
};