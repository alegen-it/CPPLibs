#pragma once
#include <string>

namespace alegen_it {
	namespace database {
		enum c_type {
			c_type_float,
			c_type_char,
			c_type_int
		};


		class Parameter
		{
		public:
			Parameter(float *pVariable, std::wstring columnName);
			Parameter(int *pVariable, std::wstring columnName);
			Parameter(char *pVariable, int size, std::wstring columnName);
			~Parameter();
		private:
			void *mpVariable;
			c_type mVar_cType;
			std::wstring mColumnName;
			Parameter *mNext = nullptr;
			int mSize;

		public:

			void setNext(Parameter * next)
			{
				mNext = next;
			}
			Parameter * getNext()
			{
				return mNext;
			}
			std::wstring getColumnName() {
				return mColumnName;
			}
			void* getVariable() {
				return mpVariable;
			}

			c_type getType() {
				return mVar_cType;
			}
			int getSize() {
				return mSize;
			}

		};
	}
}


