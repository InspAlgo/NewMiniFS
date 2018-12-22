#include "../include/nmfs_space.h"
#include "../include/exceptions/nmfs_exception.h"
#include "../include/exceptions/warning_exception.h"
#include "../include/exceptions/error_exception.h"

#pragma warning(disable:4996)

NMFSSpace* volatile NMFSSpace::_global_space_pointer = nullptr;

void  NMFSSpace::NewNMFSSpace(const std::string &space_name)
{
    if (_global_space_pointer != nullptr)
        throw NMFSWarningException("当前已打开一个简单文件系统！");

    /* 检查当前位置下是否有同名文件 */
    FILE *space_file = nullptr;
    space_file = fopen((char*)space_name.data(), "r");
    if (space_file != nullptr)
    {
        fclose(space_file);
        throw NMFSWarningException("当前有同名文件！");
    }

    /* 在当前位置创建一个空间文件，以后续写回磁盘 */
    space_file = fopen((char*)space_name.data(), "wb+");
    _fseeki64(space_file, 512 * 1024 - 1, SEEK_SET);
    fwrite("\0", 1, 1, space_file);
    fclose(space_file);

    /* 空间单例实例化 */
    _global_space_pointer = new NMFSSpace();
    _global_space_pointer->_space_name = space_name;
}

NMFSSpace* NMFSSpace::GetActiveNMFSSpace()
{
    if (_global_space_pointer == nullptr)
        throw NMFSWarningException("当前并未打开一个简单文件系统！");

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

    _cur_file_block_index = 0xffff;
    _cur_file_offset = 0xff;

    _cur_file = new File();  // 实例化，但不做赋值处理

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
    memcpy(temp.name, _cur_floder->name, 8);
    temp.index = _cur_floder->cur_index;
    temp.last_index = 0xffff;
    temp.last_offset = 0xff;
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

    _cur_file_block_index = 0xffff;
    _cur_file_offset = 0xff;

    /* 将磁盘中的数据写到内存中 */
    FILE *space_file = fopen((char*)space_name.data(), "rb+");
    _fseeki64(space_file, 0LL, SEEK_SET);
    for (int i = 0; i < 512; i++)
        fread(_space_block[i], 1024, 1, space_file);
    fclose(space_file);

    for (int i = 0; i < 64; i++)
        _header.FAT1[i] = _space_block[0][8 + i];

    for (int i = 0, j = 0; i < 512; i++, j += 2)
        _header.FAT2[i] = static_cast<unsigned __int16>((_space_block[1][j] << 8) | _space_block[1][j + 1]);

    _cur_file = new File();

    _cur_floder = new File();
    this->BlockToFile(_cur_floder, static_cast<unsigned __int16>(2), static_cast<unsigned __int8>(0));

    _path = new std::list<CurFloder>();
    CurFloder temp;
    memcpy(temp.name, _cur_floder->name, 8);
    temp.index = _cur_floder->cur_index;
    temp.last_index = 0xffff;
    temp.last_offset = 0xff;
    _path->push_back(temp);
}

NMFSSpace::~NMFSSpace()
{
    delete[] _header.FAT1;
    _header.FAT1 = nullptr;

    delete[] _header.FAT2;
    _header.FAT2 = nullptr;

    for (int i = 0; i < 512; i++)
    {
        delete[] _space_block[i];
        _space_block[i] = nullptr;
    }
    delete[] _space_block;
    _space_block = nullptr;

    delete _cur_file;
    delete _cur_floder;
    delete _path;
}

void NMFSSpace::System(const std::string &space_name)
{
    if (_global_space_pointer != nullptr)
        throw NMFSWarningException("当前已打开一个简单文件系统！");

    char magic_num[4] = { '\0' };

    /* 判断当前位置是否有此文件，进行文件头魔数校验 */
    FILE *space_file = nullptr;
    space_file = fopen((char*)space_name.data(), "rb+");

    if (space_file == nullptr)
        throw NMFSWarningException("当前目录下无此文件！");

    _fseeki64(space_file, 0LL, SEEK_SET);
    fread(&magic_num, 4, 1, space_file);
    fclose(space_file);

    if (magic_num[0] != 'N' || magic_num[1] != 'M' 
        || magic_num[2] != 'F' || magic_num[3] != 'S')
    {
        throw NMFSWarningException("非NMFS空间文件类型！");
    }

    _global_space_pointer = new NMFSSpace(space_name);
    _global_space_pointer->_space_name = space_name;
}

void NMFSSpace::Exit()
{
    if (_global_space_pointer == nullptr)
        throw NMFSWarningException("当前并未打开一个简单文件系统！");

    _global_space_pointer->HeaderToBlock();

    /* 将内存空间的数据写回磁盘 */
    FILE *space_file = fopen((char*)_global_space_pointer->_space_name.data(), "rb+");
    _fseeki64(space_file, 0LL, SEEK_SET);
    for (int i = 0; i < 512; i++)
        fwrite(_global_space_pointer->_space_block[i], 1024, 1, space_file);
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

void NMFSSpace::FileToBlock(const File *file, const unsigned __int16 &block_index, const unsigned __int8 &offset) noexcept
{
    if (offset > static_cast<unsigned __int8>(41))
        return;  // throw NMFSErrorException("块内偏移量越界！");

    if (file == nullptr)
        return;  // throw NMFSErrorException("空指针！");

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

void NMFSSpace::BlockToFile(File *file, const unsigned __int16 &block_index, const unsigned __int8 &offset) noexcept
{
    if (offset > static_cast<unsigned __int8>(41))
        return;  // throw NMFSErrorException("块内偏移量越界！");

    if (file == nullptr)
        return;  // throw NMFSErrorException("空指针！");

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

unsigned __int16 NMFSSpace::GetFreeBlock() const noexcept
{
    /* FAT1 遍历查询 */
    for (int i = 0; i < 512; i++)
    {
        unsigned char bit = static_cast<unsigned char>(1 << (i % 8));

        if (_header.FAT1[i / 8] & bit)  // _header.FAT1[i / 8] & bit <= _header.FAT1[i / 8] & bit  == bit
            continue;

        _header.FAT1[i / 8] |= bit;
        _header.FAT2[i] = 0xffff;
        return static_cast<unsigned __int16>(i);
    }

    return 0xffff;  // 未找到空闲块
}

void NMFSSpace::OffFreeBlock(const unsigned __int16 &block_index) noexcept
{
    int i = static_cast<int>(block_index);
    unsigned char bit = static_cast<unsigned char>(1 << (i % 8));
    _header.FAT1[i / 8] |= bit;
}

void NMFSSpace::GetFreeFile(unsigned __int16 &block_index, unsigned __int8 &offset) noexcept
{
    File cur_floder;
    this->BlockToFile(&cur_floder, block_index, static_cast<unsigned __int8>(0));

    // 先遍历本块
    File file_temp;
    unsigned __int8 offset_p;
    unsigned __int8 offset_end = static_cast<unsigned __int8>(41);
    unsigned __int16 index_temp = block_index;

    for (offset_p = static_cast<unsigned __int8>(1);
        offset_p <= offset_end; offset_p++)
    {
        this->BlockToFile(&file_temp, index_temp, offset_p);

        if (file_temp.last_index == 0xffff)
        {
            block_index = index_temp;
            offset = offset_p;
            return;
        }
    }

    // 如果有拓展块，再遍历拓展块
    if (cur_floder.block_num > static_cast<unsigned __int16>(1))
    {
        // 统计拓展块
        while ( _header.FAT2[index_temp] != 0xffff)
        {
            index_temp = _header.FAT2[index_temp];

            for (offset_p = static_cast<unsigned __int8>(1);
                offset_p <= offset_end; offset_p++)
            {
                this->BlockToFile(&file_temp, index_temp, offset_p);

                if (file_temp.last_index == 0xffff)
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

void NMFSSpace::InitFloderBlock(const unsigned __int16 &block_index) noexcept
{
    for (unsigned __int16 offset_p = static_cast<unsigned __int16>(24); 
        offset_p < static_cast<unsigned __int16>(1024); offset_p++)
    {
        _space_block[block_index][offset_p] = 0xff;
    }
}

void NMFSSpace::DeleteFile(const unsigned __int16 &index) noexcept
{
    unsigned __int16 index_temp = index;
    unsigned __int16 temp;

    while (index_temp != 0xffff)
    {
        for (int i = 0; i < 1024; i++)
            _space_block[index_temp][i] = 0xff;

        int ii = static_cast<int>(index_temp);
        unsigned char bit = static_cast<unsigned char>(1 << (ii % 8));
        _header.FAT1[ii / 8] &= ~bit;

        temp = index_temp;
        index_temp = _header.FAT2[index_temp];
        _header.FAT2[temp] = 0xffff;
    }
}

void NMFSSpace::ChangeDirectory(std::queue<std::string> &cd_path)
{
    // 准备拷贝路径容器，后续转移使用拷贝路径容器，防止抛出异常时不会改变原始路径信息
    std::list<CurFloder>path_copy;
    for (auto temp : *_path)
        path_copy.push_back(temp);

    File cur_floder;
    memcpy(&cur_floder, _cur_floder, sizeof(File));

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
            if (path_copy.size() > 1)
            {
                path_copy.pop_back();

                unsigned __int16 to_index = cur_floder.last_index;
                this->BlockToFile(&cur_floder, to_index, static_cast<unsigned __int8>(0));
            }
            else if (path_copy.size() == 1)  // 已经到达根目录，不能再向上一级文件夹转移
                continue;
            else
                throw NMFSWarningException("不存在此路径！");
        }
        // 向下一级文件夹转移
        else  
        {
            // a. 文件夹名称合法性判断
            if (str_floder.length() > 8)
                throw NMFSWarningException("不存在此路径！");

            char name_c[8] = { '\0' };
            for (int i = 0; i < str_floder.length(); i++)
                name_c[i] = str_floder[i];

            unsigned __int16 index_temp = cur_floder.cur_index;  // 获取当前文件夹入口
            
            bool flag_find = false;  // 标记是否查询到

            // 先遍历本块
            File file_temp;
            unsigned __int8 offset_p;
            unsigned __int8 offset_end = static_cast<unsigned __int8>(41);

            for (offset_p = static_cast<unsigned __int8>(1);
                offset_p <= offset_end; offset_p++)
            {
                this->BlockToFile(&file_temp, index_temp, offset_p);

                if (file_temp.last_index == 0xffff || file_temp.type[0] != 0xff || file_temp.type[1] != 0xff || file_temp.type[2] != 0xff || file_temp.type[3] != 0xff)
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
                    memcpy(temp.name, file_temp.name, 8);
                    temp.index = file_temp.cur_index;
                    temp.last_index = index_temp;
                    temp.last_offset = offset_p;
                    path_copy.push_back(temp);

                    this->BlockToFile(&cur_floder, temp.index, static_cast<unsigned __int8>(0));

                    flag_find = true;
                    break;
                }
            }

            // 如果有拓展块，再遍历拓展块
            if (!flag_find && cur_floder.block_num > static_cast<unsigned __int16>(1))
            {
                // 统计拓展块
                while (!flag_find && _header.FAT2[index_temp] != 0xffff)
                {
                    index_temp = _header.FAT2[index_temp];

                    for (offset_p = static_cast<unsigned __int8>(1);
                        offset_p <= offset_end; offset_p++)
                    {
                        this->BlockToFile(&file_temp, index_temp, offset_p);

                        if (file_temp.last_index == 0xffff || file_temp.type[0] != 0xff || file_temp.type[1] != 0xff || file_temp.type[2] != 0xff || file_temp.type[3] != 0xff)
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
                            memcpy(temp.name, file_temp.name, 8);
                            temp.index = file_temp.cur_index;
                            temp.last_index = index_temp;
                            temp.last_offset = offset_p;
                            path_copy.push_back(temp);

                            this->BlockToFile(&cur_floder, temp.index, static_cast<unsigned __int8>(0));

                            flag_find = true;
                            break;
                        }
                    }
                }
            }

            if (!flag_find)
                throw NMFSWarningException("不存在此路径！");
        }
    }

    // 更新路径序列以及当前文件夹指针
    _path->clear();
    for (auto temp: path_copy)
        _path->push_back(temp);
    CurFloder back = _path->back();
    this->BlockToFile(_cur_floder, back.index, static_cast<unsigned __int8>(0));
    _cur_floder_block_index = back.last_index;
    _cur_floder_offset = back.last_offset;
}

void NMFSSpace::Close()
{
    if (_cur_file_block_index == 0xffff && _cur_file_offset == 0xff)
        throw NMFSWarningException("当前并未打开文件！");

    _cur_file_block_index = 0xffff;
    _cur_file_offset = 0xff;
}

void NMFSSpace::Create(const std::string &file_name, const std::string &file_type)
{
    if (file_name.length() > 8)
        throw NMFSWarningException("文件名称不合法！");
    if (file_type.length() > 4)
        throw NMFSWarningException("文件类型不合法！");

    char name_c[8] = { '\0' };
    for (int i = 0; i < file_name.length(); i++)
        name_c[i] = file_name[i];
    char type_c[4] = { '\0' };
    for (int i = 0; i < 4; i++)
        type_c[i] = file_type[i];

    // 判断当前文件夹下是否有同名文件
    // 先遍历本块
    File file_temp;
    unsigned __int16 index_temp = _cur_floder->cur_index;
    unsigned __int8 offset_p;
    unsigned __int8 offset_end = static_cast<unsigned __int8>(41);
    bool flag_find = false;

    for (offset_p = static_cast<unsigned __int16>(1);
        offset_p <= offset_end; offset_p++)
    {
        this->BlockToFile(&file_temp, index_temp, offset_p);

        if (file_temp.last_index == 0xffff || (file_temp.type[0] == 0xff && file_temp.type[1] == 0xff
            && file_temp.type[2] == 0xff && file_temp.type[3] == 0xff))
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
        // 统计拓展块
        while (!flag_find && _header.FAT2[index_temp] != 0xffff)
        {
            index_temp = _header.FAT2[index_temp];

            for (offset_p = static_cast<unsigned __int8>(1);
                offset_p <= offset_end; offset_p++)
            {
                this->BlockToFile(&file_temp, index_temp, offset_p);

                if (file_temp.last_index == 0xffff || (file_temp.type[0] == 0xff && file_temp.type[1] == 0xff
                    && file_temp.type[2] == 0xff && file_temp.type[3] == 0xff))
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
        throw NMFSWarningException("已存在此文件！");

    // 在空闲块上写录新文件信息
    File new_floder;
    memcpy(new_floder.name, name_c, 8);

    new_floder.last_index = _cur_floder->cur_index;
    new_floder.cur_index = 0xffff;  // 说明没有写入数据

    memcpy(new_floder.type, type_c, 4);

    new_floder.file_size = static_cast<unsigned __int32>(0);
    new_floder.block_num = static_cast<unsigned __int32>(0);

    // 更新当前的文件夹的记录信息，本级文件夹多了一个子文件
    _cur_floder->file_size++;

    unsigned __int16 cur_index = _cur_floder->cur_index;
    unsigned __int8 cur_offset = 0xff;
    this->GetFreeFile(cur_index, cur_offset);

    if (cur_index == 0xffff && cur_offset == 0xff)  // 如果没有空闲，则需要新开辟一块空间
    {
        cur_index = this->GetFreeBlock();

        if (cur_index == 0xffff)
            throw NMFSErrorException("空间不足！");

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

        index_temp = _cur_floder->cur_index;
        this->FileToBlock(_cur_floder, index_temp, static_cast<unsigned __int8>(0));
        this->InitFloderBlock(cur_index);
    }
    else  // 有空闲，直接写到空闲处
    {
        this->FileToBlock(&new_floder, cur_index, cur_offset);
        index_temp = _cur_floder->cur_index;
        this->FileToBlock(_cur_floder, index_temp, static_cast<unsigned __int8>(0));
    }
}

void NMFSSpace::Delete(const std::string &del_file_name, const std::string &del_file_type)
{
    if (del_file_name.length() > 8)
        throw NMFSWarningException("文件名称不合法！");
    if (del_file_type.length() > 4)
        throw NMFSWarningException("文件类型不合法！");

    char name_c[8] = { '\0' };
    for (int i = 0; i < del_file_name.length(); i++)
        name_c[i] = del_file_name[i];
    char type_c[4] = { '\0' };
    for (int i = 0; i < 4; i++)
        type_c[i] = del_file_type[i];

    // 判断当前文件夹下是否有此文件
    // 先遍历本块
    File file_temp;
    unsigned __int16 index_temp = _cur_floder->cur_index;
    unsigned __int8 offset_p;
    unsigned __int8 offset_end = static_cast<unsigned __int8>(41);

    for (offset_p = static_cast<unsigned __int8>(1);
        offset_p <= offset_end; offset_p++)
    {
        this->BlockToFile(&file_temp, index_temp, offset_p);

        if (file_temp.last_index == 0xffff || (file_temp.type[0] == 0xff && file_temp.type[1] == 0xff
            && file_temp.type[2] == 0xff && file_temp.type[3] == 0xff))
            continue;

        if (type_c[0] != file_temp.type[0] || type_c[1] != file_temp.type[1] ||
            type_c[2] != file_temp.type[2] || type_c[3] != file_temp.type[3])
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

            unsigned __int16 last_index = file_temp.last_index;
            file_temp.last_index = 0xffff;
            file_temp.cur_index = 0xffff;

            this->FileToBlock(&file_temp, index_temp, offset_p);

            this->BlockToFile(&file_temp, last_index, static_cast<unsigned __int8>(0));
            file_temp.file_size--;
            this->FileToBlock(&file_temp, last_index, static_cast<unsigned __int8>(0));

            return;
        }
    }

    // 如果有拓展块，再遍历拓展块
    if (_cur_floder->block_num > static_cast<unsigned __int16>(1))
    {
        // 统计拓展块
        while (_header.FAT2[index_temp] != 0xffff)
        {
            index_temp = _header.FAT2[index_temp];

            for (offset_p = static_cast<unsigned __int8>(1);
                offset_p <= offset_end; offset_p++)
            {
                this->BlockToFile(&file_temp, index_temp, offset_p);

                if (file_temp.last_index == 0xffff || (file_temp.type[0] == 0xff && file_temp.type[1] == 0xff
                    && file_temp.type[2] == 0xff && file_temp.type[3] == 0xff))
                    continue;

                if (type_c[0] != file_temp.type[0] || type_c[1] != file_temp.type[1] ||
                    type_c[2] != file_temp.type[2] || type_c[3] != file_temp.type[3])
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

                    unsigned __int16 last_index = file_temp.last_index;
                    file_temp.last_index = 0xffff;
                    file_temp.cur_index = 0xffff;

                    this->FileToBlock(&file_temp, index_temp, offset_p);

                    this->BlockToFile(&file_temp, last_index, static_cast<unsigned __int8>(0));
                    file_temp.file_size--;
                    this->FileToBlock(&file_temp, last_index, static_cast<unsigned __int8>(0));

                    return;
                }
            }
        }
    }

    throw NMFSWarningException("不存在此文件！");
}

void NMFSSpace::List(std::list<TreeNode> &tree, const bool &is_root)
{
    tree.clear();
    File extra_floder;
    
    if (is_root)
        this->BlockToFile(&extra_floder, static_cast<unsigned __int16>(2), static_cast<unsigned __int8>(0));
    else
        this->BlockToFile(&extra_floder, _cur_floder->cur_index, static_cast<unsigned __int8>(0));

    this->CreateList(tree, static_cast<unsigned __int16>(0), extra_floder);
}

void NMFSSpace::CreateList(std::list<TreeNode> &tree, unsigned __int16 depth, const File &extra_floder)
{
    // 这里使用了 CurFloder 结构体，其中 index 记录深度信息，name 记录名称

    TreeNode cur_floder_temp;
    for (int i = 0; i < 4; i++)
        cur_floder_temp.type[i] = 0xff;
    memcpy(cur_floder_temp.name, extra_floder.name, 8);
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
    unsigned __int8 offset_end = static_cast<unsigned __int8>(41);

    for (offset_p = static_cast<unsigned __int16>(1);
        offset_p <= offset_end; offset_p++)
    {
        this->BlockToFile(&file_temp, index_temp, offset_p);

        if (file_temp.last_index == 0xffff)
            continue;

        if (file_temp.type[0] == 0xff && file_temp.type[1] == 0xff
            && file_temp.type[2] == 0xff && file_temp.type[3] == 0xff)
        {
            this->CreateList(tree, depth, file_temp);
        }
        else
        {
            for (int i = 0; i < 4; i++)
                cur_floder_temp.type[i] = file_temp.type[i];
            memcpy(cur_floder_temp.name, file_temp.name, 8);
            tree.push_back(cur_floder_temp);
        }
    }

    // 如果有拓展块，再遍历拓展块
    if (extra_floder.block_num > static_cast<unsigned __int16>(1))
    {
        // 统计拓展块
        while (_header.FAT2[index_temp] != 0xffff)
        {
            index_temp = _header.FAT2[index_temp];

            for (offset_p = static_cast<unsigned __int8>(1);
                offset_p <= offset_end; offset_p++)
            {
                this->BlockToFile(&file_temp, index_temp, offset_p);

                if (file_temp.last_index == 0xffff)
                    continue;

                if (file_temp.type[0] == 0xff && file_temp.type[1] == 0xff
                    && file_temp.type[2] == 0xff && file_temp.type[3] == 0xff)
                {
                    this->CreateList(tree, depth, file_temp);
                }
                else
                {
                    for (int i = 0; i < 4; i++)
                        cur_floder_temp.type[i] = file_temp.type[i];
                    memcpy(cur_floder_temp.name, file_temp.name, 8);
                    tree.push_back(cur_floder_temp);
                }
            }
        }
    }
}

void NMFSSpace::MakeDirectory(const std::string &dir_name)
{
    if (dir_name.length() > 8)
        throw NMFSWarningException("文件夹名称不合法！");

    char name_c[8] = { '\0' };
    for (int i = 0; i < dir_name.length(); i++)
        name_c[i] = dir_name[i];

    // 判断当前文件夹下是否有同名文件夹
    // 先遍历本块
    File file_temp;
    unsigned __int16 index_temp = _cur_floder->cur_index;
    unsigned __int8 offset_p;
    unsigned __int8 offset_end = static_cast<unsigned __int8>(41);
    bool flag_find = false;

    for (offset_p = static_cast<unsigned __int16>(1);
        offset_p <= offset_end; offset_p++)
    {
        this->BlockToFile(&file_temp, index_temp, offset_p);

        if (file_temp.last_index == 0xffff || file_temp.type[0] != 0xff || file_temp.type[1] != 0xff
            || file_temp.type[2] != 0xff || file_temp.type[3] != 0xff)
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
        // 统计拓展块
        while (!flag_find && _header.FAT2[index_temp] != 0xffff)
        {
            index_temp = _header.FAT2[index_temp];

            for (offset_p = static_cast<unsigned __int8>(1);
                offset_p <= offset_end; offset_p++)
            {
                this->BlockToFile(&file_temp, index_temp, offset_p);

                if (file_temp.last_index == 0xffff || file_temp.type[0] != 0xff || file_temp.type[1] != 0xff
                    || file_temp.type[2] != 0xff || file_temp.type[3] != 0xff)
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
        throw NMFSWarningException("已存在此文件夹！");

    // 找到一个新的空闲块
    unsigned __int16 new_index = this->GetFreeBlock();

    if (new_index == 0xffff)
        throw NMFSErrorException("空间不足！");

    // 在空闲块上写录新文件夹信息
    File new_floder;
    memcpy(new_floder.name, name_c, 8);
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

    File floder_temp;

    if (cur_index == 0xffff && cur_offset == 0xff)  // 如果没有空闲，则需要新开辟一块空间
    {
        cur_index = this->GetFreeBlock();
        if (cur_index == 0xffff)
            throw NMFSErrorException("空间不足！");

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
        this->InitFloderBlock(cur_index);

        index_temp = _cur_floder->cur_index;
        this->FileToBlock(_cur_floder, index_temp, static_cast<unsigned __int8>(0));
        if (_cur_floder->last_index != _cur_floder->cur_index)  // 如果当前文件夹不是根文件夹
            this->FileToBlock(_cur_floder, _cur_floder_block_index, _cur_floder_offset);
    }
    else  // 有空闲，直接写到空闲处
    {
        this->FileToBlock(&new_floder, cur_index, cur_offset);

        index_temp = _cur_floder->cur_index;
        this->FileToBlock(_cur_floder, index_temp, static_cast<unsigned __int8>(0));
        if (_cur_floder->last_index != _cur_floder->cur_index)  // 如果当前文件夹不是根文件夹
            this->FileToBlock(_cur_floder, _cur_floder_block_index, _cur_floder_offset);
    }
}

void NMFSSpace::Open(const std::string &file_name, const std::string &file_type)
{
    if (_cur_file_block_index != 0xffff && _cur_file_offset != 0xff)
        throw NMFSWarningException("不能重复打开一个文件！");

    if (file_name.length() > 8)
        throw NMFSWarningException("文件名称不合法！");
    if (file_type.length() > 4)
        throw NMFSWarningException("文件类型不合法！");

    char name_c[8] = { '\0' };
    for (int i = 0; i < file_name.length(); i++)
        name_c[i] = file_name[i];
    char type_c[4] = { '\0' };
    for (int i = 0; i < 4; i++)
        type_c[i] = file_type[i];

    // 判断当前文件夹下是否有同名文件
    // 先遍历本块
    File file_temp;
    unsigned __int16 index_temp = _cur_floder->cur_index;
    unsigned __int8 offset_p;
    unsigned __int8 offset_end = static_cast<unsigned __int8>(41);
    bool flag_find = false;

    for (offset_p = static_cast<unsigned __int8>(1);
        offset_p <= offset_end; offset_p++)
    {
        this->BlockToFile(&file_temp, index_temp, offset_p);

        if (file_temp.last_index == 0xffff || (file_temp.type[0] == 0xff && file_temp.type[1] == 0xff
            && file_temp.type[2] == 0xff && file_temp.type[3] == 0xff))
            continue;

        if (type_c[0] != file_temp.type[0] || type_c[1] != file_temp.type[1] ||
            type_c[2] != file_temp.type[2] || type_c[3] != file_temp.type[3])
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
            memcpy(_cur_file, &file_temp, sizeof(File));
            _cur_file_block_index = index_temp;
            _cur_file_offset = offset_p;

            flag_find = true;
            break;
        }
    }

    // 如果有拓展块，再遍历拓展块
    if (!flag_find && _cur_floder->block_num > static_cast<unsigned __int16>(1))
    {
        // 统计拓展块
        while (!flag_find && _header.FAT2[index_temp] != 0xffff)
        {
            index_temp = _header.FAT2[index_temp];

            for (offset_p = static_cast<unsigned __int8>(1);
                offset_p <= offset_end; offset_p++)
            {
                this->BlockToFile(&file_temp, index_temp, offset_p);

                if (file_temp.last_index == 0xffff || (file_temp.type[0] == 0xff && file_temp.type[1] == 0xff
                    && file_temp.type[2] == 0xff && file_temp.type[3] == 0xff))
                    continue;

                if (type_c[0] != file_temp.type[0] || type_c[1] != file_temp.type[1] ||
                    type_c[2] != file_temp.type[2] || type_c[3] != file_temp.type[3])
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
                    memcpy(_cur_file, &file_temp, sizeof(File));
                    _cur_file_block_index = index_temp;
                    _cur_file_offset = offset_p;

                    flag_find = true;
                    break;
                }
            }
        }
    }

    if (!flag_find)
        throw NMFSWarningException("未找到此文件！");
}

void NMFSSpace::Read(unsigned char *&output)
{
    if (_cur_file_block_index == 0xffff && _cur_file_offset == 0xff)
        throw NMFSWarningException("当前并未打开文件！");

    output = nullptr;

    if (_cur_file->file_size > static_cast<unsigned __int32>(0))
        output = new unsigned char[_cur_file->file_size + 1]{ '\0' };
    else
        return;

    unsigned __int16 index_temp = _cur_file->cur_index;
    unsigned __int32 remaining_read_size = _cur_file->file_size;
    unsigned __int32 r = static_cast<unsigned __int32>(0);

    while (index_temp != 0xffff)
    {
        unsigned __int32 end = static_cast<unsigned __int32>(0);

        if (remaining_read_size > static_cast<unsigned __int32>(1024))
            end = static_cast<unsigned __int32>(1024);
        else
            end = remaining_read_size;

        for (unsigned __int32 p = static_cast<unsigned __int32>(0);
            p < end; p++)
            output[r++] = _space_block[index_temp][p];

        index_temp = _header.FAT2[index_temp];
        remaining_read_size -= static_cast<unsigned __int32>(1024);
    }
}

void NMFSSpace::RemoveDirectory(const std::string &del_floder_name)
{
    if (del_floder_name.length() > 8)
        throw NMFSWarningException("文件夹名称不合法！");

    char name_c[8] = { '\0' };
    for (int i = 0; i < del_floder_name.length(); i++)
        name_c[i] = del_floder_name[i];

    // 判断当前文件夹下是否有此文件
    // 先遍历本块
    File file_temp;
    unsigned __int16 index_temp = _cur_floder->cur_index;
    unsigned __int8 offset_p;
    unsigned __int8 offset_end = static_cast<unsigned __int8>(41);
    bool flag_find = false;

    for (offset_p = static_cast<unsigned __int8>(1);
        offset_p <= offset_end; offset_p++)
    {
        this->BlockToFile(&file_temp, index_temp, offset_p);

        if (file_temp.last_index == 0xffff || file_temp.type[0] != 0xff || file_temp.type[1] != 0xff
            || file_temp.type[2] != 0xff || file_temp.type[3] != 0xff)
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
        // 统计拓展块
        while (!flag_find && _header.FAT2[index_temp] != 0xffff)
        {
            index_temp = _header.FAT2[index_temp];

            for (offset_p = static_cast<unsigned __int8>(1);
                offset_p <= offset_end; offset_p++)
            {
                this->BlockToFile(&file_temp, index_temp, offset_p);

                if (file_temp.last_index == 0xffff || file_temp.type[0] != 0xff || file_temp.type[1] != 0xff || file_temp.type[2] != 0xff || file_temp.type[3] != 0xff)
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
        throw NMFSWarningException("不存在此文件夹！");
    else
    {
        _cur_floder->file_size--;
        index_temp = _cur_floder->cur_index;
        this->FileToBlock(_cur_floder, index_temp, static_cast<unsigned __int8>(0));
    }
}

void NMFSSpace::DelDirectory(File &extra_floder)  // 入口为信息表，非处于下一级文件夹块
{
    if (extra_floder.last_index == extra_floder.cur_index)  // 根文件夹不能被删除，但是其中的子文件夹可以被删除
        return;

    if (extra_floder.file_size == static_cast<unsigned __int32>(0))  // 注销文件夹块
    {
        this->DeleteDirectory(extra_floder);
        return;
    }

    // 先遍历本块
    File file_temp;
    unsigned __int16 index_temp = extra_floder.cur_index;
    unsigned __int8 offset_p;
    unsigned __int8 offset_end = static_cast<unsigned __int8>(41);

    for (offset_p = static_cast<unsigned __int8>(1);
        offset_p <= offset_end; offset_p++)
    {
        this->BlockToFile(&file_temp, index_temp, offset_p);

        if (file_temp.last_index == 0xffff)
            continue;

        if (file_temp.type[0] == 0xff && file_temp.type[1] == 0xff
            && file_temp.type[2] == 0xff && file_temp.type[3] == 0xff)
        {
            this->DelDirectory(file_temp);
        }
        else
            this->DeleteDirectory(file_temp);
    }

    // 如果有拓展块，再遍历拓展块
    if (extra_floder.block_num > static_cast<unsigned __int16>(1))
    {
        // 统计拓展块
        while (_header.FAT2[index_temp] != 0xffff)
        {
            index_temp = _header.FAT2[index_temp];

            for (offset_p = static_cast<unsigned __int8>(1);
                offset_p <= offset_end; offset_p++)
            {
                this->BlockToFile(&file_temp, index_temp, offset_p);

                if (file_temp.last_index == 0xffff)
                    continue;

                if (file_temp.type[0] == 0xff && file_temp.type[1] == 0xff
                    && file_temp.type[2] == 0xff && file_temp.type[3] == 0xff)
                {
                    this->DelDirectory(file_temp);
                }
                else
                    this->DeleteDirectory(file_temp);
            }
        }
    }

    this->DeleteDirectory(extra_floder);
}

void NMFSSpace::DeleteDirectory(const File &extra_floder)
{
    if (extra_floder.cur_index == 0xffff || extra_floder.last_index == 0xffff)
    {
        return;
    }
    else
    {
        unsigned __int16 index_temp = extra_floder.cur_index;
        unsigned __int16 temp = 0xffff;

        while (index_temp != 0xffff)
        {
            for (int i = 0; i < 1024; i++)
                _space_block[index_temp][i] = 0xff;

            int ii = static_cast<int>(index_temp);
            unsigned char bit = static_cast<unsigned char>(1 << (ii % 8));
            _header.FAT1[ii / 8] &= ~bit;

            temp = index_temp;
            index_temp = _header.FAT2[index_temp];
            _header.FAT2[temp] = 0xffff;
        }
    }
}

void NMFSSpace::Write(const unsigned char *input, const unsigned __int32 size)
{
    if (_cur_file_block_index == 0xffff && _cur_file_offset == 0xff)
        throw NMFSWarningException("当前并未打开文件！");
    if (input == nullptr)
        throw NMFSErrorException("空指针！");

    this->DeleteFile(_cur_file->cur_index);  // 先删除原有的块
    _cur_file->cur_index = 0xffff;
    _cur_file->file_size = static_cast<unsigned __int32>(0);
    _cur_file->block_num = static_cast<unsigned __int32>(0);

    try 
    {
        _cur_file->cur_index = this->WriteFile(input, size);
        _cur_file->file_size = size;
    }
    catch(NMFSErrorException)
    {
        throw;
    }

    this->FileToBlock(_cur_file, _cur_file_block_index, _cur_file_offset);
}

unsigned __int16 NMFSSpace::WriteFile(const unsigned char *input, const unsigned __int32 size)
{
    unsigned __int32 remaining_size = size;

    unsigned __int16 index_temp = this->GetFreeBlock();
    this->InitFileBlock(index_temp);

    if (index_temp == 0xffff)
        throw NMFSErrorException("空间大小不足！");

    _cur_file->block_num++;
    unsigned __int16 extra_index = index_temp;
    unsigned __int16 end;

    if (remaining_size > static_cast<unsigned __int32>(1024))
        end = static_cast<unsigned __int16>(1024);
    else
        end = static_cast<unsigned __int16>(remaining_size);

    for (unsigned __int16 p = static_cast<unsigned __int16>(0); 
        p < end; p++)
        _space_block[index_temp][p] = input[p];

    remaining_size -= static_cast<unsigned __int32>(end);

    while (remaining_size > static_cast<unsigned __int32>(0))
    {
        _header.FAT2[index_temp] = this->GetFreeBlock();
        this->InitFileBlock(_header.FAT2[index_temp]);
        if (_header.FAT2[index_temp] == 0xffff)
            throw NMFSErrorException("空间大小不足");

        _cur_file->block_num++;
        if (remaining_size > static_cast<unsigned __int32>(1024))
            end = static_cast<unsigned __int16>(1024);
        else
            end = static_cast<unsigned __int16>(remaining_size);

        for (unsigned __int16 p = static_cast<unsigned __int16>(0);
            p < end; p++)
            _space_block[index_temp][p] = input[p];

        remaining_size -= static_cast<unsigned __int32>(end);
        index_temp = _header.FAT2[index_temp];
    }

    return extra_index;
}

void NMFSSpace::InitFileBlock(const unsigned __int16 &block_index)
{
    if (block_index == 0xffff)
        return;

    for (int i = 0; i < 1024; i++)
        _space_block[block_index][i] = 0x00;
}

std::string NMFSSpace::GetSpaceName() noexcept
{
    if (_global_space_pointer == nullptr)
        return "";
    else
        return _global_space_pointer->_space_name;
}

std::string NMFSSpace::GetCurPath() noexcept
{
    if (_global_space_pointer == nullptr)
        return "";

    char name[9] = { '\0' };

    std::list<CurFloder>::iterator iter = _global_space_pointer->_path->begin();
    ++iter;

    std::string path = ":~";
    for (; iter != _global_space_pointer->_path->end(); ++iter)
    {
        memcpy(name, (*iter).name, 8);
        path += "/" + std::string(name);
    }

    return path;
}