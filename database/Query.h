#pragma once

#include <memory>
#include <string>

namespace alegen_it {
	namespace database {
		class Query
		{
		private:
			// pimpl idiom for private members
			class impl;
			std::unique_ptr<impl> pimpl; // opaque type here
			

		public:
			Query();
			~Query();

			// //try to connect to database
			bool testConnection();
			void setConnectionParameters(std::wstring serverName, std::wstring databaseName, std::wstring userName, std::wstring password);
			std::wstring getMessage();
			std::wstring ExecDirect(std::wstring sql);
		};
	}
}

