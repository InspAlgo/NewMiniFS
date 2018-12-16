#include "../include/nmfs_space.h"

#include <iostream>

NMFSSpace* volatile NMFSSpace::_global_space_pointer = nullptr;

void  NMFSSpace::NewNMFSSpace(const std::string &space_name)
{
    if (_global_space_pointer != nullptr)
    {
        // TODO: 应该写成抛出异常
        std::cout << "Warning: 当前已打开一个简单文件系统！" << std::endl;
        return;
    }

    /* 检查当前位置下是否有同名文件 */
    FILE *space_file = nullptr;
    space_file = fopen((char*)space_name.data(), "r");
    if (space_file != nullptr)
    {
        fclose(space_file);
        // TODO: 应该写成抛出异常
        std::cout << "Warning: 当前有同名文件！" << std::endl;
        return;
    }
    fclose(space_file);

    /* 在当前位置创建一个空间文件，以后续写回磁盘 */
    space_file = fopen((char*)space_name.data(), "wb+");
    _fseeki64(space_file, 512 * 1024 - 1, SEEK_SET);
    fwrite("\0", 1, 1, space_file);
    fclose(space_file);

    /* 空间单例实例化 */
    _global_space_pointer = new NMFSSpace();
    _global_space_pointer->_space_name = space_name;
}

NMFSSpace*  NMFSSpace::GetActiveNMFSSpace()
{
    if (_global_space_pointer == nullptr)
    {
        // TODO: 应该写成抛出异常
        std::cout << "Warning: 当前并未打开一个简单文件系统！" << std::endl;
        return;
    }

    return _global_space_pointer;
}

NMFSSpace::NMFSSpace()
{
    _header.magic_num[0] = 'N';
    _header.magic_num[1] = 'M';
    _header.magic_num[2] = 'F';
    _header.magic_num[3] = 'S';

    _header.block_count = static_cast<unsigned __int32>(512);

    _header.FAT1 = new unsigned char[64];  // 512/8 = 64; 64*1B = 64B
    _header.FAT2 = new unsigned __int16[512];  // 512*2B = 1024B

    _space_block = new unsigned char*[512];  // 512 个块
    for (int i = 0; i < 512; i++)
        _space_block[i] = new unsigned char[1024];  // 每个块 1KB

    /* FAT1 初始化 */
    for (int i = 0; i < 512; i++)
    {
        unsigned char bit = static_cast<unsigned char>(1 << (i % 8));
        _header.FAT1[i / 8] &= ~bit;
    }
    _header.FAT1[0] |= static_cast<unsigned char>(1);  // 空间头占用块
    _header.FAT1[0] |= static_cast<unsigned char>(1 << 1);  // FAT2 占用块
    _header.FAT1[0] |= static_cast<unsigned char>(1 << 2);  // 根目录占用块

    _cur_floder = nullptr;
    _cur_file = nullptr;
    _path = nullptr;

    /* 根目录初始化 */
    _cur_floder = new File();

    for (int i = 0; i < 8; i++)  // 根目录无名称
        _cur_floder->name[i] = '\0';

    _cur_floder->last_index = _cur_floder->cur_index = static_cast<unsigned __int16>(2);

    for (int i = 0; i < 32; i++)  // 全部置 0，说明为文件夹类型
    {
        unsigned char bit = static_cast<unsigned char>(1 << (i % 8));
        _cur_floder->type[i / 8] &= ~bit;
    }

    _cur_floder->file_size = static_cast<unsigned __int32>(0);
    _cur_floder->block_num = static_cast<unsigned __int32>(1);

    /* TODO: 根目录写回内存块 */
    this->FileToBlock(_cur_floder, static_cast<unsigned __int16>(2), static_cast<unsigned __int8>(0));

    _path = new std::stack<CurFloder>();

    CurFloder temp;
    memcpy(temp.name, _cur_floder->name, sizeof(_cur_floder->name));
    temp.index = _cur_floder->cur_index;
    _path->push(temp);
}

NMFSSpace::NMFSSpace(const std::string &space_name)
{
    _header.magic_num[0] = 'N';
    _header.magic_num[1] = 'M';
    _header.magic_num[2] = 'F';
    _header.magic_num[3] = 'S';

    _header.block_count = static_cast<unsigned __int32>(512);

    _header.FAT1 = new unsigned char[64];  // 512/8 = 64; 64*1B = 64B
    _header.FAT2 = new unsigned __int16[512];  // 512*2B = 1024B

    _space_block = new unsigned char*[512];  // 512 个块
    for (int i = 0; i < 512; i++)
        _space_block[i] = new unsigned char[1024];  // 每个块 1KB

    _cur_floder = nullptr;
    _cur_file = nullptr;
    _path = nullptr;

    /* 将磁盘中的数据写到内存中 */
    FILE *space_file = fopen((char*)space_name.data(), "rb+");
    _fseeki64(space_file, 0LL, SEEK_SET);
    fread(&_space_block, sizeof(_space_block), 1, space_file);
    fclose(space_file);

    for (int i = 0; i < 64; i++)
        _header.FAT1[i] = _space_block[0][8 + i];

    for (int i = 0, j = 0; i < 512; i++, j += 2)
        _header.FAT2[i] = static_cast<unsigned __int16>((_space_block[1][j] << 8) | _space_block[1][j + 1]);

    _cur_floder = new File();
    this->BlockToFile(_cur_floder, static_cast<unsigned __int16>(2), static_cast<unsigned __int8>(0));

    _path = new std::stack<CurFloder>();
    CurFloder temp;
    memcpy(temp.name, _cur_floder->name, sizeof(_cur_floder->name));
    temp.index = _cur_floder->cur_index;
    _path->push(temp);
}

void NMFSSpace::System(const std::string &space_name)
{
    if (_global_space_pointer != nullptr)
    {
        // TODO: 应该写成抛出异常
        std::cout << "Warning: 当前已打开一个简单文件系统！" << std::endl;
        return;
    }

    char magic_num[4] = { '\0' };

    /* TODO: 判断当前位置是否有此文件，进行文件头魔数校验 */
    FILE *space_file = nullptr;
    space_file = fopen((char*)space_name.data(), "rb+");

    if (space_file == nullptr)
    {
        fclose(space_file);
        // TODO: 应该写成抛出异常
        std::cout << "Warning: 当前目录下无此文件！" << std::endl;
        return;
    }

    _fseeki64(space_file, 0LL, SEEK_SET);
    fread(&magic_num, sizeof(magic_num), 1, space_file);
    fclose(space_file);

    if (magic_num[0] != 'N' || magic_num[1] != 'M' 
        || magic_num[2] != 'F' || magic_num[3] != 'S')
    {
        // TODO: 应该写成抛出异常
        std::cout << "Warning: 非NMFS空间文件类型！" << std::endl;
        return;
    }

    _global_space_pointer = new NMFSSpace(space_name);
    _global_space_pointer->_space_name = space_name;
}

void NMFSSpace::Exit()
{
    if (_global_space_pointer == nullptr)
    {
        // TODO: 应该写成抛出异常
        std::cout << "Warning: 当前并未打开一个简单文件系统！" << std::endl;
        return;
    }

    _global_space_pointer->HeaderToBlock();

    /* 将内存空间的数据写回磁盘 */
    FILE *space_file = fopen((char*)_global_space_pointer->_space_name.data(), "rb+");
    _fseeki64(space_file, 0LL, SEEK_SET);
    fwrite(&_global_space_pointer->_space_block, sizeof(_global_space_pointer->_space_block), 1, space_file);
    fclose(space_file);

    /* 删除单例 */
    delete _global_space_pointer;
    _global_space_pointer = nullptr;
}

void NMFSSpace::HeaderToBlock()
{
    /* Write Header */
    for (int i = 0; i < 4; i++)
        _space_block[0][i] = _header.magic_num[i];

    _space_block[0][4] = static_cast<unsigned char>((_header.block_count & 0xff000000) >> 24);
    _space_block[0][5] = static_cast<unsigned char>((_header.block_count & 0x00ff0000) >> 16);
    _space_block[0][6] = static_cast<unsigned char>((_header.block_count & 0x0000ff00) >> 8);
    _space_block[0][7] = static_cast<unsigned char>(_header.block_count & 0x000000ff);

    for (int i = 0; i < 64; i++)
        _space_block[0][8 + i] = _header.FAT1[i];

    /* Write FAT2 */
    for (int i = 0, j=0; i < 512; i++, j+=2)
    {
        _space_block[1][j] = static_cast<unsigned char>((_header.FAT2[i] & 0xff00) >> 8);  // 取高8位
        _space_block[1][j + 1] = static_cast<unsigned char>(_header.FAT2[i] & 0x00ff);  // 取低8位
    }
}

void NMFSSpace::FileToBlock(const File *file, const unsigned __int16 &block_index, const unsigned __int8 &offset)
{
    if (offset > static_cast<unsigned __int8>(41))
    {
        // TODO: 应该写成抛出异常
        std::cout << "Error: 块内偏移量越界！" << std::endl;
        return;
    }

    if (file == nullptr)
    {
        // TODO: 应该写成抛出异常
        std::cout << "Error: 空指针！" << std::endl;
        return;
    }

    unsigned __int8 i = static_cast<unsigned __int8>(0);

    for (;i < static_cast<unsigned __int8>(8); i++)
        _space_block[block_index][offset + i] = file->name[i];

    _space_block[block_index][offset + i] = static_cast<unsigned char>((file->last_index & 0xff00) >> 8);
    i++;
    _space_block[block_index][offset + i] = static_cast<unsigned char>(file->last_index & 0x00ff);
    i++;

    _space_block[block_index][offset + i] = static_cast<unsigned char>((file->cur_index & 0xff00) >> 8);
    i++;
    _space_block[block_index][offset + i] = static_cast<unsigned char>(file->cur_index & 0x00ff);
    i++;

    for (int k = 0; k < 4; k++)
    {
        _space_block[block_index][offset + i] = file->type[k];
        i++;
    }

    _space_block[block_index][offset + i] = static_cast<unsigned char>((file->file_size & 0xff000000) >> 24);
    i++;
    _space_block[block_index][offset + i] = static_cast<unsigned char>((file->file_size & 0x00ff0000) >> 16);
    i++;
    _space_block[block_index][offset + i] = static_cast<unsigned char>((file->file_size & 0x0000ff00) >> 8);
    i++;
    _space_block[block_index][offset + i] = static_cast<unsigned char>(file->file_size & 0x000000ff);
    i++;

    _space_block[block_index][offset + i] = static_cast<unsigned char>((file->block_num & 0xff000000) >> 24);
    i++;
    _space_block[block_index][offset + i] = static_cast<unsigned char>((file->block_num & 0x00ff0000) >> 16);
    i++;
    _space_block[block_index][offset + i] = static_cast<unsigned char>((file->block_num & 0x0000ff00) >> 8);
    i++;
    _space_block[block_index][offset + i] = static_cast<unsigned char>(file->block_num & 0x000000ff);
    i++;
}

void NMFSSpace::BlockToFile(File *file, const unsigned __int16 &block_index, const unsigned __int8 &offset)
{
    if (offset > static_cast<unsigned __int8>(41))
    {
        // TODO: 应该写成抛出异常
        std::cout << "Error: 块内偏移量越界！" << std::endl;
        return;
    }

    if (file == nullptr)
    {
        // TODO: 应该写成抛出异常
        std::cout << "Error: 空指针！" << std::endl;
        return;
    }

    unsigned __int8 i = static_cast<unsigned __int8>(0);

    for (; i < static_cast<unsigned __int8>(8); i++)
        file->name[i] = _space_block[block_index][offset + i];

    file->last_index = static_cast<unsigned __int16>(0);
    file->last_index |= static_cast<unsigned __int16>(_space_block[block_index][offset + i] << 8);
    i++;
    file->last_index |= static_cast<unsigned __int16>(_space_block[block_index][offset + i]);
    i++;

    file->cur_index = static_cast<unsigned __int16>(0);
    file->cur_index |= static_cast<unsigned __int16>(_space_block[block_index][offset + i] << 8);
    i++;
    file->cur_index |= static_cast<unsigned __int16>(_space_block[block_index][offset + i]);
    i++;

    for (int k = 0; k < 4; k++)
    {
        file->type[k] = _space_block[block_index][offset + i];
        i++;
    }

    file->file_size = static_cast<unsigned __int32>(0);
    file->file_size |= static_cast<unsigned __int32>(_space_block[block_index][offset + i] << 24);
    i++;
    file->file_size |= static_cast<unsigned __int32>(_space_block[block_index][offset + i] << 16);
    i++;
    file->file_size |= static_cast<unsigned __int32>(_space_block[block_index][offset + i] << 8);
    i++;
    file->file_size |= static_cast<unsigned __int32>(_space_block[block_index][offset + i]);
    i++;

    file->block_num = static_cast<unsigned __int32>(0);
    file->block_num |= static_cast<unsigned __int32>(_space_block[block_index][offset + i] << 24);
    i++;
    file->block_num |= static_cast<unsigned __int32>(_space_block[block_index][offset + i] << 16);
    i++;
    file->block_num |= static_cast<unsigned __int32>(_space_block[block_index][offset + i] << 8);
    i++;
    file->block_num |= static_cast<unsigned __int32>(_space_block[block_index][offset + i]);
    i++;
}

void NMFSSpace::ChangeDirectory()
{
    // 1. 分割路径
    std::queue<std::string>cd_path;  // 待转移路径

    // 2. 修改路径序列
    while (!cd_path.empty())
    {
        std::string cur_floder = cd_path.front();
        cd_path.pop();

        if (cur_floder == ".")  // 还是在当前文件夹
            continue;
        else if (cur_floder == "..")  // 向上一级文件夹转移
        {
            unsigned __int16 to_index = _cur_floder->last_index;
            this->BlockToFile(_cur_floder, to_index, static_cast<unsigned __int16>(0));

            if (_path->size() > 1)  // 当前位置出栈
                _path->pop();
            else if (_path->size() == 1)  // 已经到达根目录，不能再向上一级文件夹转移
                continue;
            else
            {
                // TODO: 抛出异常
                std::cout << "ERROR: 位置转移错误!" << std::endl;
                return;
            }

            // 要转移的位置进栈
            CurFloder temp;
            memcpy(temp.name, _cur_floder->name, sizeof(_cur_floder->name));
            temp.index = _cur_floder->cur_index;
            _path->push(temp);
        }
        else  // 向下一级文件夹转移
        {
            // a. 文件夹名称合法性判断
            if (cur_floder.length() > 8)
            {
                // TODO: 报告错误
                std::cout << "目录名错误" << std::endl;
                return;
            }

            // b. 先判断下一级文件夹是否存在
            // c. 转移
        }
    }
}

void NMFSSpace::Close()
{

}

void NMFSSpace::Create()
{

}

void NMFSSpace::Delete()
{

}

void NMFSSpace::List()
{

}

void NMFSSpace::MakeDirectory()
{

}

void NMFSSpace::Open()
{

}

void NMFSSpace::Read()
{

}

void NMFSSpace::RemoveDirectory()
{

}

void NMFSSpace::Write()
{

}