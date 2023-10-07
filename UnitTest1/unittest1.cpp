#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\database\Query.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
//using namespace System::Diagnostics;

namespace UnitTest1
{		
	
	alegen_it::database::Query *mpQuery;
	TEST_CLASS(UnitTest1)
	{
	private:

	public:

		TEST_CLASS_INITIALIZE(TestClassInitialize)
		{
			mpQuery = new alegen_it::database::Query();
			mpQuery->setConnectionParameters(L"localhost", L"database1", L"user1", L"Password1");

		}

		TEST_CLASS_CLEANUP(TestClassCleanup)
		{
			mpQuery->~Query();
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(TestClassInit)
			TEST_OWNER(L"Alegen")
			TEST_PRIORITY(1)
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(TestClassInit)
		{
			Assert::IsNotNull(mpQuery);
			
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(TestSQLConnection)
			TEST_OWNER(L"Alegen")
			TEST_PRIORITY(2)
		END_TEST_METHOD_ATTRIBUTE()
		TEST_METHOD(TestSQLConnection)
		{
			alegen_it::database::Query *pQuery = new alegen_it::database::Query();;
			pQuery->setConnectionParameters(L"localhost", L"not_existing", L"user1", L"Password1");
			Assert::IsFalse(pQuery->testConnection(), L"Connection to a wrong server fails");
			Logger::WriteMessage(pQuery->getMessage().c_str());
			pQuery->~Query();

			Assert::IsTrue(mpQuery->testConnection(), L"Connection to the server succeeds");
			Logger::WriteMessage(mpQuery->getMessage().c_str());

		}

		BEGIN_TEST_METHOD_ATTRIBUTE(TestQuery)
			TEST_OWNER(L"Alegen")
			TEST_PRIORITY(3)
		END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(TestQuery)
		{

			std::wstring result = mpQuery->ExecDirect(L"SELECT @@VERSION");
			Logger::WriteMessage(result.c_str());
			Assert::IsTrue(result.substr(0, 10) == std::wstring(L"Microsoft SQL Server 2022").substr(0,10), L"The expected version is Microsoft SQL Server 2022");

		}

		BEGIN_TEST_METHOD_ATTRIBUTE(TestCreate)
			TEST_OWNER(L"Alegen")
			TEST_PRIORITY(4)
		END_TEST_METHOD_ATTRIBUTE()
		TEST_METHOD(TestCreate)
		{

			Logger::WriteMessage("Drop table ...");
			std::wstring result = mpQuery->ExecDirect(L"DROP TABLE dbo.TABLE2");
			Logger::WriteMessage((L"... done. Result: " + result + L" - Message: " + mpQuery->getMessage()).c_str());

			Logger::WriteMessage("Create table ...");
			result = mpQuery->ExecDirect(L"CREATE TABLE dbo.TABLE2 (ID INT NOT NULL)");
			Logger::WriteMessage((L"... done. Result: " + result + L" - Message: " + mpQuery->getMessage()).c_str());

			Assert::IsTrue(result == std::wstring(L""),  L"Result should be empty");

			Logger::WriteMessage("Drop table ...");
			result = mpQuery->ExecDirect(L"DROP TABLE dbo.TABLE2");
			Logger::WriteMessage((L"... done. Result: " + result + L" - Message: " + mpQuery->getMessage()).c_str());

			Assert::IsTrue(result == std::wstring(L""), L"Result should be empty");

		}

		BEGIN_TEST_METHOD_ATTRIBUTE(TestSetParameter)
			TEST_OWNER(L"Alegen")
			TEST_PRIORITY(5)
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(TestSetParameter)
		{


			struct {
				float aFloat;
				int aInt;
				char aString[10];
			} aStruct = { 1.1, 12, "pippo" };

			Logger::WriteMessage("Add float param ...");
			mpQuery->addParameter(new alegen_it::database::Parameter(&aStruct.aFloat, L"FLOAT1"));
			Logger::WriteMessage(L"... done.");

			Logger::WriteMessage("Add int param ...");
			mpQuery->addParameter(new alegen_it::database::Parameter(&aStruct.aInt, L"INT1"));
			Logger::WriteMessage(L"... done.");

			Logger::WriteMessage("Add string param ...");
			mpQuery->addParameter(new alegen_it::database::Parameter(aStruct.aString, 10, L"STRING1"));
			Logger::WriteMessage(L"... done.");


		}



		BEGIN_TEST_METHOD_ATTRIBUTE(TestInsert)
			TEST_OWNER(L"Alegen")
			TEST_PRIORITY(4)
		END_TEST_METHOD_ATTRIBUTE()
		TEST_METHOD(TestInsert)
		{
			//create the table
			Logger::WriteMessage("Create table ...");
			std::wstring result = mpQuery->ExecDirect(L"CREATE TABLE dbo.TABLE2 (ID INT NOT NULL)");
			Logger::WriteMessage((L"... done. Result: " + result + L" - Message: " + mpQuery->getMessage()).c_str());
			Logger::WriteMessage(result.c_str());
			Assert::IsTrue(result == std::wstring(L""), L"Result should be empty");

			mpQuery->ClearParameters();
			Logger::WriteMessage("Add int param ...");
			int id;
			mpQuery->addParameter(new alegen_it::database::Parameter(&id, L"ID"));
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());

			Logger::WriteMessage("Insert 1 ...");
			id = 1;
			bool result1 = mpQuery->Insert(L"TABLE2");
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());
			Assert::IsTrue(result1);
			Logger::WriteMessage("Insert 3 ...");
			id = 3;
			result1 = mpQuery->Insert(L"TABLE2");
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());
			Assert::IsTrue(result1);



		}


	};



}