#include "../include/nmfs_space.h"

#include <iostream>

NMFSSpace* volatile NMFSSpace::_global_space_pointer = nullptr;

void  NMFSSpace::NewNMFSSpace(const std::string &space_name)
{
    if (_global_space_pointer != nullptr)
    {
        // TODO: Ӧ��д���׳��쳣
        std::cout << "Warning: ��ǰ�Ѵ�һ�����ļ�ϵͳ��" << std::endl;
        return;
    }

    /* ��鵱ǰλ�����Ƿ���ͬ���ļ� */
    FILE *space_file = nullptr;
    space_file = fopen((char*)space_name.data(), "r");
    if (space_file != nullptr)
    {
        fclose(space_file);
        // TODO: Ӧ��д���׳��쳣
        std::cout << "Warning: ��ǰ��ͬ���ļ���" << std::endl;
        return;
    }
    fclose(space_file);

    /* �ڵ�ǰλ�ô���һ���ռ��ļ����Ժ���д�ش��� */
    space_file = fopen((char*)space_name.data(), "wb+");
    _fseeki64(space_file, 512 * 1024 - 1, SEEK_SET);
    fwrite("\0", 1, 1, space_file);
    fclose(space_file);

    /* �ռ䵥��ʵ���� */
    _global_space_pointer = new NMFSSpace();
    _global_space_pointer->_space_name = space_name;
}

NMFSSpace*  NMFSSpace::GetActiveNMFSSpace()
{
    if (_global_space_pointer == nullptr)
    {
        // TODO: Ӧ��д���׳��쳣
        std::cout << "Warning: ��ǰ��δ��һ�����ļ�ϵͳ��" << std::endl;
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

    _space_block = new unsigned char*[512];  // 512 ����
    for (int i = 0; i < 512; i++)
        _space_block[i] = new unsigned char[1024];  // ÿ���� 1KB

    /* FAT1 ��ʼ�� */
    for (int i = 0; i < 512; i++)
    {
        unsigned char bit = static_cast<unsigned char>(1 << (i % 8));
        _header.FAT1[i / 8] &= ~bit;
    }
    _header.FAT1[0] |= static_cast<unsigned char>(1);  // �ռ�ͷռ�ÿ�
    _header.FAT1[0] |= static_cast<unsigned char>(1 << 1);  // FAT2 ռ�ÿ�
    _header.FAT1[0] |= static_cast<unsigned char>(1 << 2);  // ��Ŀ¼ռ�ÿ�

    _cur_floder = nullptr;
    _cur_file = nullptr;
    _path = nullptr;

    /* ��Ŀ¼��ʼ�� */
    _cur_floder = new File();

    for (int i = 0; i < 8; i++)  // ��Ŀ¼������
        _cur_floder->name[i] = '\0';

    _cur_floder->last_index = _cur_floder->cur_index = static_cast<unsigned __int16>(2);

    for (int i = 0; i < 32; i++)  // ȫ���� 0��˵��Ϊ�ļ�������
    {
        unsigned char bit = static_cast<unsigned char>(1 << (i % 8));
        _cur_floder->type[i / 8] &= ~bit;
    }

    _cur_floder->file_size = static_cast<unsigned __int32>(0);
    _cur_floder->block_num = static_cast<unsigned __int32>(1);

    /* TODO: ��Ŀ¼д���ڴ�� */
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

    _space_block = new unsigned char*[512];  // 512 ����
    for (int i = 0; i < 512; i++)
        _space_block[i] = new unsigned char[1024];  // ÿ���� 1KB

    _cur_floder = nullptr;
    _cur_file = nullptr;
    _path = nullptr;

    /* �������е�����д���ڴ��� */
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
        // TODO: Ӧ��д���׳��쳣
        std::cout << "Warning: ��ǰ�Ѵ�һ�����ļ�ϵͳ��" << std::endl;
        return;
    }

    char magic_num[4] = { '\0' };

    /* TODO: �жϵ�ǰλ���Ƿ��д��ļ��������ļ�ͷħ��У�� */
    FILE *space_file = nullptr;
    space_file = fopen((char*)space_name.data(), "rb+");

    if (space_file == nullptr)
    {
        fclose(space_file);
        // TODO: Ӧ��д���׳��쳣
        std::cout << "Warning: ��ǰĿ¼���޴��ļ���" << std::endl;
        return;
    }

    _fseeki64(space_file, 0LL, SEEK_SET);
    fread(&magic_num, sizeof(magic_num), 1, space_file);
    fclose(space_file);

    if (magic_num[0] != 'N' || magic_num[1] != 'M' 
        || magic_num[2] != 'F' || magic_num[3] != 'S')
    {
        // TODO: Ӧ��д���׳��쳣
        std::cout << "Warning: ��NMFS�ռ��ļ����ͣ�" << std::endl;
        return;
    }

    _global_space_pointer = new NMFSSpace(space_name);
    _global_space_pointer->_space_name = space_name;
}

void NMFSSpace::Exit()
{
    if (_global_space_pointer == nullptr)
    {
        // TODO: Ӧ��д���׳��쳣
        std::cout << "Warning: ��ǰ��δ��һ�����ļ�ϵͳ��" << std::endl;
        return;
    }

    _global_space_pointer->HeaderToBlock();

    /* ���ڴ�ռ������д�ش��� */
    FILE *space_file = fopen((char*)_global_space_pointer->_space_name.data(), "rb+");
    _fseeki64(space_file, 0LL, SEEK_SET);
    fwrite(&_global_space_pointer->_space_block, sizeof(_global_space_pointer->_space_block), 1, space_file);
    fclose(space_file);

    /* ɾ������ */
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
        _space_block[1][j] = static_cast<unsigned char>((_header.FAT2[i] & 0xff00) >> 8);  // ȡ��8λ
        _space_block[1][j + 1] = static_cast<unsigned char>(_header.FAT2[i] & 0x00ff);  // ȡ��8λ
    }
}

void NMFSSpace::FileToBlock(const File *file, const unsigned __int16 &block_index, const unsigned __int8 &offset)
{
    if (offset > static_cast<unsigned __int8>(41))
    {
        // TODO: Ӧ��д���׳��쳣
        std::cout << "Error: ����ƫ����Խ�磡" << std::endl;
        return;
    }

    if (file == nullptr)
    {
        // TODO: Ӧ��д���׳��쳣
        std::cout << "Error: ��ָ�룡" << std::endl;
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
        // TODO: Ӧ��д���׳��쳣
        std::cout << "Error: ����ƫ����Խ�磡" << std::endl;
        return;
    }

    if (file == nullptr)
    {
        // TODO: Ӧ��д���׳��쳣
        std::cout << "Error: ��ָ�룡" << std::endl;
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
    // 1. �ָ�·��
    std::queue<std::string>cd_path;  // ��ת��·��

    // 2. �޸�·������
    while (!cd_path.empty())
    {
        std::string cur_floder = cd_path.front();
        cd_path.pop();

        if (cur_floder == ".")  // �����ڵ�ǰ�ļ���
            continue;
        else if (cur_floder == "..")  // ����һ���ļ���ת��
        {
            unsigned __int16 to_index = _cur_floder->last_index;
            this->BlockToFile(_cur_floder, to_index, static_cast<unsigned __int16>(0));

            if (_path->size() > 1)  // ��ǰλ�ó�ջ
                _path->pop();
            else if (_path->size() == 1)  // �Ѿ������Ŀ¼������������һ���ļ���ת��
                continue;
            else
            {
                // TODO: �׳��쳣
                std::cout << "ERROR: λ��ת�ƴ���!" << std::endl;
                return;
            }

            // Ҫת�Ƶ�λ�ý�ջ
            CurFloder temp;
            memcpy(temp.name, _cur_floder->name, sizeof(_cur_floder->name));
            temp.index = _cur_floder->cur_index;
            _path->push(temp);
        }
        else  // ����һ���ļ���ת��
        {
            // a. �ļ������ƺϷ����ж�
            if (cur_floder.length() > 8)
            {
                // TODO: �������
                std::cout << "Ŀ¼������" << std::endl;
                return;
            }

            // b. ���ж���һ���ļ����Ƿ����
            // c. ת��
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