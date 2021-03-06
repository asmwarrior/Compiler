#pragma once

#include "Token.h"

enum DataType {
	Byte,
	Short,
	Integer,
	Float,
	Bool,
	Array,
	Custom,
	Error,
	Void,
	String
};

DataType getType(Token * token);
DataType getTypeForLiteral(Token * token);
int getTypeSize(DataType d);