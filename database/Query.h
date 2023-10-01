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
			std::wstring buildConnectionString();

		public:
			Query();
			~Query();

			// //try to connect to database
			bool testConnection();
			void setConnectionParameters(std::wstring serverName, std::wstring databaseName, std::wstring userName, std::wstring password);
		};
	}
}

