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
		TEST_METHOD(TestMethod1)
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
        TEST_METHOD(TestMethod2)
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

        /* 根目录下删除文件测试 */

        /* 根目录下创建文件夹测试 */

        /* 根目录下删除文件夹测试 */

        /* 在一级文件夹下创建文件测试 */

        /* 在一级文件夹下删除文件测试 */

        /* 在一级文件夹下创建文件夹测试 */

        /* 在一级文件夹下删除文件夹测试 */

        /* 在二级文件夹下创建文件测试 */

        /* 在二级文件夹下删除文件测试 */

        /* 在二级文件夹下创建文件夹测试 */

        /* 在二级文件夹下删除文件夹测试 */

        /* 在一级文件夹下删除多级文件夹测试 */

        /* 在根目录下删除多级文件夹测试 */
    };

    /* 目录转移与显示测试 */
    TEST_CLASS(UnitTest3)
    {
    public:
        /* 从根目录向一级文件夹转移测试 */

        /* 从根目录向二级文件夹转移测试 */

        /* 从一级文件夹向根目录转移测试 */

        /* 从二级文件夹向根目录转移测试 */

        /* 从一级文件夹向另一个一级文件夹转移测试 */

        /* 从二级文件夹向另一个二级文件夹转移测试 */

        /* 显示当前文件夹的目录信息 */

        /* 显示根目录的目录信息 */
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