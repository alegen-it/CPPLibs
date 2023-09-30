#include "stdafx.h"
#include "Query.h"


using namespace std;

namespace alegen_it {
	namespace database {

		class Query::impl {
		public:
			SQLHENV mHEnv;
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