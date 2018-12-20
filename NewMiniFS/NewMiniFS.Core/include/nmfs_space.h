#pragma once

#include <string>
#include <queue>
#include <list>

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
    unsigned __int16 last_index;  // 上一级 FAT2 索引入口；同时用来标记本块是否被删除 2B
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

struct TreeNode
{
    unsigned char name[8];
    unsigned char type[4];
    unsigned __int16 index;
};

class NMFSSpace
{
public:
    static void NewNMFSSpace(const std::string &space_name);  // 建立一个新的简单文件系统
    static NMFSSpace* GetActiveNMFSSpace();  // 获取文件系统，用于命令调用
    static void System(const std::string &space_name);  // 打开一个简单文件系统
    static void Exit();  // 退出打开的简单文件系统

    void ChangeDirectory(std::queue<std::string> &cd_path);  // 更改当前目录
    void Close();  // 关闭文件
    void Create(const std::string &file_name, const std::string &file_type);  // 创建文件
    void Delete(const std::string &del_file_name, const std::string &del_file_type);  // 删除文件
    void List(std::list<TreeNode> &tree, const bool &is_root);  // 显示目录
    void MakeDirectory(const std::string &dir_name);  // 创建子目录
    void Open(const std::string &file_name, const std::string &file_type);  // 打开文件
    void Read(unsigned char *output);  // 关闭文件
    void RemoveDirectory(const std::string &del_floder_name);  // 删除文件夹
    void Write(const unsigned char *input, const unsigned __int32 size);  // 写文件

private:
    NMFSSpace();
    NMFSSpace(const std::string &space_name);

    void HeaderToBlock();  // 将文件头、FAT 表等一些基本信息写回内存空间块中
    void FileToBlock(const File *file, const unsigned __int16 &block_index, const unsigned __int8 &offset) noexcept;  // 将文件类型块写到内存空间块中
    void BlockToFile(File *file, const unsigned __int16 &block_index, const unsigned __int8 &offset) noexcept;  // 将内存空间块写到文件类型块中
    unsigned __int16 GetFreeBlock() const noexcept;  // 从 FAT1 表中查到还有那个块是空闲的，返回块的位置 返回0xffff表示没有空闲块
    void OffFreeBlock(const unsigned __int16 &block_index) noexcept;  // 块被占用
    void GetFreeFile(unsigned __int16 &block_index, unsigned __int8 &offset) noexcept;  // 从文件夹块中得到一个空闲位
    void InitFloderBlock(const unsigned __int16 &block_index) noexcept;  // 初始化新的文件夹块，主要是给空间中空闲位标记的 last_index 标记 0xffff
    void DeleteFile(const unsigned __int16 &index) noexcept;  // 删除文件或文件夹的数据块，以及修改 FAT1、FAT2 表
    void DelDirectory(File &extra_floder);  // 删除子目录
    void CreateList(std::list<TreeNode> &tree, unsigned __int16 depth, const File &extra_floder);
    unsigned __int16 WriteFile(const unsigned char *input, const unsigned __int32 size);

    std::string _space_name;
    Header _header;
    unsigned char **_space_block;  // 512 个块，每个块 1KB
    File *_cur_floder;  // 当前文件夹
    File *_cur_file;  // 当前文件
    unsigned __int16 _cur_file_block_index;  // 记录当前文件的文件夹块索引
    unsigned __int8 _cur_file_offset;  // 记录当前文件的文件夹块内的偏移量

    std::list<CurFloder>*_path;  // 当前路径序列，包含根目录

    static NMFSSpace* volatile _global_space_pointer;
};