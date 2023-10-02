#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\database\Query.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
//using namespace System::Diagnostics;

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
			Logger::WriteMessage(pQuery->getMessage().c_str());

			pQuery->setConnectionParameters(L"localhost", L"database1", L"user1", L"Password1");
			Assert::IsTrue(pQuery->testConnection(), L"Connection to the server succeeds");
			Logger::WriteMessage(pQuery->getMessage().c_str());


			pQuery->~Query();
		}

		TEST_METHOD(TestQuery)
		{
			alegen_it::database::Query *pQuery = new alegen_it::database::Query();
			pQuery->setConnectionParameters(L"localhost", L"database1", L"user1", L"Password1");

			std::wstring result = pQuery->ExecDirect(L"SELECT @@VERSION");
			Logger::WriteMessage(result.c_str());
			Assert::IsTrue(result.substr(0, 10) == std::wstring(L"Microsoft SQL Server 2022").substr(0,10), L"The expected version is Microsoft SQL Server 2022");
		}


	};
}