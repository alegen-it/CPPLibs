#include "stdafx.h"
#include "Query.h"


using namespace std;


/******************************************/
/* Forward references                     */
/******************************************/




wstring errorInfo(SQLSMALLINT handleType, SQLHANDLE handle);
wstring processError(SQLRETURN retcode, SQLHSTMT hstmt);
wstring convert(SQLCHAR * pChar);
void freeResources(SQLHSTMT hstmt, SQLHDBC hDbc);

namespace alegen_it {
	namespace database {

		class Query::impl {
		public:
			SQLHENV mHEnv;
			wstring mServerName;
			wstring mDatabaseName;
			wstring mUserName;
			wstring mPassword;
			wstring mMessage;
			bool CheckRetCode(RETCODE RetCode, SQLHANDLE hHandle, SQLSMALLINT hType);
			void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
			SQLHDBC Connect();
			wstring buildConnectionString();
			SQLSMALLINT getCType(c_type cType);
			SQLSMALLINT getSQLType(c_type cType);
			bool bindParameter(Parameter * parameter, SQLHSTMT hstmt, int iPar, SQLSMALLINT fParamType);
			bool bindColumn(Parameter * parameter, SQLHSTMT hstmt, int iPar);
		};



		Query::Query() : pimpl(new impl)
		{
			// Allocate an environment
			if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &pimpl->mHEnv) == SQL_ERROR)
			{
				pimpl->mMessage = L"Unable to allocate an environment handle";
				exit(-1);
			}

			bool result = pimpl->CheckRetCode(SQLSetEnvAttr(pimpl->mHEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0), pimpl->mHEnv, SQL_HANDLE_ENV);

		}


		Query::~Query()
		{
			if (pimpl->mHEnv)
			{
				SQLFreeHandle(SQL_HANDLE_ENV, pimpl->mHEnv);
			}

			pimpl->mMessage = L"Disconnected";

		}


	}
}


/************************************************/
/*/
/************************************************/
bool alegen_it::database::Query::impl::bindParameter(Parameter * parameter, SQLHSTMT hstmt, int iPar, SQLSMALLINT fParamType)
{
	SQLSMALLINT fCType = getCType(parameter->getType());
	SQLSMALLINT fSQLType = getSQLType(parameter->getType());

	return CheckRetCode(SQLBindParameter(hstmt, iPar, fParamType, fCType, fSQLType, parameter->getSize(), 0, (SQLPOINTER)parameter->getVariable(), parameter->getSize(), NULL), hstmt, SQL_HANDLE_STMT);

}


/************************************************/
/* Bind columns for SELECT queries              */
/************************************************/
bool alegen_it::database::Query::impl::bindColumn(Parameter * parameter, SQLHSTMT hstmt, int iCol)
{
	SQLSMALLINT fCType = getCType(parameter->getType());
	SQLSMALLINT fSQLType = getSQLType(parameter->getType());

	//return CheckRetCode(SQLBindParameter(hstmt, iPar, fParamType, fCType, fSQLType, parameter->getSize(), 0, (SQLPOINTER)parameter->getVariable(), parameter->getSize(), NULL), hstmt, SQL_HANDLE_STMT);
	return CheckRetCode(SQLBindCol(hstmt, iCol, fCType, (SQLPOINTER)parameter->getVariable(), parameter->getBufferSize(), NULL), hstmt, SQL_HANDLE_STMT);

}


SQLSMALLINT alegen_it::database::Query::impl::getCType(c_type cType)
{
	switch (cType) {
	case c_type_float:
		return SQL_C_FLOAT;
	case c_type_int:
		return SQL_C_LONG;
	case c_type_char:
		return SQL_C_CHAR;
	}
	return SQLSMALLINT();
}

SQLSMALLINT alegen_it::database::Query::impl::getSQLType(c_type cType)
{
	switch (cType) {
	case c_type_float:
		return SQL_REAL;
	case c_type_int:
		return SQL_INTEGER;
	case c_type_char:
		return SQL_VARCHAR;
	}
	return SQLSMALLINT();
}


//
//
// example: Driver = { SQL Server }; Server = myServerAddress; Database = myDataBase; Uid = myUsername; Pwd = myPassword;
//
std::wstring alegen_it::database::Query::impl::buildConnectionString()
{
	std::wstring result = L"";

	result = result + L"Driver={SQL Server}; Server=";
	result += mServerName + L"; Database=";
	result += mDatabaseName + L"; Uid=";
	result += mUserName + L"; Pwd=";
	result += mPassword + L";";

	return result;
}


SQLHDBC alegen_it::database::Query::impl::Connect() {
	SQLHDBC     hDbc = NULL;
	bool result = false;

	// Allocate a connection handle
	result = CheckRetCode(SQLAllocHandle(SQL_HANDLE_DBC, mHEnv, &hDbc), mHEnv, SQL_HANDLE_ENV);

	if (!result) return NULL;

	std::wstring str1 = buildConnectionString().c_str();
	std::vector<SQLWCHAR> connectionString(str1.c_str(), str1.c_str() + str1.size() + 1);

	result = CheckRetCode(SQLDriverConnect(hDbc, NULL, connectionString.data(), SQL_NTS, NULL,
		0, NULL, SQL_DRIVER_NOPROMPT), hDbc, SQL_HANDLE_DBC);

	if (result) {
		return hDbc;
	}
	else {
		SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
		return false;
	}

}

// //try to connect to database
bool alegen_it::database::Query::testConnection()
{
	SQLHDBC     hDbc = pimpl->Connect();


	// Free ODBC handles and return
	if (hDbc) {
		SQLDisconnect(hDbc);
		SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
		return true;
	}
	else {
		return false;
	}

}


void alegen_it::database::Query::setConnectionParameters(std::wstring serverName, std::wstring databaseName, std::wstring userName, std::wstring password)
{
	pimpl->mDatabaseName = databaseName;
	pimpl->mServerName = serverName;
	pimpl->mUserName = userName;
	pimpl->mPassword = password;

}

std::wstring alegen_it::database::Query::getMessage()
{
	return pimpl->mMessage;
}


/************************************************************************
/* HandleDiagnosticRecord : display error/warning information
/*
/* Parameters:
/*      hHandle     ODBC handle
/*      hType       Type of handle (HANDLE_STMT, HANDLE_ENV, HANDLE_DBC)
/*      RetCode     Return code of failing command
/************************************************************************/

void alegen_it::database::Query::impl::HandleDiagnosticRecord(SQLHANDLE hHandle,
	SQLSMALLINT    hType,
	RETCODE        RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER  iError;
	WCHAR       wszMessage[1000];
	WCHAR       wszState[SQL_SQLSTATE_SIZE + 1];


	if (RetCode == SQL_INVALID_HANDLE)
	{
		mMessage = L"Invalid handle";
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
			mMessage = std::wstring(wszMessage);
		}
	}
}


bool alegen_it::database::Query::impl::CheckRetCode(RETCODE RetCode, SQLHANDLE hHandle, SQLSMALLINT hType) {
	if (RetCode != SQL_SUCCESS) {
		HandleDiagnosticRecord(hHandle, hType, RetCode);
	}
	else {
		mMessage = L"SQL_SUCCESS!";
	}
	if (RetCode == SQL_ERROR || RetCode == SQL_NO_DATA_FOUND ) {
		return false;
	}
	else {
		return true;
	}

}


wstring errorInfo(SQLSMALLINT handleType, SQLHANDLE handle)
{
	SQLSTATE state;
	SQLINTEGER nativeError;
	SQLWCHAR messageText[100];
	SQLSMALLINT messageLen;

	SQLGetDiagRec(handleType, handle, 1, state, &nativeError, messageText, 100, &messageLen);

	wstring retval = L"code: ";
	retval = retval + wstring(state);
	retval = retval + L" - '";
	retval = retval + wstring(messageText);
	retval = retval + L"'";

	return retval;
}


wstring processError(SQLRETURN retcode, SQLHANDLE hstmt)
{
	switch (retcode) {
	case SQL_SUCCESS:
		return L"Success!" ;
	case SQL_SUCCESS_WITH_INFO:
		return L"Success with info - " + errorInfo(SQL_HANDLE_STMT, hstmt) ;
	case SQL_NEED_DATA:
		return L"Need data";
	case SQL_STILL_EXECUTING:
		return L"Still executing";
	case SQL_ERROR:
		return L"Error: " + errorInfo(SQL_HANDLE_STMT, hstmt);
	case SQL_NO_DATA:
		return L"No data" ;
		break;
	case SQL_INVALID_HANDLE:
		return L"Invalid handle";
		break;
	case SQL_PARAM_DATA_AVAILABLE:
		return L"Param data available";
		break;
	default:
		return (std::wstring(L"Error code " + retcode) + L" unknown");
		break;
	}

}

std::wstring alegen_it::database::Query::ExecDirect(std::wstring sql)
{
	wstring retVal;
	SQLHDBC     hDbc = pimpl->Connect();
	if (!hDbc) {
		return L"No connection to database";
	}

	SQLHSTMT hstmt = NULL;
	bool result = pimpl->CheckRetCode(SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hstmt), hDbc, SQL_HANDLE_DBC);

	if (!result) {
		return L"Unable to allocate statement handle";
	}

	result = pimpl->CheckRetCode(SQLExecDirect(hstmt, (SQLWCHAR*)sql.c_str(), SQL_NTS), hstmt, SQL_HANDLE_STMT);
	if (result) {
		SQLWCHAR sqlResult[1000];
		SQLINTEGER ptrSqlResult;
		while (SQLFetch(hstmt) == SQL_SUCCESS) {
			SQLGetData(hstmt, 1, SQL_WCHAR, sqlResult, 1000, &ptrSqlResult);
			//display query result
			retVal = sqlResult;
		}
	}
	else {
		retVal = L"Query failed";
		}

	// Free ODBC handles and return
	SQLFreeStmt(hstmt, SQL_CLOSE);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hDbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	return retVal;

}

void alegen_it::database::Query::addParameter(Parameter *pParameter)
{
	if (mpFirstParameter == NULL) {
		mpFirstParameter = pParameter;
		mpLastParameter = pParameter;
		mpFirstParameter->setNext(nullptr);
	}
	else {
		mpLastParameter->setNext(pParameter);
		mpLastParameter = pParameter;
	}

}

/******************************************************/
/* Add a parameter to bind a column in a select query */
/******************************************************/
void alegen_it::database::Query::addOutParameter(Parameter *pParameter)
{
	if (mpFirstOutParameter == NULL) {
		mpFirstOutParameter = pParameter;
		mpLastOutParameter = pParameter;
		mpFirstOutParameter->setNext(nullptr);
	}
	else {
		mpLastOutParameter->setNext(pParameter);
		mpLastOutParameter = pParameter;
	}

}




wstring convert(SQLCHAR *pchar) {
	wstring result;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	result = converter.from_bytes(*pchar);
	return result;

}


bool alegen_it::database::Query::Insert(std::wstring TableName)
{
	//get statement
	wstring retVal;
	SQLHDBC     hDbc = pimpl->Connect();
	if (!hDbc) {
		return L"No connection to database";
	}

	SQLHSTMT hstmt = NULL;
	bool result = pimpl->CheckRetCode(SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hstmt), hDbc, SQL_HANDLE_DBC);

	if (!result) {
		return L"Unable to allocate statement handle";
	}

	// bind parameters
	Parameter *parameter = mpFirstParameter;
	wstring sql1 = L"INSERT INTO " + TableName + L"(";
	wstring sql2 = L" VALUES (";
	int iPar = 1;
	while (parameter != NULL) {
		sql1 = sql1 + parameter->getColumnName();
		sql2 = sql2 + L"?";

		result = pimpl->bindParameter(parameter, hstmt, iPar++, SQL_PARAM_INPUT);
		if (!result) {
			freeResources(hstmt, hDbc);
			return false;
		}

		parameter = parameter->getNext();
		if (parameter != NULL) {
			sql1 = sql1 + L",";
			sql2 = sql2 + L",";
		}
		else {
			sql1 = sql1 + L") ";
			sql2 = sql2 + L") ";
		}
	}

	// execdirect
	wstring sql = sql1 + sql2;
	result = pimpl->CheckRetCode(SQLExecDirect(hstmt, (SQLWCHAR*)sql.c_str(), SQL_NTS), hstmt, SQL_HANDLE_STMT);

	// free resources
	freeResources(hstmt, hDbc);
	return result;
}


/**********************************************************************/
/* The TableName must contain all selected tables separated by commas */
/* the where clause must contain the join conditions without WHERE keyword*/
/* example TableName = L"Table1, Table2"
/* where = L"Table1.table2ID = Table2.ID"                               */
/**********************************************************************/
bool alegen_it::database::Query::Select(std::wstring TableName, std::wstring where)
{
	//get statement
	wstring retVal;
	SQLHDBC     hDbc = pimpl->Connect();
	if (!hDbc) {
		return L"No connection to database";
	}

	SQLHSTMT hstmt = NULL;
	bool result = pimpl->CheckRetCode(SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hstmt), hDbc, SQL_HANDLE_DBC);

	if (!result) {
		return L"Unable to allocate statement handle";
	}

	// bind parameters
	Parameter *parameter = mpFirstParameter;
	Parameter *outParameter = mpFirstOutParameter;
	wstring sql1 = L"SELECT  ";
	wstring sql2 = L" FROM " + TableName;

	if (where != L"" || mpFirstParameter != NULL) {
		if (where != L"") {
			sql2 = sql2 + L" WHERE (" + where + L") ";
			if (mpFirstParameter) {
				sql2 = sql2 + L"AND (";
			}
		} else {
			sql2 = sql2 + L" WHERE (";
		}
	}


	int iPar = 1;
	while (parameter != NULL || outParameter !=NULL) {

		if (outParameter != NULL) {
			sql1 = sql1 + outParameter->getColumnName();
			result = pimpl->bindColumn(outParameter, hstmt, iPar);
			if (!result) {
				freeResources(hstmt, hDbc);
				return false;
			}
			outParameter = outParameter->getNext();
			if (outParameter != NULL) {
				sql1 = sql1 + L",";
			}
		}

		if (parameter != NULL) {
			if (parameter != NULL) {
				sql2 = sql2 + parameter->getColumnName() + L" = ?)";
				result = pimpl->bindParameter(parameter, hstmt, iPar, SQL_PARAM_INPUT);
				if (!result) {
					freeResources(hstmt, hDbc);
					return false;
				}
				parameter = parameter->getNext();
				if (parameter != NULL) {
					sql2 = sql2 + L" AND (";
				}
			}
		}

		iPar++;
	}

	// execdirect
	wstring sql = sql1 + sql2;
	result = pimpl->CheckRetCode(SQLExecDirect(hstmt, (SQLWCHAR*)sql.c_str(), SQL_NTS), hstmt, SQL_HANDLE_STMT);

	if (result) {
		result = pimpl->CheckRetCode(SQLFetch(hstmt), hstmt, SQL_HANDLE_STMT);
	}

	// free resources
	freeResources(hstmt, hDbc);
	return result;
}



void freeResources(SQLHSTMT hstmt, SQLHDBC hDbc) {
	// Free ODBC handles and return
	if (hstmt) {
		SQLFreeStmt(hstmt, SQL_CLOSE);
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	}
	if (hDbc) {
		SQLDisconnect(hDbc);
		SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	}

}


void alegen_it::database::Query::ClearParameters()
{
	while (mpFirstParameter != NULL) {
		Parameter *parameter = mpFirstParameter;
		mpFirstParameter = parameter->getNext();
		parameter->~Parameter();
	}

	while (mpFirstOutParameter != NULL) {
		Parameter *parameter = mpFirstOutParameter;
		mpFirstOutParameter = parameter->getNext();
		parameter->~Parameter();
	}

}
