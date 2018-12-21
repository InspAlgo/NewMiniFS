#include "stdafx.h"
#include "CppUnitTest.h"

#include <iostream>

#include "../NewMiniFS.Core/include/nmfs_space.h"
#include "../NewMiniFS.Core/include/exceptions/error_exception.h"
#include "../NewMiniFS.Core/include/exceptions/warning_exception.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NewMiniFSUnitTests
{		
    /* 空间创建与打开测试 */
	TEST_CLASS(UnitTest1)
	{
	public:

        /* 创建空间测试 */
		TEST_METHOD(TM_1_1)
		{
            try
            {
                NMFSSpace *space = nullptr;
                NMFSSpace::NewNMFSSpace("test");
                space = NMFSSpace::GetActiveNMFSSpace();
                Assert::AreEqual(space != nullptr, true);
                NMFSSpace::Exit();
            }
            catch(NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
		}

        /* 打开空间测试 */
        TEST_METHOD(TM_1_2)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();
                Assert::AreEqual(space != nullptr, true);
                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
        }
	};

    /* 文件与文件夹的创建、删除测试 */
    TEST_CLASS(UnitTest2)
    {
    public:

        /* 根目录下创建文件测试 */
        TEST_METHOD(TM_2_1)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();
                
                space->Create("rf1", "txt");
                
                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 根目录下删除文件测试 */
        TEST_METHOD(TM_2_2)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                space->Delete("rf1", "txt");

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 根目录下创建文件夹测试 */
        TEST_METHOD(TM_2_3)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                space->MakeDirectory("rF1");

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 根目录下删除文件夹测试 */
        TEST_METHOD(TM_2_4)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                space->RemoveDirectory("rF1");

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 在一级文件夹下创建文件测试 */
        TEST_METHOD(TM_2_5)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                std::queue<std::string>cd_path;
                cd_path.push(".");
                cd_path.push("rF1");
                space->ChangeDirectory(cd_path);

                space->Create("Ff1", "xxxx");

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 在一级文件夹下删除文件测试 */
        TEST_METHOD(TM_2_6)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                std::queue<std::string>cd_path;
                cd_path.push(".");
                cd_path.push("rF1");
                space->ChangeDirectory(cd_path);

                space->Delete("Ff1", "xxxx");

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 在一级文件夹下创建文件夹测试 */
        TEST_METHOD(TM_2_7)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                std::queue<std::string>cd_path;
                cd_path.push(".");
                cd_path.push("rF1");
                space->ChangeDirectory(cd_path);

                space->MakeDirectory("FF1");

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 在一级文件夹下删除文件夹测试 */
        TEST_METHOD(TM_2_8)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                std::queue<std::string>cd_path;
                cd_path.push(".");
                cd_path.push("rF1");
                space->ChangeDirectory(cd_path);

                space->RemoveDirectory("FF1");

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 在二级文件夹下创建文件测试 */
        TEST_METHOD(TM_2_9)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                std::queue<std::string>cd_path;
                cd_path.push(".");
                cd_path.push("rF1");
                cd_path.push("FF1");
                space->ChangeDirectory(cd_path);

                space->Create("FFf1", "txt");

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 在二级文件夹下删除文件测试 */
        TEST_METHOD(TM_2_10)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                std::queue<std::string>cd_path;
                cd_path.push(".");
                cd_path.push("rF1");
                cd_path.push("FF1");
                space->ChangeDirectory(cd_path);

                space->Delete("FFf1", "txt");

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 在二级文件夹下创建文件夹测试 */
        TEST_METHOD(TM_2_11)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                std::queue<std::string>cd_path;
                cd_path.push(".");
                cd_path.push("rF1");
                cd_path.push("FF1");
                space->ChangeDirectory(cd_path);

                space->MakeDirectory("FFF1");

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 在二级文件夹下删除文件夹测试 */
        TEST_METHOD(TM_2_12)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                std::queue<std::string>cd_path;
                cd_path.push(".");
                cd_path.push("rF1");
                cd_path.push("FF1");
                space->ChangeDirectory(cd_path);

                space->RemoveDirectory("FFF1");

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 在一级文件夹下删除多级文件夹测试 */
        TEST_METHOD(TM_2_13)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();
                 
                std::queue<std::string>cd_path;
                cd_path.push(".");
                cd_path.push("rF1");
                cd_path.push("FF1");
                cd_path.push("FFF1");
                space->ChangeDirectory(cd_path);

                space->MakeDirectory("FFFF1");

                std::queue<std::string>cd_path2;
                cd_path2.push("..");
                cd_path2.push("..");
                cd_path2.push("..");
                cd_path2.push("..");
                cd_path2.push("rF1");
                space->ChangeDirectory(cd_path2);
                
                space->RemoveDirectory("FF1");

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 在根目录下删除多级文件夹测试 */
        TEST_METHOD(TM_2_14)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                space->RemoveDirectory("rF1");

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }
    };

    /* 目录转移与显示测试 */
    TEST_CLASS(UnitTest3)
    {
    public:
        /* 从根目录向一级文件夹转移测试 */
        TEST_METHOD(TM_3_1)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                std::queue<std::string>cd_path;
                cd_path.push(".");
                cd_path.push("rF1");

                space->ChangeDirectory(cd_path);

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 从根目录向二级文件夹转移测试 */
        TEST_METHOD(TM_3_2)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                std::queue<std::string>cd_path;
                cd_path.push(".");
                cd_path.push("rF1");
                cd_path.push("FF1");

                space->ChangeDirectory(cd_path);

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 从一级文件夹向根目录转移测试 */
        TEST_METHOD(TM_3_3)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                std::queue<std::string>cd_path;
                cd_path.push(".");
                cd_path.push("rF1");
                cd_path.push("..");

                space->ChangeDirectory(cd_path);

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 从二级文件夹向根目录转移测试 */
        TEST_METHOD(TM_3_4)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                std::queue<std::string>cd_path;
                cd_path.push(".");
                cd_path.push("rF1");
                cd_path.push("FF1");
                cd_path.push("..");
                cd_path.push("..");

                space->ChangeDirectory(cd_path);

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 从一级文件夹向另一个一级文件夹转移测试 */
        TEST_METHOD(TM_3_5)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                std::queue<std::string>cd_path;
                cd_path.push(".");
                cd_path.push("rF1");

                space->ChangeDirectory(cd_path);

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 从二级文件夹向另一个二级文件夹转移测试 */
        TEST_METHOD(TM_3_6)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                std::queue<std::string>cd_path;
                cd_path.push(".");
                cd_path.push("rF1");

                space->ChangeDirectory(cd_path);

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 显示当前文件夹的目录信息 */
        TEST_METHOD(TM_3_7)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                std::queue<std::string>cd_path;
                cd_path.push(".");
                cd_path.push("rF1");

                space->ChangeDirectory(cd_path);
                std::list<TreeNode>tree;
                space->List(tree, false);
                std::string blank;
                for (auto i:tree)
                {
                    blank = "";
                    for (unsigned __int16 blank_num = static_cast<unsigned __int16>(0);
                        blank_num <= i.index; blank_num++)
                    {
                        blank += " ";
                    }
                    
                    char cname[9] = { '\0' };
                    memcpy(cname, i.name, 8);

                    blank += "┕   " + std::string(cname);
                    Logger::WriteMessage((char*)blank.data());
                }

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }

        /* 显示根目录的目录信息 */
        TEST_METHOD(TM_3_8)
        {
            try
            {
                NMFSSpace::System("test");
                NMFSSpace *space = NMFSSpace::GetActiveNMFSSpace();

                std::queue<std::string>cd_path;
                cd_path.push(".");
                cd_path.push("rF1");

                space->ChangeDirectory(cd_path);

                std::list<TreeNode>tree;
                space->List(tree, true);
                std::string blank;
                for (auto i : tree)
                {
                    blank = "";
                    for (unsigned __int16 blank_num = static_cast<unsigned __int16>(0);
                        blank_num < i.index; blank_num++)
                    {
                        blank += " ";
                    }

                    bool flag1 = true;
                    for (int k = 0; k < 8; k++)
                    {
                        if (i.name[k] != 0x00)
                            flag1 = false;
                    }

                    if (flag1)
                        blank += "test";
                    else
                    {
                        char cname[9] = { '\0' };
                        memcpy(cname, i.name, 8);
                        char ctype[5] = { '\0' };
                        memcpy(ctype, i.type, 4);

                        blank += "┕   " + std::string(cname) + std::string(ctype);
                    }
                    Logger::WriteMessage((char*)blank.data());
                }

                NMFSSpace::Exit();
            }
            catch (NMFSWarningException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (NMFSErrorException &ex)
            {
                Logger::WriteMessage((char*)ex.GetExceptMessage().data());
            }
            catch (...)
            {
                Assert::Fail();
            }
        }
    };

    /* 文件打开关闭及读写测试 */
    TEST_CLASS(UnitTest4)
    {
    public:
        /* 在根目录打开文件测试 */

        /* 在根目录关闭文件测试 */

        /* 在一级文件夹下打开文件测试 */

        /* 在一级文件夹下关闭文件测试 */

        /* 在二级文件夹下打开文件测试 */

        /* 在二级文件夹下关闭文件测试 */

        /* 在根目录写文件测试 */

        /* 在根目录读文件测试 */

        /* 在一级文件夹下写文件测试 */

        /* 在一级文件夹下读文件测试 */

        /* 在二级文件夹下写文件测试 */

        /* 在二级文件夹下读文件测试 */
    };
}