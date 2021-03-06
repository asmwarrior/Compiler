#include "stdafx.h"
#include "CodeGenerator.h"
#include "OperationCodes.h"

CodeGenerator::CodeGenerator(std::string filename, ProgramTree * program, ScopeHelper * s){
	this->byteFile = new std::ofstream();
	this->byteFile->open(filename + "c", std::ios::binary | std::ios::trunc | std::ios::out);
	this->program = program;
	this->scopeHelper = s;
}

CodeGenerator::~CodeGenerator(){
	this->byteFile->close();
	delete this->byteFile;
}

void CodeGenerator::writeProgram(ProgramTree * p){
	this->program->writeCode(this);
	// Add missing function positions, as all functions are now known
	for (std::pair<std::string, int>& call : this->unfinishedCalls) {
		int functionPosition = this->functionPositions.at(call.first);
		this->writePrevInteger(functionPosition, call.second);
	}
}

void CodeGenerator::writeExpression(ExpressionTree * e){
	
}

void CodeGenerator::writeInteger(int value){
	this->byteFile->write(reinterpret_cast<const char *>(&value), 4);
}

void CodeGenerator::writeByte(unsigned char value){
	this->byteFile->write(reinterpret_cast<const char *>(&value), 1);
}

void CodeGenerator::writeFloat(float value){
	this->byteFile->write(reinterpret_cast<const char *>(&value), 4);
}

void CodeGenerator::writeLong(long value) {
	this->byteFile->write(reinterpret_cast<const char *>(&value), 8);
}

void CodeGenerator::writeBool(bool value){
	this->byteFile->write(reinterpret_cast<const char *>(&value), sizeof(bool));
}

void CodeGenerator::writeString(std::string value){
	this->byteFile->write(value.c_str(), value.size() + 1);
}

void CodeGenerator::writePrevInteger(int value, long position){
	long currentPos = this->byteFile->tellp();
	this->byteFile->seekp(position);
	this->byteFile->write(reinterpret_cast<const char *>(&value), 4);
	this->byteFile->seekp(currentPos);
}

void CodeGenerator::writePrevByte(char value, long position){
	long currentPos = this->byteFile->tellp();
	this->byteFile->seekp(position);
	this->byteFile->write(reinterpret_cast<const char *>(&value), 1);
	this->byteFile->seekp(currentPos);
}

void CodeGenerator::writeprevFloat(float value, long position){
	long currentPos = this->byteFile->tellp();
	this->byteFile->seekp(position);
	this->byteFile->write(reinterpret_cast<const char *>(&value), 4);
	this->byteFile->seekp(currentPos);
}

void CodeGenerator::writePrevLong(long value, long position){
	long currentPos = this->byteFile->tellp();
	this->byteFile->seekp(position);
	this->byteFile->write(reinterpret_cast<const char *>(&value), 8);
	this->byteFile->seekp(currentPos);
}

void CodeGenerator::writePrevBool(bool value, long position){
	long currentPos = this->byteFile->tellp();
	this->byteFile->seekp(position);
	this->byteFile->write(reinterpret_cast<const char *>(&value), sizeof(bool));
	this->byteFile->seekp(currentPos);
}

int CodeGenerator::getCurrentPosition(){
	return this->byteFile->tellp();
}

void CodeGenerator::addUnfinishedFunctionCall(std::string name){
	int currentPos = this->byteFile->tellp();
	this->unfinishedCalls.emplace_back(name, currentPos);
}

void CodeGenerator::saveFunctionStart(){
	long position = this->byteFile->tellp();
	this->functionPositions.emplace(this->currentFunction, position);
}

std::pair<bool, int> CodeGenerator::getFunctionPosition(std::string functionName){
	if (this->functionPositions.find(functionName) == this->functionPositions.end()) {
		return std::pair<bool, long>(false, 0);
	}
	long position = functionPositions.at(functionName);
	return std::pair<bool, long>(true, position);
}
