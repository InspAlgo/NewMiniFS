#pragma once

#include <string>
#include <queue>
#include <list>

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
    unsigned __int16 last_index;  // ��һ�� FAT2 ������ڣ�ͬʱ������Ǳ����Ƿ�ɾ�� 2B
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

struct TreeNode
{
    unsigned char name[8];
    unsigned char type[4];
    unsigned __int16 index;
};

class NMFSSpace
{
public:
    static void NewNMFSSpace(const std::string &space_name);  // ����һ���µļ��ļ�ϵͳ
    static NMFSSpace* GetActiveNMFSSpace();  // ��ȡ�ļ�ϵͳ�������������
    static void System(const std::string &space_name);  // ��һ�����ļ�ϵͳ
    static void Exit();  // �˳��򿪵ļ��ļ�ϵͳ

    void ChangeDirectory(std::queue<std::string> &cd_path);  // ���ĵ�ǰĿ¼
    void Close();  // �ر��ļ�
    void Create(const std::string &file_name, const std::string &file_type);  // �����ļ�
    void Delete(const std::string &del_file_name, const std::string &del_file_type);  // ɾ���ļ�
    void List(std::list<TreeNode> &tree, const bool &is_root);  // ��ʾĿ¼
    void MakeDirectory(const std::string &dir_name);  // ������Ŀ¼
    void Open(const std::string &file_name, const std::string &file_type);  // ���ļ�
    void Read(unsigned char *output);  // �ر��ļ�
    void RemoveDirectory(const std::string &del_floder_name);  // ɾ���ļ���
    void Write(const unsigned char *input, const unsigned __int32 size);  // д�ļ�

private:
    NMFSSpace();
    NMFSSpace(const std::string &space_name);

    void HeaderToBlock();  // ���ļ�ͷ��FAT ���һЩ������Ϣд���ڴ�ռ����
    void FileToBlock(const File *file, const unsigned __int16 &block_index, const unsigned __int8 &offset) noexcept;  // ���ļ����Ϳ�д���ڴ�ռ����
    void BlockToFile(File *file, const unsigned __int16 &block_index, const unsigned __int8 &offset) noexcept;  // ���ڴ�ռ��д���ļ����Ϳ���
    unsigned __int16 GetFreeBlock() const noexcept;  // �� FAT1 ���в鵽�����Ǹ����ǿ��еģ����ؿ��λ�� ����0xffff��ʾû�п��п�
    void OffFreeBlock(const unsigned __int16 &block_index) noexcept;  // �鱻ռ��
    void GetFreeFile(unsigned __int16 &block_index, unsigned __int8 &offset) noexcept;  // ���ļ��п��еõ�һ������λ
    void InitFloderBlock(const unsigned __int16 &block_index) noexcept;  // ��ʼ���µ��ļ��п飬��Ҫ�Ǹ��ռ��п���λ��ǵ� last_index ��� 0xffff
    void DeleteFile(const unsigned __int16 &index) noexcept;  // ɾ���ļ����ļ��е����ݿ飬�Լ��޸� FAT1��FAT2 ��
    void DelDirectory(File &extra_floder);  // ɾ����Ŀ¼
    void CreateList(std::list<TreeNode> &tree, unsigned __int16 depth, const File &extra_floder);
    unsigned __int16 WriteFile(const unsigned char *input, const unsigned __int32 size);

    std::string _space_name;
    Header _header;
    unsigned char **_space_block;  // 512 ���飬ÿ���� 1KB
    File *_cur_floder;  // ��ǰ�ļ���
    File *_cur_file;  // ��ǰ�ļ�
    unsigned __int16 _cur_file_block_index;  // ��¼��ǰ�ļ����ļ��п�����
    unsigned __int8 _cur_file_offset;  // ��¼��ǰ�ļ����ļ��п��ڵ�ƫ����

    std::list<CurFloder>*_path;  // ��ǰ·�����У�������Ŀ¼

    static NMFSSpace* volatile _global_space_pointer;
};