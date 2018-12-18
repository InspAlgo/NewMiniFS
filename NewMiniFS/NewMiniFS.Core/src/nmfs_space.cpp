#include "../include/nmfs_space.h"

#include <iostream>
#include <queue>

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

    /* FAT 初始化 */
    for (int i = 0; i < 512; i++)
    {
        unsigned char bit = static_cast<unsigned char>(1 << (i % 8));
        _header.FAT1[i / 8] &= ~bit;

        _header.FAT2[i] = 0xffff;
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

    for (int i = 0; i < 32; i++)  // 全部置 1，说明为文件夹类型
    {
        unsigned char bit = static_cast<unsigned char>(1 << (i % 8));
        _cur_floder->type[i / 8] |= bit;
    }

    _cur_floder->file_size = static_cast<unsigned __int32>(0);
    _cur_floder->block_num = static_cast<unsigned __int32>(1);

    this->FileToBlock(_cur_floder, static_cast<unsigned __int16>(2), static_cast<unsigned __int8>(0));
    this->InitFloderBlock(static_cast<unsigned __int16>(2));

    _path = new std::list<CurFloder>();

    CurFloder temp;
    memcpy(temp.name, _cur_floder->name, sizeof(_cur_floder->name));
    temp.index = _cur_floder->cur_index;
    _path->push_back(temp);
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

    _path = new std::list<CurFloder>();
    CurFloder temp;
    memcpy(temp.name, _cur_floder->name, sizeof(_cur_floder->name));
    temp.index = _cur_floder->cur_index;
    _path->push_back(temp);
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

    /* 判断当前位置是否有此文件，进行文件头魔数校验 */
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

    unsigned __int16 i = static_cast<unsigned __int16>(0);
    unsigned __int16 cur_p = static_cast<unsigned __int16>(offset) * static_cast<unsigned __int16>(24);

    for (;i < static_cast<unsigned __int16>(8); i++)
        _space_block[block_index][cur_p + i] = file->name[i];

    _space_block[block_index][cur_p + i] = static_cast<unsigned char>((file->last_index & 0xff00) >> 8);
    i++;
    _space_block[block_index][cur_p + i] = static_cast<unsigned char>(file->last_index & 0x00ff);
    i++;

    _space_block[block_index][cur_p + i] = static_cast<unsigned char>((file->cur_index & 0xff00) >> 8);
    i++;
    _space_block[block_index][cur_p + i] = static_cast<unsigned char>(file->cur_index & 0x00ff);
    i++;

    for (int k = 0; k < 4; k++)
    {
        _space_block[block_index][cur_p + i] = file->type[k];
        i++;
    }

    _space_block[block_index][cur_p + i] = static_cast<unsigned char>((file->file_size & 0xff000000) >> 24);
    i++;
    _space_block[block_index][cur_p + i] = static_cast<unsigned char>((file->file_size & 0x00ff0000) >> 16);
    i++;
    _space_block[block_index][cur_p + i] = static_cast<unsigned char>((file->file_size & 0x0000ff00) >> 8);
    i++;
    _space_block[block_index][cur_p + i] = static_cast<unsigned char>(file->file_size & 0x000000ff);
    i++;

    _space_block[block_index][cur_p + i] = static_cast<unsigned char>((file->block_num & 0xff000000) >> 24);
    i++;
    _space_block[block_index][cur_p + i] = static_cast<unsigned char>((file->block_num & 0x00ff0000) >> 16);
    i++;
    _space_block[block_index][cur_p + i] = static_cast<unsigned char>((file->block_num & 0x0000ff00) >> 8);
    i++;
    _space_block[block_index][cur_p + i] = static_cast<unsigned char>(file->block_num & 0x000000ff);
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

    unsigned __int16 i = static_cast<unsigned __int16>(0);
    unsigned __int16 cur_p = static_cast<unsigned __int16>(offset) * static_cast<unsigned __int16>(24);

    for (; i < static_cast<unsigned __int16>(8); i++)
        file->name[i] = _space_block[block_index][cur_p + i];

    file->last_index = static_cast<unsigned __int16>(0);
    file->last_index |= static_cast<unsigned __int16>(_space_block[block_index][cur_p + i] << 8);
    i++;
    file->last_index |= static_cast<unsigned __int16>(_space_block[block_index][cur_p + i]);
    i++;

    file->cur_index = static_cast<unsigned __int16>(0);
    file->cur_index |= static_cast<unsigned __int16>(_space_block[block_index][cur_p + i] << 8);
    i++;
    file->cur_index |= static_cast<unsigned __int16>(_space_block[block_index][cur_p + i]);
    i++;

    for (int k = 0; k < 4; k++)
    {
        file->type[k] = _space_block[block_index][cur_p + i];
        i++;
    }

    file->file_size = static_cast<unsigned __int32>(0);
    file->file_size |= static_cast<unsigned __int32>(_space_block[block_index][cur_p + i] << 24);
    i++;
    file->file_size |= static_cast<unsigned __int32>(_space_block[block_index][cur_p + i] << 16);
    i++;
    file->file_size |= static_cast<unsigned __int32>(_space_block[block_index][cur_p + i] << 8);
    i++;
    file->file_size |= static_cast<unsigned __int32>(_space_block[block_index][cur_p + i]);
    i++;

    file->block_num = static_cast<unsigned __int32>(0);
    file->block_num |= static_cast<unsigned __int32>(_space_block[block_index][cur_p + i] << 24);
    i++;
    file->block_num |= static_cast<unsigned __int32>(_space_block[block_index][cur_p + i] << 16);
    i++;
    file->block_num |= static_cast<unsigned __int32>(_space_block[block_index][cur_p + i] << 8);
    i++;
    file->block_num |= static_cast<unsigned __int32>(_space_block[block_index][cur_p + i]);
    i++;
}

unsigned __int16 NMFSSpace::GetFreeBlock() const
{
    /* FAT1 遍历查询 */
    for (int i = 0; i < 512; i++)
    {
        unsigned char bit = static_cast<unsigned char>(1 << (i % 8));

        if (_header.FAT1[i / 8] & bit)  // _header.FAT1[i / 8] & bit <= _header.FAT1[i / 8] & bit  == bit
            continue;

        return static_cast<unsigned __int16>(i);
    }

    return 0xffff;  // 未找到空闲块
}

void NMFSSpace::OffFreeBlock(const unsigned __int16 &block_index)
{
    int i = static_cast<int>(block_index);
    unsigned char bit = static_cast<unsigned char>(1 << (i % 8));
    _header.FAT1[i / 8] |= bit;
}

void NMFSSpace::GetFreeFile(unsigned __int16 &block_index, unsigned __int8 &offset)
{
    File cur_floder;
    this->BlockToFile(&cur_floder, block_index, static_cast<unsigned __int8>(0));

    // 先遍历本块
    File file_temp;
    bool flag_find = false;
    unsigned __int8 offset_p;
    unsigned __int8 offset_end = static_cast<unsigned __int8>(1);
    unsigned __int16 index_temp = block_index;

    if (cur_floder.file_size < static_cast<unsigned __int8>(42))
        offset_end = static_cast<unsigned __int8>(cur_floder.file_size);
    else
        offset_end = static_cast<unsigned __int8>(41);

    for (offset_p = static_cast<unsigned __int8>(1);
        offset_p <= offset_end; offset_p++)
    {
        this->BlockToFile(&file_temp, index_temp, offset_p);

        if (file_temp.last_index == 0xfffff)
        {
            block_index = index_temp;
            offset = offset_p;
            return;
        }
    }

    // 如果有拓展块，再遍历拓展块
    if (cur_floder.block_num > static_cast<unsigned __int16>(1))
    {
        unsigned __int16 end = cur_floder.file_size;

        // 统计拓展块
        while (!flag_find && _header.FAT2[index_temp] != 0xffff)
        {
            index_temp = _header.FAT2[index_temp];
            end -= static_cast<unsigned __int16>(41);

            if (end > static_cast<unsigned __int16>(41))
                offset_end = static_cast<unsigned __int8>(41);
            else
                offset_end = static_cast<unsigned __int8>(end);

            for (offset_p = static_cast<unsigned __int8>(1);
                offset_p <= offset_end; offset_p++)
            {
                this->BlockToFile(&file_temp, index_temp, offset_p);

                if (file_temp.last_index == 0xfffff)
                {
                    block_index = index_temp;
                    offset = offset_p;
                    return;
                }
            }
        }
    }

    // 没有找到空闲块
    block_index = 0xffff;
    offset = 0xff;
}

void NMFSSpace::InitFloderBlock(const unsigned __int16 &block_index)
{
    for (unsigned __int16 offset_p = static_cast<unsigned __int16>(24); 
        offset_p < static_cast<unsigned __int16>(1024); offset_p++)
    {
        _space_block[block_index][offset_p] = 0x00;
        // 将 last_index 位置为 0xffff
        if(offset_p / static_cast<unsigned __int16>(24) + static_cast<unsigned __int16>(10) == offset_p)
        {
            _space_block[block_index][offset_p - static_cast<unsigned __int16>(1)] = 0xff;
            _space_block[block_index][offset_p - static_cast<unsigned __int16>(2)] = 0xff;
        }
    }
}

void NMFSSpace::DeleteFile(const unsigned __int16 &index)
{
    unsigned __int16 index_temp = index;
    unsigned __int16 temp;

    while (index_temp != 0xffff)
    {
        for (int i = 0; i < 1024; i++)
            _space_block[index_temp][i] = 0x0;

        int ii = static_cast<int>(index_temp);
        unsigned char bit = static_cast<unsigned char>(1 << (ii % 8));
        _header.FAT1[ii / 8] &= ~bit;

        temp = index_temp;
        index_temp = _header.FAT2[index_temp];
        _header.FAT2[temp] = 0xffff;
    }
}

void NMFSSpace::ChangeDirectory()
{
    // TODO: 分割路径
    std::queue<std::string>cd_path;  // 待转移路径

    // 准备拷贝路径容器，后续转移使用拷贝路径容器，防止抛出异常时不会改变原始路径信息
    std::list<CurFloder>path_copy;
    for (auto temp : *_path)
        path_copy.push_back(temp);

    File cur_floder;
    memcpy(&cur_floder, _cur_floder, sizeof(_cur_floder));

    // 修改路径序列
    while (!cd_path.empty())
    {
        std::string str_floder = cd_path.front();
        cd_path.pop();

        if (str_floder == ".")  // 还是在当前文件夹
            continue;
        // 向上一级文件夹转移
        else if (str_floder == "..")  
        {
            unsigned __int16 to_index = _cur_floder->last_index;
            this->BlockToFile(_cur_floder, to_index, static_cast<unsigned __int8>(0));

            if (path_copy.size() > 1)
                path_copy.pop_back();
            else if (path_copy.size() == 1)  // 已经到达根目录，不能再向上一级文件夹转移
                continue;
            else
            {
                // TODO: 抛出异常
                std::cout << "Error: 不存在此路径!" << std::endl;
                return;
            }

            // 要转移的位置进栈
            CurFloder temp;
            memcpy(temp.name, _cur_floder->name, sizeof(_cur_floder->name));
            temp.index = _cur_floder->cur_index;
            path_copy.push_back(temp);

        }
        // 向下一级文件夹转移
        else  
        {
            // a. 文件夹名称合法性判断
            if (str_floder.length() > 8)
            {
                // TODO: 抛出异常
                std::cout << "Error: 不存在此路径！" << std::endl;
                return;
            }

            char name_c[8] = { '\0' };
            for (int i = 0; i < str_floder.length(); i++)
                name_c[i] = str_floder[i];

            unsigned __int16 index_temp = cur_floder.cur_index;  // 获取当前文件夹入口
            
            bool flag_find = false;  // 标记是否查询到

            // 先遍历本块
            File file_temp;
            unsigned __int8 offset_p;
            unsigned __int8 offset_end = static_cast<unsigned __int8>(1);

            if (_cur_floder->file_size < static_cast<unsigned __int8>(42))
                offset_end = static_cast<unsigned __int8>(cur_floder.file_size);
            else
                offset_end = static_cast<unsigned __int8>(41);

            for (offset_p = static_cast<unsigned __int8>(1);
                offset_p <= offset_end; offset_p++)
            {
                this->BlockToFile(&file_temp, index_temp, offset_p);

                if (file_temp.last_index == 0xfffff || file_temp.type[0] != 0xf || file_temp.type[1] != 0xf
                    || file_temp.type[2] != 0xf || file_temp.type[3] != 0xf)
                    continue;

                bool flag_name = true;

                for (int i = 0; i < 8; i++)
                {
                    if (name_c[i] != file_temp.name[i])
                    {
                        flag_name = false;
                        break;
                    }
                }

                if (flag_name)
                {
                    CurFloder temp;
                    memcpy(temp.name, file_temp.name, sizeof(file_temp.name));
                    temp.index = file_temp.cur_index;
                    path_copy.push_back(temp);

                    flag_find = true;
                    break;
                }
            }

            // 如果有拓展块，再遍历拓展块
            if (!flag_find && cur_floder.block_num > static_cast<unsigned __int16>(1))
            {
                unsigned __int16 end = cur_floder.file_size;

                // 统计拓展块
                while (!flag_find && _header.FAT2[index_temp] != 0xffff)
                {
                    index_temp = _header.FAT2[index_temp];
                    end -= static_cast<unsigned __int16>(41);

                    if (end > static_cast<unsigned __int16>(41))
                        offset_end = static_cast<unsigned __int8>(41);
                    else
                        offset_end = static_cast<unsigned __int8>(end);

                    for (offset_p = static_cast<unsigned __int8>(1);
                        offset_p <= offset_end; offset_p++)
                    {
                        this->BlockToFile(&file_temp, index_temp, offset_p);

                        if (file_temp.last_index == 0xfffff || file_temp.type[0] != 0xf || file_temp.type[1] != 0xf
                            || file_temp.type[2] != 0xf || file_temp.type[3] != 0xf)
                            continue;

                        bool flag_name = true;

                        for (int i = 0; i < 8; i++)
                        {
                            if (name_c[i] != file_temp.name[i])
                            {
                                flag_name = false;
                                break;
                            }
                        }

                        if (flag_name)
                        {
                            CurFloder temp;
                            memcpy(temp.name, file_temp.name, sizeof(file_temp.name));
                            temp.index = file_temp.cur_index;
                            path_copy.push_back(temp);

                            flag_find = true;
                            break;
                        }
                    }
                }
            }

            if (!flag_find)
            {
                // TODO: 抛出异常
                std::cout << "Error: 不存在此路径！" << std::endl;
                return;
            }
        }
    }

    // 更新路径序列以及当前文件夹指针
    _path->clear();
    for (auto temp: path_copy)
        _path->push_back(temp);
    CurFloder back = _path->back();
    this->BlockToFile(_cur_floder, back.index, static_cast<unsigned __int8>(0));
}

void NMFSSpace::Close()
{

}

void NMFSSpace::Create()
{
    // TODO: 在当前位置要创建的文件的名称
    std::string file_name;

    if (file_name.length() > 8)
    {
        // TODO: 抛出异常
        std::cout << "Error: 文件名称不合法！" << std::endl;
        return;
    }

    char name_c[8] = { '\0' };
    for (int i = 0; i < file_name.length(); i++)
        name_c[i] = file_name[i];

    // 判断当前文件夹下是否有同名文件
    // 先遍历本块
    File file_temp;
    unsigned __int16 index_temp = _cur_floder->cur_index;
    unsigned __int8 offset_p;
    unsigned __int8 offset_end = static_cast<unsigned __int8>(1);
    bool flag_find = false;

    if (_cur_floder->file_size < static_cast<unsigned __int8>(42))
        offset_end = static_cast<unsigned __int8>(_cur_floder->file_size);
    else
        offset_end = static_cast<unsigned __int8>(41);

    for (offset_p = static_cast<unsigned __int16>(1);
        offset_p <= offset_end; offset_p++)
    {
        this->BlockToFile(&file_temp, index_temp, offset_p);

        if (file_temp.last_index == 0xfffff || (file_temp.type[0] == 0xf && file_temp.type[1] == 0xf
            && file_temp.type[2] == 0xf && file_temp.type[3] == 0xf))
            continue;

        bool flag_name = true;

        for (int i = 0; i < 8; i++)
        {
            if (name_c[i] != file_temp.name[i])
            {
                flag_name = false;
                break;
            }
        }

        if (flag_name)
        {
            flag_find = true;
            break;
        }
    }

    // 如果有拓展块，再遍历拓展块
    if (!flag_find && _cur_floder->block_num > static_cast<unsigned __int16>(1))
    {
        unsigned __int16 end = _cur_floder->file_size;

        // 统计拓展块
        while (!flag_find && _header.FAT2[index_temp] != 0xffff)
        {
            index_temp = _header.FAT2[index_temp];
            end -= static_cast<unsigned __int16>(41);

            if (end > static_cast<unsigned __int16>(41))
                offset_end = static_cast<unsigned __int8>(41);
            else
                offset_end = static_cast<unsigned __int8>(end);

            for (offset_p = static_cast<unsigned __int8>(1);
                offset_p <= offset_end; offset_p++)
            {
                this->BlockToFile(&file_temp, index_temp, offset_p);

                if (file_temp.last_index == 0xfffff || (file_temp.type[0] == 0xf && file_temp.type[1] == 0xf
                    && file_temp.type[2] == 0xf && file_temp.type[3] == 0xf))
                    continue;

                bool flag_name = true;

                for (int i = 0; i < 8; i++)
                {
                    if (name_c[i] != file_temp.name[i])
                    {
                        flag_name = false;
                        break;
                    }
                }

                if (flag_name)
                {
                    flag_find = true;
                    break;
                }
            }
        }
    }

    if (flag_find)
    {
        // TODO: 抛出异常
        std::cout << "Warnging: 已存在此文件夹！" << std::endl;
        return;
    }

    // 在空闲块上写录新文件信息
    File new_floder;
    memcpy(new_floder.name, name_c, sizeof(name_c));
    new_floder.last_index = _cur_floder->cur_index;
    new_floder.cur_index = 0xffff;  // 说明没有写入数据

    for (int i = 0; i < 32; i++)  // 全部置 0，说明不为文件夹类型
    {
        unsigned char bit = static_cast<unsigned char>(1 << (i % 8));
        new_floder.type[i / 8] &= ~bit;
    }

    new_floder.file_size = static_cast<unsigned __int32>(0);
    new_floder.block_num = static_cast<unsigned __int32>(1);

    // 更新当前的文件夹的记录信息，本级文件夹多了一个子文件
    _cur_floder->file_size++;

    unsigned __int16 cur_index = _cur_floder->cur_index;
    unsigned __int8 cur_offset = 0xff;
    this->GetFreeFile(cur_index, cur_offset);

    if (cur_index == 0xffff && cur_offset == 0xff)  // 如果没有空闲，则需要新开辟一块空间
    {
        cur_index = this->GetFreeBlock();

        if (cur_index == 0xffff)
        {
            // 抛出异常
            std::cout << "Error: 空间不足！" << std::endl;
            return;
        }

        unsigned __int8 cur_offset = 0x00;
        this->FileToBlock(&new_floder, cur_index, cur_offset);
        this->OffFreeBlock(cur_index);

        index_temp = _cur_floder->cur_index;
        while (_header.FAT2[index_temp] != 0xffff)
        {
            index_temp = _header.FAT2[index_temp];
        }
        _header.FAT2[index_temp] = cur_index;
        _header.FAT2[cur_index] = 0xffff;

        _cur_floder->block_num++;

    }
    else  // 有空闲，直接写到空闲处
    {
        this->FileToBlock(&new_floder, cur_index, cur_offset);
    }
}

void NMFSSpace::Delete()
{
    // 被删除的文件名称
    std::string del_file_name;

    if (del_file_name.length() > 8)
    {
        // TODO: 抛出异常
        std::cout << "Error: 文件名称不合法！" << std::endl;
        return;
    }

    char name_c[8] = { '\0' };
    for (int i = 0; i < del_file_name.length(); i++)
        name_c[i] = del_file_name[i];

    // 判断当前文件夹下是否有此文件
    // 先遍历本块
    File file_temp;
    unsigned __int16 index_temp = _cur_floder->cur_index;
    unsigned __int8 offset_p;
    unsigned __int8 offset_end = static_cast<unsigned __int8>(1);
    bool flag_find = false;

    if (_cur_floder->file_size < static_cast<unsigned __int8>(42))
        offset_end = static_cast<unsigned __int8>(_cur_floder->file_size);
    else
        offset_end = static_cast<unsigned __int8>(41);

    for (offset_p = static_cast<unsigned __int16>(1);
        offset_p <= offset_end; offset_p++)
    {
        this->BlockToFile(&file_temp, index_temp, offset_p);

        if (file_temp.last_index == 0xfffff || (file_temp.type[0] == 0xf && file_temp.type[1] == 0xf
            && file_temp.type[2] == 0xf && file_temp.type[3] == 0xf))
            continue;

        bool flag_name = true;

        for (int i = 0; i < 8; i++)
        {
            if (name_c[i] != file_temp.name[i])
            {
                flag_name = false;
                break;
            }
        }

        if (flag_name)
        {
            // TODO: 删除操作
            this->DeleteFile(file_temp.cur_index);

            file_temp.last_index = 0xffff;
            file_temp.cur_index = 0xffff;

            this->FileToBlock(&file_temp, index_temp, offset_p);

            flag_find = true;
            break;
        }
    }

    // 如果有拓展块，再遍历拓展块
    if (!flag_find && _cur_floder->block_num > static_cast<unsigned __int16>(1))
    {
        unsigned __int16 end = _cur_floder->file_size;

        // 统计拓展块
        while (!flag_find && _header.FAT2[index_temp] != 0xffff)
        {
            index_temp = _header.FAT2[index_temp];
            end -= static_cast<unsigned __int16>(41);

            if (end > static_cast<unsigned __int16>(41))
                offset_end = static_cast<unsigned __int8>(41);
            else
                offset_end = static_cast<unsigned __int8>(end);

            for (offset_p = static_cast<unsigned __int8>(1);
                offset_p <= offset_end; offset_p++)
            {
                this->BlockToFile(&file_temp, index_temp, offset_p);

                if (file_temp.last_index == 0xfffff || (file_temp.type[0] == 0xf && file_temp.type[1] == 0xf
                    && file_temp.type[2] == 0xf && file_temp.type[3] == 0xf))
                    continue;

                bool flag_name = true;

                for (int i = 0; i < 8; i++)
                {
                    if (name_c[i] != file_temp.name[i])
                    {
                        flag_name = false;
                        break;
                    }
                }

                if (flag_name)
                {
                    this->DeleteFile(file_temp.cur_index);

                    file_temp.last_index = 0xffff;
                    file_temp.cur_index = 0xffff;

                    this->FileToBlock(&file_temp, index_temp, offset_p);

                    flag_find = true;
                    break;
                }
            }
        }
    }

    if (!flag_find)
    {
        // TODO: 抛出异常
        std::cout << "Warnging: 不存在此文件！" << std::endl;
        return;
    }
}

void NMFSSpace::List(std::list<CurFloder> &tree, const bool &is_root)
{
    tree.clear();
    File extra_floder;

    if (is_root)
        this->BlockToFile(&extra_floder, static_cast<unsigned __int16>(2), static_cast<unsigned __int8>(0));
    else
        this->BlockToFile(&extra_floder, _cur_floder->cur_index, static_cast<unsigned __int8>(0));

    this->CreateList(tree, static_cast<unsigned __int16>(0), extra_floder);
}

void NMFSSpace::CreateList(std::list<CurFloder> &tree, unsigned __int16 depth, const File &extra_floder)
{
    // 这里使用了 CurFloder 结构体，其中 index 记录深度信息，name 记录名称

    CurFloder cur_floder_temp;
    memcpy(cur_floder_temp.name, extra_floder.name, size_t(8));
    cur_floder_temp.index = depth;
    tree.push_back(cur_floder_temp);

    if (extra_floder.file_size == static_cast<unsigned __int8>(0))
        return;

    depth += static_cast<unsigned __int16>(1);
    cur_floder_temp.index = depth;

    // 先遍历本块
    File file_temp;
    unsigned __int16 index_temp = extra_floder.cur_index;
    unsigned __int8 offset_p;
    unsigned __int8 offset_end = static_cast<unsigned __int8>(1);

    if (extra_floder.file_size < static_cast<unsigned __int8>(42))
        offset_end = static_cast<unsigned __int8>(extra_floder.file_size);
    else
        offset_end = static_cast<unsigned __int8>(41);

    for (offset_p = static_cast<unsigned __int16>(1);
        offset_p <= offset_end; offset_p++)
    {
        this->BlockToFile(&file_temp, index_temp, offset_p);

        if (file_temp.last_index == 0xfffff)
            continue;

        if (file_temp.type[0] == 0xf && file_temp.type[1] == 0xf
            && file_temp.type[2] == 0xf && file_temp.type[3] == 0xf)
        {
            this->CreateList(tree, depth, file_temp);
        }
        else
        {
            memcpy(cur_floder_temp.name, file_temp.name, size_t(8));
            tree.push_back(cur_floder_temp);
        }
    }

    // 如果有拓展块，再遍历拓展块
    if (extra_floder.block_num > static_cast<unsigned __int16>(1))
    {
        unsigned __int16 end = extra_floder.file_size;

        // 统计拓展块
        while (_header.FAT2[index_temp] != 0xffff)
        {
            index_temp = _header.FAT2[index_temp];
            end -= static_cast<unsigned __int16>(41);

            if (end > static_cast<unsigned __int16>(41))
                offset_end = static_cast<unsigned __int8>(41);
            else
                offset_end = static_cast<unsigned __int8>(end);

            for (offset_p = static_cast<unsigned __int8>(1);
                offset_p <= offset_end; offset_p++)
            {
                this->BlockToFile(&file_temp, index_temp, offset_p);

                if (file_temp.last_index == 0xfffff)
                    continue;

                if (file_temp.type[0] == 0xf && file_temp.type[1] == 0xf
                    && file_temp.type[2] == 0xf && file_temp.type[3] == 0xf)
                {
                    this->CreateList(tree, depth, file_temp);
                }
                else
                {
                    memcpy(cur_floder_temp.name, file_temp.name, size_t(8));
                    tree.push_back(cur_floder_temp);
                }
            }
        }
    }
}

void NMFSSpace::MakeDirectory()
{
    // TODO: 在当前位置要创建的文件夹的名称
    std::string dir_name;  

    if (dir_name.length() > 8)
    {
        // TODO: 抛出异常
        std::cout << "Error: 文件夹名称不合法！" << std::endl;
        return;
    }

    char name_c[8] = { '\0' };
    for (int i = 0; i < dir_name.length(); i++)
        name_c[i] = dir_name[i];

    // 判断当前文件夹下是否有同名文件夹
    // 先遍历本块
    File file_temp;
    unsigned __int16 index_temp = _cur_floder->cur_index;
    unsigned __int8 offset_p;
    unsigned __int8 offset_end = static_cast<unsigned __int8>(1);
    bool flag_find = false;

    if (_cur_floder->file_size < static_cast<unsigned __int8>(42))
        offset_end = static_cast<unsigned __int8>(_cur_floder->file_size);
    else
        offset_end = static_cast<unsigned __int8>(41);

    for (offset_p = static_cast<unsigned __int16>(1);
        offset_p <= offset_end; offset_p++)
    {
        this->BlockToFile(&file_temp, index_temp, offset_p);

        if (file_temp.last_index == 0xfffff || file_temp.type[0] != 0xf || file_temp.type[1] != 0xf
            || file_temp.type[2] != 0xf || file_temp.type[3] != 0xf)
            continue;

        bool flag_name = true;

        for (int i = 0; i < 8; i++)
        {
            if (name_c[i] != file_temp.name[i])
            {
                flag_name = false;
                break;
            }
        }

        if (flag_name)
        {
            flag_find = true;
            break;
        }
    }

    // 如果有拓展块，再遍历拓展块
    if (!flag_find && _cur_floder->block_num > static_cast<unsigned __int16>(1))
    {
        unsigned __int16 end = _cur_floder->file_size;

        // 统计拓展块
        while (!flag_find && _header.FAT2[index_temp] != 0xffff)
        {
            index_temp = _header.FAT2[index_temp];
            end -= static_cast<unsigned __int16>(41);

            if (end > static_cast<unsigned __int16>(41))
                offset_end = static_cast<unsigned __int8>(41);
            else
                offset_end = static_cast<unsigned __int8>(end);

            for (offset_p = static_cast<unsigned __int8>(1);
                offset_p <= offset_end; offset_p++)
            {
                this->BlockToFile(&file_temp, index_temp, offset_p);

                if (file_temp.last_index == 0xfffff || file_temp.type[0] != 0xf || file_temp.type[1] != 0xf
                    || file_temp.type[2] != 0xf || file_temp.type[3] != 0xf)
                    continue;

                bool flag_name = true;

                for (int i = 0; i < 8; i++)
                {
                    if (name_c[i] != file_temp.name[i])
                    {
                        flag_name = false;
                        break;
                    }
                }

                if (flag_name)
                {
                    flag_find = true;
                    break;
                }
            }
        }
    }

    if (flag_find)
    {
        // TODO: 抛出异常
        std::cout << "Warnging: 已存在此文件夹！" << std::endl;
        return;
    }

    // 找到一个新的空闲块
    unsigned __int16 new_index = this->GetFreeBlock();

    if (new_index == 0xffff)
    {
        // 抛出异常
        std::cout << "Error: 空间不足！" << std::endl;
        return;
    }

    // 在空闲块上写录新文件夹信息
    File new_floder;
    memcpy(new_floder.name, name_c, sizeof(name_c));
    new_floder.last_index = _cur_floder->cur_index;
    new_floder.cur_index = new_index;

    for (int i = 0; i < 32; i++)  // 全部置 1，说明为文件夹类型
    {
        unsigned char bit = static_cast<unsigned char>(1 << (i % 8));
        new_floder.type[i / 8] |= bit;
    }

    new_floder.file_size = static_cast<unsigned __int32>(0);
    new_floder.block_num = static_cast<unsigned __int32>(1);

    this->FileToBlock(&new_floder, new_index, static_cast<unsigned __int8>(0));
    this->InitFloderBlock(new_index);
    this->OffFreeBlock(new_index);

    // 更新当前的文件夹的记录信息，本级文件夹多了一个子文件夹
    _cur_floder->file_size++;

    unsigned __int16 cur_index = _cur_floder->cur_index;
    unsigned __int8 cur_offset = 0xff;
    this->GetFreeFile(cur_index, cur_offset);

    if (cur_index == 0xffff && cur_offset == 0xff)  // 如果没有空闲，则需要新开辟一块空间
    {
        cur_index = this->GetFreeBlock();
        if (cur_index == 0xffff)
        {
            // TODO: 抛出异常
            std::cout << "Error: 空间不足！" << std::endl;
            return;
        }
        unsigned __int8 cur_offset = 0x00;
        this->FileToBlock(&new_floder, cur_index, cur_offset);
        this->OffFreeBlock(cur_index);

        index_temp = _cur_floder->cur_index; 
        while (_header.FAT2[index_temp] != 0xffff)
        {
            index_temp = _header.FAT2[index_temp];
        }
        _header.FAT2[index_temp] = cur_index;
        _header.FAT2[cur_index] = 0xffff;

        _cur_floder->block_num++;

    }
    else  // 有空闲，直接写到空闲处
    {
        this->FileToBlock(&new_floder, cur_index, cur_offset);
    }
}

void NMFSSpace::Open()
{
    
}

void NMFSSpace::Read()
{

}

void NMFSSpace::RemoveDirectory()
{
    // 被删除的文件名称
    std::string del_floder_name;

    if (del_floder_name.length() > 8)
    {
        // TODO: 抛出异常
        std::cout << "Error: 文件夹名称不合法！" << std::endl;
        return;
    }

    char name_c[8] = { '\0' };
    for (int i = 0; i < del_floder_name.length(); i++)
        name_c[i] = del_floder_name[i];

    // 判断当前文件夹下是否有此文件
    // 先遍历本块
    File file_temp;
    unsigned __int16 index_temp = _cur_floder->cur_index;
    unsigned __int8 offset_p;
    unsigned __int8 offset_end = static_cast<unsigned __int8>(1);
    bool flag_find = false;

    if (_cur_floder->file_size < static_cast<unsigned __int8>(42))
        offset_end = static_cast<unsigned __int8>(_cur_floder->file_size);
    else
        offset_end = static_cast<unsigned __int8>(41);

    for (offset_p = static_cast<unsigned __int16>(1);
        offset_p <= offset_end; offset_p++)
    {
        this->BlockToFile(&file_temp, index_temp, offset_p);

        if (file_temp.last_index == 0xfffff || file_temp.type[0] != 0xf || file_temp.type[1] != 0xf
            || file_temp.type[2] != 0xf || file_temp.type[3] != 0xf)
            continue;

        bool flag_name = true;

        for (int i = 0; i < 8; i++)
        {
            if (name_c[i] != file_temp.name[i])
            {
                flag_name = false;
                break;
            }
        }

        if (flag_name)
        {
            this->DelDirectory(file_temp);

            file_temp.last_index = 0xffff;
            file_temp.cur_index = 0xffff;

            this->FileToBlock(&file_temp, index_temp, offset_p);

            flag_find = true;
            break;
        }
    }

    // 如果有拓展块，再遍历拓展块
    if (!flag_find && _cur_floder->block_num > static_cast<unsigned __int16>(1))
    {
        unsigned __int16 end = _cur_floder->file_size;

        // 统计拓展块
        while (!flag_find && _header.FAT2[index_temp] != 0xffff)
        {
            index_temp = _header.FAT2[index_temp];
            end -= static_cast<unsigned __int16>(41);

            if (end > static_cast<unsigned __int16>(41))
                offset_end = static_cast<unsigned __int8>(41);
            else
                offset_end = static_cast<unsigned __int8>(end);

            for (offset_p = static_cast<unsigned __int8>(1);
                offset_p <= offset_end; offset_p++)
            {
                this->BlockToFile(&file_temp, index_temp, offset_p);

                if (file_temp.last_index == 0xfffff || file_temp.type[0] != 0xf || file_temp.type[1] != 0xf
                    || file_temp.type[2] != 0xf || file_temp.type[3] != 0xf)
                    continue;

                bool flag_name = true;

                for (int i = 0; i < 8; i++)
                {
                    if (name_c[i] != file_temp.name[i])
                    {
                        flag_name = false;
                        break;
                    }
                }

                if (flag_name)
                {
                    this->DelDirectory(file_temp);

                    file_temp.last_index = 0xffff;
                    file_temp.cur_index = 0xffff;

                    this->FileToBlock(&file_temp, index_temp, offset_p);

                    flag_find = true;
                    break;
                }
            }
        }
    }

    if (!flag_find)
    {
        // TODO: 抛出异常
        std::cout << "Warnging: 不存在此文件！" << std::endl;
    }
    else
    {
        _cur_floder->file_size--;
        this->FileToBlock(_cur_floder, _cur_floder->cur_index, static_cast<unsigned __int8>(0));
    }
}

void NMFSSpace::DelDirectory(File &extra_floder)
{
    // 先删除子文件，再删除自己
    // 对于文件夹类型，需要再调用结束后注销掉自己在上一级的信息

    if (extra_floder.last_index == extra_floder.cur_index  // 根文件夹不能被删除，但是其中的子文件夹可以被删除
        && extra_floder.last_index != 0xffff && extra_floder.cur_index != 0xffff)
        return;

    if (extra_floder.file_size == static_cast<unsigned __int32>(0))
    {
        this->DeleteFile(extra_floder.cur_index);
        return;
    }

    // 先遍历本块
    File file_temp;
    unsigned __int16 index_temp = extra_floder.cur_index;
    unsigned __int8 offset_p;
    unsigned __int8 offset_end = static_cast<unsigned __int8>(1);

    if (extra_floder.file_size < static_cast<unsigned __int8>(42))
        offset_end = static_cast<unsigned __int8>(extra_floder.file_size);
    else
        offset_end = static_cast<unsigned __int8>(41);

    for (offset_p = static_cast<unsigned __int16>(1);
        offset_p <= offset_end; offset_p++)
    {
        this->BlockToFile(&file_temp, index_temp, offset_p);

        if (file_temp.last_index == 0xfffff)
            continue;

        if (file_temp.file_size != static_cast<unsigned __int32>(0)
            && file_temp.type[0] == 0xf && file_temp.type[1] == 0xf
            && file_temp.type[2] == 0xf && file_temp.type[3] == 0xf)
        {
            this->DelDirectory(file_temp);
        }

        this->DeleteFile(file_temp.cur_index);
    }

    // 如果有拓展块，再遍历拓展块
    if (extra_floder.block_num > static_cast<unsigned __int16>(1))
    {
        unsigned __int16 end = extra_floder.file_size;

        // 统计拓展块
        while (_header.FAT2[index_temp] != 0xffff)
        {
            index_temp = _header.FAT2[index_temp];
            end -= static_cast<unsigned __int16>(41);

            if (end > static_cast<unsigned __int16>(41))
                offset_end = static_cast<unsigned __int8>(41);
            else
                offset_end = static_cast<unsigned __int8>(end);

            for (offset_p = static_cast<unsigned __int8>(1);
                offset_p <= offset_end; offset_p++)
            {
                this->BlockToFile(&file_temp, index_temp, offset_p);

                if (file_temp.last_index == 0xfffff)
                    continue;

                if (file_temp.file_size != static_cast<unsigned __int32>(0) 
                    && file_temp.type[0] == 0xf && file_temp.type[1] == 0xf
                    && file_temp.type[2] == 0xf && file_temp.type[3] == 0xf)
                {
                    this->DelDirectory(file_temp);
                }

                this->DeleteFile(file_temp.cur_index);
            }
        }
    }

    this->DeleteFile(extra_floder.cur_index);
}

void NMFSSpace::Write()
{

}