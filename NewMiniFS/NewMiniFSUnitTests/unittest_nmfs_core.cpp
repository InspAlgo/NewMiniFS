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

        /* �򿪿ռ���� */
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

        /* ��Ŀ¼��ɾ���ļ����� */

        /* ��Ŀ¼�´����ļ��в��� */

        /* ��Ŀ¼��ɾ���ļ��в��� */

        /* ��һ���ļ����´����ļ����� */

        /* ��һ���ļ�����ɾ���ļ����� */

        /* ��һ���ļ����´����ļ��в��� */

        /* ��һ���ļ�����ɾ���ļ��в��� */

        /* �ڶ����ļ����´����ļ����� */

        /* �ڶ����ļ�����ɾ���ļ����� */

        /* �ڶ����ļ����´����ļ��в��� */

        /* �ڶ����ļ�����ɾ���ļ��в��� */

        /* ��һ���ļ�����ɾ���༶�ļ��в��� */

        /* �ڸ�Ŀ¼��ɾ���༶�ļ��в��� */
    };

    /* Ŀ¼ת������ʾ���� */
    TEST_CLASS(UnitTest3)
    {
    public:
        /* �Ӹ�Ŀ¼��һ���ļ���ת�Ʋ��� */

        /* �Ӹ�Ŀ¼������ļ���ת�Ʋ��� */

        /* ��һ���ļ������Ŀ¼ת�Ʋ��� */

        /* �Ӷ����ļ������Ŀ¼ת�Ʋ��� */

        /* ��һ���ļ�������һ��һ���ļ���ת�Ʋ��� */

        /* �Ӷ����ļ�������һ�������ļ���ת�Ʋ��� */

        /* ��ʾ��ǰ�ļ��е�Ŀ¼��Ϣ */

        /* ��ʾ��Ŀ¼��Ŀ¼��Ϣ */
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