#include "stdafx.h"
#include "Parameter.h"


alegen_it::database::Parameter::Parameter(float * pVariable, std::wstring columnName)
{
	this->mColumnName = std::wstring(columnName);
	this->mpVariable = pVariable;
	this->mVar_cType = c_type_float;
}


alegen_it::database::Parameter::Parameter(int * pVariable, std::wstring columnName)
{
	this->mColumnName = std::wstring(columnName);
	this->mpVariable = pVariable;
	this->mVar_cType = c_type_int;
}

alegen_it::database::Parameter::Parameter(char * pVariable, int size, std::wstring columnName)
{
	this->mColumnName = std::wstring(columnName);
	this->mpVariable = pVariable;
	this->mVar_cType = c_type_char;
	this->mSize = size;
}


alegen_it::database::Parameter::~Parameter()
{
}



