#pragma once

#include <string>
#include <array>

struct Header  // 空间头  24B
{
    unsigned char magic_num[4];  // 空间文件头魔数 NMFS  4B
    unsigned __int32 block_count;  // 空间总块数  4B
    unsigned char *FAT1;  // 判断块是否可用  8B
    unsigned __int16 *FAT2;  // 索引表  8B
};

struct File  // 文件(夹)  32B
{
    unsigned char name[8];  // 文件夹名称  8B
    unsigned __int32 last_index;  // 上一级 FAT2 索引入口  4B
    unsigned __int32 cur_index;  // 本级 FAT2 索引入口  4B
    unsigned char name[4];  // 文件类型 用来判别是文件夹还是文件以及后缀  4B
    unsigned __int32 file_size;  // 如果是文件夹，则此值记录当前文件夹下所包含的文件数；如果是文件，则记录文件大小  4B
    unsigned __int32 block_num;  // 占用块数  4B
    unsigned __int32 unknown;  // 保留字节，未定义 4B
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

    std::array<unsigned char*, 512>*_space_block;  // 512 个块，每个块 1KB
    Header header;

    static NMFSSpace* volatile _global_space_pointer;
};