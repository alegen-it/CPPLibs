#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\database\Query.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestClassInit)
		{
			alegen_it::database::Query *pQuery = new alegen_it::database::Query();
			Assert::IsNotNull(pQuery);
			pQuery->~Query();
		}

		TEST_METHOD(TestSQLConnection)
		{
			alegen_it::database::Query *pQuery = new alegen_it::database::Query();
			pQuery->setConnectionParameters(L"localhost", L"notExist", L"user1", L"Password1");
			Assert::IsFalse(pQuery->testConnection(), L"Connection to a wrong server fails");

			pQuery->setConnectionParameters(L"localhost", L"database1", L"user1", L"Password1");
			Assert::IsTrue(pQuery->testConnection(), L"Connection to the server succeeds");

			pQuery->~Query();
		}


	};
}