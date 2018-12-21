#include "stdafx.h"
#include "CppUnitTest.h"

#include <iostream>

#include "../NewMiniFS.Core/include/nmfs_space.h"
#include "../NewMiniFS.Core/include/exceptions/error_exception.h"
#include "../NewMiniFS.Core/include/exceptions/warning_exception.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NewMiniFSUnitTests
{		
    /* �ռ䴴����򿪲��� */
	TEST_CLASS(UnitTest1)
	{
	public:

        /* �����ռ���� */
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

        /* �򿪿ռ���� */
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

    /* �ļ����ļ��еĴ�����ɾ������ */
    TEST_CLASS(UnitTest2)
    {
    public:

        /* ��Ŀ¼�´����ļ����� */
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

        /* ��Ŀ¼��ɾ���ļ����� */
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

        /* ��Ŀ¼�´����ļ��в��� */
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

        /* ��Ŀ¼��ɾ���ļ��в��� */
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

        /* ��һ���ļ����´����ļ����� */
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

        /* ��һ���ļ�����ɾ���ļ����� */
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

        /* ��һ���ļ����´����ļ��в��� */
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

        /* ��һ���ļ�����ɾ���ļ��в��� */
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

        /* �ڶ����ļ����´����ļ����� */
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

        /* �ڶ����ļ�����ɾ���ļ����� */
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

        /* �ڶ����ļ����´����ļ��в��� */
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

        /* �ڶ����ļ�����ɾ���ļ��в��� */
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

        /* ��һ���ļ�����ɾ���༶�ļ��в��� */
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

        /* �ڸ�Ŀ¼��ɾ���༶�ļ��в��� */
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

    /* Ŀ¼ת������ʾ���� */
    TEST_CLASS(UnitTest3)
    {
    public:
        /* �Ӹ�Ŀ¼��һ���ļ���ת�Ʋ��� */
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

        /* �Ӹ�Ŀ¼������ļ���ת�Ʋ��� */
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

        /* ��һ���ļ������Ŀ¼ת�Ʋ��� */
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

        /* �Ӷ����ļ������Ŀ¼ת�Ʋ��� */
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

        /* ��һ���ļ�������һ��һ���ļ���ת�Ʋ��� */
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

        /* �Ӷ����ļ�������һ�������ļ���ת�Ʋ��� */
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

        /* ��ʾ��ǰ�ļ��е�Ŀ¼��Ϣ */
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

                    blank += "��   " + std::string(cname);
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

        /* ��ʾ��Ŀ¼��Ŀ¼��Ϣ */
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

                        blank += "��   " + std::string(cname) + std::string(ctype);
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

    /* �ļ��򿪹رռ���д���� */
    TEST_CLASS(UnitTest4)
    {
    public:
        /* �ڸ�Ŀ¼���ļ����� */

        /* �ڸ�Ŀ¼�ر��ļ����� */

        /* ��һ���ļ����´��ļ����� */

        /* ��һ���ļ����¹ر��ļ����� */

        /* �ڶ����ļ����´��ļ����� */

        /* �ڶ����ļ����¹ر��ļ����� */

        /* �ڸ�Ŀ¼д�ļ����� */

        /* �ڸ�Ŀ¼���ļ����� */

        /* ��һ���ļ�����д�ļ����� */

        /* ��һ���ļ����¶��ļ����� */

        /* �ڶ����ļ�����д�ļ����� */

        /* �ڶ����ļ����¶��ļ����� */
    };
}