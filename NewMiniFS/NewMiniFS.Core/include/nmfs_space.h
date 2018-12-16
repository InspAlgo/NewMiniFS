#pragma once

#include <string>
#include <array>
#include <stack>
#include <queue>

struct Header  // 空间头  24B
{
    unsigned char magic_num[4];  // 空间文件头魔数 NMFS  4B
    unsigned __int32 block_count;  // 空间总块数  4B
    unsigned char *FAT1;  // 判断块是否可用  8B
    unsigned __int16 *FAT2;  // 索引表  8B
};

struct File  // 文件(夹)  24B
{
    unsigned char name[8];  // 文件夹名称  8B
    unsigned __int16 last_index;  // 上一级 FAT2 索引入口  2B
    unsigned __int16 cur_index;  // 本级 FAT2 索引入口  2B
    unsigned char type[4];  // 文件类型 用来判别是文件夹还是文件以及后缀  4B
    unsigned __int32 file_size;  // 如果是文件夹，则此值记录当前文件夹下所包含的文件数；如果是文件，则记录文件大小  4B
    unsigned __int32 block_num;  // 占用块数  4B
};

struct CurFloder
{
    unsigned char name[8];
    unsigned __int16 index;
};

class NMFSSpace
{
public:
    static void NewNMFSSpace(const std::string &space_name);  // 建立一个新的简单文件系统
    static NMFSSpace* GetActiveNMFSSpace();  // 获取文件系统，用于命令调用
    static void System(const std::string &space_name);  // 打开一个简单文件系统
    static void Exit();  // 退出打开的简单文件系统

    void ChangeDirectory();  // 更改当前目录
    void Close();  // 关闭文件
    void Create();  // 创建文件
    void Delete();  // 删除文件
    void List();  // 显示目录
    void MakeDirectory();  // 创建子目录
    void Open();  // 打开文件
    void Read();  // 关闭文件
    void RemoveDirectory();  // 删除子目录
    void Write();  // 写文件

private:
    NMFSSpace();
    NMFSSpace(const std::string &space_name);

    void HeaderToBlock();  // 将文件头、FAT 表等一些基本信息写回内存空间块中
    void FileToBlock(const File *file, const unsigned __int16 &block_index, const unsigned __int8 &offset);  // 将文件类型块写到内存空间块中
    void BlockToFile(File *file, const unsigned __int16 &block_index, const unsigned __int8 &offset);  // 将内存空间块写到文件类型块中

    std::string _space_name;
    Header _header;
    unsigned char **_space_block;  // 512 个块，每个块 1KB
    File *_cur_floder;  // 当前文件夹
    File *_cur_file;  // 当前文件

    std::stack<CurFloder>*_path;  // 当前路径序列，包含根目录

    static NMFSSpace* volatile _global_space_pointer;
};