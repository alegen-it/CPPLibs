#include "stdafx.h"
#include "Query.h"


using namespace std;


/******************************************/
/* Forward references                     */
/******************************************/
void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);

bool CheckRetCode(RETCODE RetCode, SQLHANDLE hHandle, SQLSMALLINT hType);

namespace alegen_it {
	namespace database {

		class Query::impl {
		public:
			SQLHENV mHEnv;
			wstring mServerName;
			wstring mDatabaseName;
			wstring mUserName;
			wstring mPassword;
		};



		Query::Query(): pimpl(new impl)
		{
			cout << "started";

			// Allocate an environment


			if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &pimpl->mHEnv) == SQL_ERROR)
			{
				fwprintf(stderr, L"Unable to allocate an environment handle\n");
				exit(-1);
			}

			bool result = CheckRetCode(SQLSetEnvAttr(pimpl->mHEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0), pimpl->mHEnv, SQL_HANDLE_ENV);

		}


		Query::~Query()
		{
			if (pimpl->mHEnv)
			{
				SQLFreeHandle(SQL_HANDLE_ENV, pimpl->mHEnv);
			}

			wprintf(L"\nDisconnected.");

		}


	}
}

//
//
// example: Driver = { SQL Server }; Server = myServerAddress; Database = myDataBase; Uid = myUsername; Pwd = myPassword;
//
std::wstring alegen_it::database::Query::buildConnectionString()
{
	std::wstring result = L"";

	result = result + L"Driver={SQL Server}; Server=";
	result += pimpl->mServerName + L"; Database=";
	result += pimpl->mDatabaseName + L"; Uid=";
	result += pimpl->mUserName + L"; Pwd=";
	result += pimpl->mPassword + L";";

	return result;
}


// //try to connect to database
bool alegen_it::database::Query::testConnection()
{
	SQLHDBC     hDbc = NULL;
	bool result = false;

	
	// Allocate a connection handle
	result = CheckRetCode(SQLAllocHandle(SQL_HANDLE_DBC, pimpl->mHEnv, &hDbc), pimpl->mHEnv, SQL_HANDLE_ENV);

	if(!result) return result;

	std::wstring str1 = buildConnectionString().c_str();
	std::vector<SQLWCHAR> connectionString(str1.c_str(), str1.c_str() + str1.size() + 1);

	result = CheckRetCode(SQLDriverConnect(hDbc, NULL, connectionString.data(), SQL_NTS, NULL,
		0, NULL, SQL_DRIVER_NOPROMPT), hDbc, SQL_HANDLE_DBC);

	// Free ODBC handles and return
	if (hDbc) {
		SQLDisconnect(hDbc);
		SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	}

	return result;
}


void alegen_it::database::Query::setConnectionParameters(std::wstring serverName, std::wstring databaseName, std::wstring userName, std::wstring password)
{
	pimpl->mDatabaseName = databaseName;
	pimpl->mServerName = serverName;
	pimpl->mUserName = userName;
	pimpl->mPassword = password;

}


/************************************************************************
/* HandleDiagnosticRecord : display error/warning information
/*
/* Parameters:
/*      hHandle     ODBC handle
/*      hType       Type of handle (HANDLE_STMT, HANDLE_ENV, HANDLE_DBC)
/*      RetCode     Return code of failing command
/************************************************************************/

void HandleDiagnosticRecord(SQLHANDLE      hHandle,
	SQLSMALLINT    hType,
	RETCODE        RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER  iError;
	WCHAR       wszMessage[1000];
	WCHAR       wszState[SQL_SQLSTATE_SIZE + 1];


	if (RetCode == SQL_INVALID_HANDLE)
	{
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}

	while (SQLGetDiagRec(hType,
		hHandle,
		++iRec,
		wszState,
		&iError,
		wszMessage,
		(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)),
		(SQLSMALLINT *)NULL) == SQL_SUCCESS)
	{
		// Hide data truncated..
		if (wcsncmp(wszState, L"01004", 5))
		{
			fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
		}
	}
}


bool CheckRetCode(RETCODE RetCode, SQLHANDLE hHandle, SQLSMALLINT hType) {
	if (RetCode != SQL_SUCCESS) {
		HandleDiagnosticRecord(hHandle, hType, RetCode);
	}
	if (RetCode == SQL_ERROR) {
		return false;
	}
	else {
		return true;
	}

}