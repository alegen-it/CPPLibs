#pragma once

#include <memory>
#include <string>
#include "Parameter.h"

namespace alegen_it {
	namespace database {
		class Query
		{
		private:
			// pimpl idiom for private members
			class impl;
			std::unique_ptr<impl> pimpl; // opaque type here
			Parameter *mpFirstParameter = nullptr;
			Parameter *mpLastParameter = nullptr;
			// first output parameter of the linked list
			Parameter *mpFirstOutParameter = nullptr;
			// last output parameter of the linked list
			Parameter *mpLastOutParameter = nullptr;


		public:
			Query();
			~Query();

			// //try to connect to database
			bool testConnection();
			void setConnectionParameters(std::wstring serverName, std::wstring databaseName, std::wstring userName, std::wstring password);
			std::wstring getMessage();
			std::wstring ExecDirect(std::wstring sql);

			void addParameter(Parameter *pParameter);
			void addOutParameter(Parameter *pParameter);
			bool Insert(std::wstring TableName);
			bool Select(std::wstring TableName, std::wstring where = L"");
			void ClearParameters();
		};
	}
}

