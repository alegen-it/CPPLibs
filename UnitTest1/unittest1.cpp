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

			struct {
				float aFloat;
				int aInt;
				char aString[10];
			} aOutStruct = { 1.3, 13, "pippo1" };


			Logger::WriteMessage("Add float param ...");
			mpQuery->addParameter(new alegen_it::database::Parameter(&aStruct.aFloat, L"FLOAT1"));
			Logger::WriteMessage(L"... done.");

			Logger::WriteMessage("Add float Output param ...");
			mpQuery->addOutParameter(new alegen_it::database::Parameter(&aOutStruct.aFloat, L"FLOAT2"));
			Logger::WriteMessage(L"... done.");


			Logger::WriteMessage("Add int param ...");
			mpQuery->addParameter(new alegen_it::database::Parameter(&aStruct.aInt, L"INT1"));
			Logger::WriteMessage(L"... done.");

			Logger::WriteMessage("Add int output param ...");
			mpQuery->addOutParameter(new alegen_it::database::Parameter(&aOutStruct.aInt, L"INT2"));
			Logger::WriteMessage(L"... done.");


			Logger::WriteMessage("Add string param ...");
			mpQuery->addParameter(new alegen_it::database::Parameter(aStruct.aString, 10, L"STRING1"));
			Logger::WriteMessage(L"... done.");

			Logger::WriteMessage("Add string output param ...");
			mpQuery->addParameter(new alegen_it::database::Parameter(aOutStruct.aString, 10, L"STRING2"));
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

			//SQL SERVER SYNTAX FOR AUTO INCREMENT ID
			std::wstring result = mpQuery->ExecDirect(L"CREATE TABLE dbo.TABLE2 (ID INT IDENTITY(1,1) PRIMARY KEY,INTVALUE1 INT )");
			Logger::WriteMessage((L"... done. Result: " + result + L" - Message: " + mpQuery->getMessage()).c_str());
			Logger::WriteMessage(result.c_str());
			Assert::IsTrue(result == std::wstring(L""), L"Result should be empty");

			mpQuery->ClearParameters();
			Logger::WriteMessage("Add int param ...");
			int id;
			int intValue1;
			//mpQuery->addParameter(new alegen_it::database::Parameter(&id, L"ID"));
			mpQuery->addParameter(new alegen_it::database::Parameter(&intValue1, L"INTVALUE1"));

			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());

			Logger::WriteMessage("Insert 1 ...");
			id = 1;
			intValue1 = 2;
			bool result1 = mpQuery->Insert(L"TABLE2");
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());
			Assert::IsTrue(result1);
			Logger::WriteMessage("Insert 3 ...");
			id = 3;
			intValue1 = 4;
			result1 = mpQuery->Insert(L"TABLE2");
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());
			Assert::IsTrue(result1);

		}

		BEGIN_TEST_METHOD_ATTRIBUTE(TestSelect)
			TEST_OWNER(L"Alegen")
			TEST_PRIORITY(5)
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(TestSelect)
		{
			//create the table
			Logger::WriteMessage("Create table ...");

			//SQL SERVER SYNTAX FOR AUTO INCREMENT ID
			std::wstring result = mpQuery->ExecDirect(L"DROP TABLE dbo.TABLE3");
			result = mpQuery->ExecDirect(L"CREATE TABLE dbo.TABLE3 (ID INT IDENTITY(1,1) PRIMARY KEY,INTVALUE1 INT, FLOATVALUE1 FLOAT, STRINGVALUE1 varchar(10) )");
			Logger::WriteMessage((L"... done. Result: " + result + L" - Message: " + mpQuery->getMessage()).c_str());
			Logger::WriteMessage(result.c_str());
			Assert::IsTrue(result == std::wstring(L""), L"Result should be empty");

			mpQuery->ClearParameters();
			Logger::WriteMessage("Add int param ...");
			int intValue1;
			mpQuery->addParameter(new alegen_it::database::Parameter(&intValue1, L"INTVALUE1"));
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());

			Logger::WriteMessage("Add float param ...");
			float floatValue1;
			mpQuery->addParameter(new alegen_it::database::Parameter(&floatValue1, L"FLOATVALUE1"));
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());

			Logger::WriteMessage("Add string param ...");
			char stringValue1[10];
			mpQuery->addParameter(new alegen_it::database::Parameter(stringValue1, 10, L"STRINGVALUE1"));
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());


			Logger::WriteMessage("Insert 1, 12.3, pippo ...");
			intValue1 = 1;
			floatValue1 = 12.3;
			strcpy(stringValue1, "pippo");
			bool result1 = mpQuery->Insert(L"TABLE3");
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());
			Assert::IsTrue(result1);

			Logger::WriteMessage("Insert 3, 12.5, pippo1 ...");
			intValue1 = 3;
			floatValue1 = 12.5;
			strcpy(stringValue1, "pippo1");
			result1 = mpQuery->Insert(L"TABLE3");
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());
			Assert::IsTrue(result1);

			/*****************************/
			/* select phase              */
			/*****************************/

			mpQuery->ClearParameters();

			Logger::WriteMessage("Add int param ...");
			int intValue2;
			mpQuery->addParameter(new alegen_it::database::Parameter(&intValue2, L"INTVALUE1"));
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());

			Logger::WriteMessage("Add float output param ...");
			float floatValue2 = -1.0;
			mpQuery->addOutParameter(new alegen_it::database::Parameter(&floatValue2, L"FLOATVALUE1"));
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());

			Logger::WriteMessage("Add string param ...");
			char stringValue2[10] = "not found";
			mpQuery->addOutParameter(new alegen_it::database::Parameter(stringValue2, 10, L"STRINGVALUE1"));
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());


			Logger::WriteMessage("Select 1 ...");
			intValue2 = 1;
			result1 = mpQuery->Select(L"TABLE3");
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());
			char message[100];
			sprintf(message, "FLOATVALUE1 = %f", floatValue2);
			Logger::WriteMessage(message);
			Assert::AreEqual(12.3F, floatValue2);
			
			sprintf(message, "STRINGVALUE1 = %s", stringValue2);
			Logger::WriteMessage(message);
			Assert::AreEqual("pippo", stringValue2);
			Assert::IsTrue(result1);

			Logger::WriteMessage("Select 2 ...");
			intValue2 = 2;
			result1 = mpQuery->Select(L"TABLE3");
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());
			Assert::IsFalse(result1, L"No data found");


			Logger::WriteMessage("Select 3 ...");
			intValue2 = 3;
			result1 = mpQuery->Select(L"TABLE3");
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());
			
			sprintf(message, "FLOATVALUE1 = %f", floatValue2);
			Logger::WriteMessage(message);
			Assert::AreEqual(12.5F, floatValue2);

			sprintf(message, "STRINGVALUE1 = %s", stringValue2);
			Logger::WriteMessage(message);
			Assert::AreEqual("pippo1", stringValue2);
			Assert::IsTrue(result1);



		}



		BEGIN_TEST_METHOD_ATTRIBUTE(TestSelect2Tables)
			TEST_OWNER(L"Alegen")
			TEST_PRIORITY(5)
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(TestSelect2Tables)
		{
			//create the table
			Logger::WriteMessage("Create tables ...");

			//SQL SERVER SYNTAX FOR AUTO INCREMENT ID
			std::wstring result = mpQuery->ExecDirect(L"DROP TABLE dbo.TABLE3");
			result = mpQuery->ExecDirect(L"CREATE TABLE dbo.TABLE3 (ID INT IDENTITY(1,1) PRIMARY KEY,INTVALUE1 INT, FLOATVALUE1 FLOAT, STRINGVALUE1 varchar(10) )");
			Logger::WriteMessage((L"... done. Result: " + result + L" - Message: " + mpQuery->getMessage()).c_str());
			Logger::WriteMessage(result.c_str());
			Assert::IsTrue(result == std::wstring(L""), L"Result should be empty");

			result = mpQuery->ExecDirect(L"DROP TABLE dbo.TABLE4");
			result = mpQuery->ExecDirect(L"CREATE TABLE dbo.TABLE4 (ID INT IDENTITY(1,1) PRIMARY KEY,INTVALUE4 INT, FLOATVALUE4 FLOAT, STRINGVALUE4 varchar(10) )");
			Logger::WriteMessage((L"... done. Result: " + result + L" - Message: " + mpQuery->getMessage()).c_str());
			Logger::WriteMessage(result.c_str());
			Assert::IsTrue(result == std::wstring(L""), L"Result should be empty");


			mpQuery->ClearParameters();
			Logger::WriteMessage("Add int param ...");
			int intValue1;
			mpQuery->addParameter(new alegen_it::database::Parameter(&intValue1, L"INTVALUE1"));
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());

			Logger::WriteMessage("Add float param ...");
			float floatValue1;
			mpQuery->addParameter(new alegen_it::database::Parameter(&floatValue1, L"FLOATVALUE1"));
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());

			Logger::WriteMessage("Add string param ...");
			char stringValue1[10];
			mpQuery->addParameter(new alegen_it::database::Parameter(stringValue1, 10, L"STRINGVALUE1"));
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());


			Logger::WriteMessage("Insert 1, 12.3, pippo ...");
			intValue1 = 1;
			floatValue1 = 12.3;
			strcpy(stringValue1, "pippo");
			bool result1 = mpQuery->Insert(L"TABLE3");
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());
			Assert::IsTrue(result1);

			Logger::WriteMessage("Insert 3, 12.5, success ...");
			intValue1 = 3;
			floatValue1 = 12.5;
			strcpy(stringValue1, "success");
			result1 = mpQuery->Insert(L"TABLE3");
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());
			Assert::IsTrue(result1);


			mpQuery->ClearParameters();
			Logger::WriteMessage("Add int param ...");
			int intValue4;
			mpQuery->addParameter(new alegen_it::database::Parameter(&intValue4, L"INTVALUE4"));
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());

			Logger::WriteMessage("Add float param ...");
			float floatValue4;
			mpQuery->addParameter(new alegen_it::database::Parameter(&floatValue4, L"FLOATVALUE4"));
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());

			Logger::WriteMessage("Add string param ...");
			char stringValue4[10];
			mpQuery->addParameter(new alegen_it::database::Parameter(stringValue4, 10, L"STRINGVALUE4"));
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());


			Logger::WriteMessage("Insert 2, 12.8, pippo4 ...");
			intValue4 = 2;
			floatValue4 = 12.8;
			strcpy(stringValue4, "pippo4");
			result1 = mpQuery->Insert(L"TABLE4");
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());
			Assert::IsTrue(result1);

			Logger::WriteMessage("Insert 3, 12.9, pippo5 ...");
			intValue4 = 3;
			floatValue4 = 12.9;
			strcpy(stringValue4, "pippo5");
			result1 = mpQuery->Insert(L"TABLE4");
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());
			Assert::IsTrue(result1);


			/*****************************/
			/* select phase              */
			/*****************************/

			mpQuery->ClearParameters();


			Logger::WriteMessage("Add float output param ...");
			float floatValue2 = -1.0;
			mpQuery->addOutParameter(new alegen_it::database::Parameter(&floatValue2, L"TABLE3.FLOATVALUE1"));
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());

			Logger::WriteMessage("Add string param ...");
			char stringValue2[10] = "not found";
			mpQuery->addOutParameter(new alegen_it::database::Parameter(stringValue2, 10, L"TABLE3.STRINGVALUE1"));
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());

			Logger::WriteMessage("Add string param ...");
			char stringValue5[10] = "not found";
			mpQuery->addOutParameter(new alegen_it::database::Parameter(stringValue5, 10, L"TABLE4.STRINGVALUE4"));
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());


			Logger::WriteMessage("Select ");
			result1 = mpQuery->Select(L"TABLE3, TABLE4", L"TABLE3.INTVALUE1 = TABLE4.INTVALUE4");
			Logger::WriteMessage((L"... done. Message: " + mpQuery->getMessage()).c_str());
			Assert::IsTrue(result1);

			char message[100];
			sprintf(message, "FLOATVALUE1 = %f", floatValue2);
			Logger::WriteMessage(message);

			sprintf(message, "STRINGVALUE1 = %s", stringValue2);
			Logger::WriteMessage(message);
			Assert::AreEqual("success", stringValue2);
			


			sprintf(message, "STRINGVALUE4 = %s", stringValue4);
			Logger::WriteMessage(message);




		}



	};



}