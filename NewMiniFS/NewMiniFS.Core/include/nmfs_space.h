#pragma once

#include <string>
#include <array>

struct Header  // �ռ�ͷ  24B
{
    unsigned char magic_num[4];  // �ռ��ļ�ͷħ�� NMFS  4B
    unsigned __int32 block_count;  // �ռ��ܿ���  4B
    unsigned char *FAT1;  // �жϿ��Ƿ����  8B
    unsigned __int16 *FAT2;  // ������  8B
};

struct File  // �ļ�(��)  32B
{
    unsigned char name[8];  // �ļ�������  8B
    unsigned __int32 last_index;  // ��һ�� FAT2 �������  4B
    unsigned __int32 cur_index;  // ���� FAT2 �������  4B
    unsigned char name[4];  // �ļ����� �����б����ļ��л����ļ��Լ���׺  4B
    unsigned __int32 file_size;  // ������ļ��У����ֵ��¼��ǰ�ļ��������������ļ�����������ļ������¼�ļ���С  4B
    unsigned __int32 block_num;  // ռ�ÿ���  4B
    unsigned __int32 unknown;  // �����ֽڣ�δ���� 4B
};

class NMFSSpace
{
public:
    static NMFSSpace* NewNMFSSpace(const std::string &space_name);
    static NMFSSpace* GetActiveNMFSSpace();
    static void System();

    void ChangeDirectory();
    void Close();
    void Create();
    void Delete();
    void Exit();
    void List();
    void MakeDirectory();
    void Open();
    void Read();
    void RemoveDirectory();
    void Write();

private:
    NMFSSpace() = delete;
    NMFSSpace(const std::string &space_name);

    std::array<unsigned char*, 512>*_space_block;  // 512 ���飬ÿ���� 1KB
    Header header;

    static NMFSSpace* volatile _global_space_pointer;
};