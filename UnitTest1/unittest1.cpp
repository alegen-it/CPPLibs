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

	};
}