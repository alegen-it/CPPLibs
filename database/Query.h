#pragma once

#include <memory>

namespace alegen_it {
	namespace database {
		class Query
		{
		public:
			Query();
			~Query();
		private:
			// pimpl idiom for private members
			class impl; 
			std::unique_ptr<impl> pimpl; // opaque type here
		};
	}
}

