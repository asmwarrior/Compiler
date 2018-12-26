#include "stdafx.h"
#include "VirtualMachine.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include "OperationCodes.h"

VirtualMachine::VirtualMachine(std::string name){
	std::streampos size;
	char * data;
	std::ifstream bytecodeFile = std::ifstream(name, std::ios::in | std::ios::binary | std::ios::ate);
	if (bytecodeFile.is_open()) {
		size = bytecodeFile.tellg();
		data = new char[size];
		bytecodeFile.seekg(0, std::ios::beg);
		bytecodeFile.read(data, size);
		bytecodeFile.close();
		std::cout << "The file size is " << size << " bytes" << std::endl;
		this->byteProgram = new ByteProgram(data, size);
		this->stack = new VmStack(sizeof(char) * 1024 * 1024 * 2); // ->2MB
	}
	else {
		std::cout << "ERROR: FILE " << name << " DOES NOT EXIST\n";
		system("pause");
		exit(1);
	}
}

VirtualMachine::~VirtualMachine(){
	delete this->stack;
	delete this->byteProgram;
}

void VirtualMachine::output() {
	this->byteProgram->output();
}


void VirtualMachine::callFunction(int position){
	int prevProgramPointer = this->byteProgram->getPosition();
	// Get number of parameters and size in bytes
	this->byteProgram->setPosition(position);
	int numberOfParameters = this->byteProgram->getNextInt();
	int functionSpace = this->byteProgram->getNextInt();
	int stackSize  = 0;
	for (int i = 0; i < numberOfParameters; i++) {
		switch (this->byteProgram->getNextChar()) {
		case OpCode::I_LOAD:
			stackSize += sizeof(int);
			break;
		case OpCode::F_LOAD:
			stackSize += sizeof(float);
			break;
		case OpCode::BY_LOAD:
			stackSize += sizeof(char);
			break;
		case OpCode::BO_LOAD:
			stackSize += sizeof(bool);
			break;
		}
	}
	int localVarSpace = functionSpace - stackSize;
	// Save parameters in temporary variable
	char * temp = new  char[stackSize];
	std::memcpy(temp, this->stack->popBytes(stackSize), stackSize);
	int nPointer = position + numberOfParameters + 2 * sizeof(int);
	this->byteProgram->setPosition(nPointer);
	int prevStackBottom = this->stack->getBottomPointer();
	//int prevStackTop = this->stack->getStackPointer();
	this->stack->pushInt(prevStackBottom);
	this->stack->pushInt(prevProgramPointer);
	this->stack->setBottomPointer(this->stack->getStackPointer());
	int prevPointer = this->stack->getStackPointer();
	this->stack->pushBytes(temp, stackSize);
	int after = this->stack->getStackPointer();
	int newStackPointer = this->stack->getStackPointer() + functionSpace - stackSize;
	this->stack->setStackPointer(newStackPointer);
	
	delete temp;
}

void VirtualMachine::returnFunction(int size){
	char * returnBytes = this->stack->popBytes(size);
	int prevStackPointer = this->stack->getBottomPointer() - 2 * sizeof(int);
	int prevBottomPointer = this->stack->loadInt(-2 * sizeof(int));
	int prevProgramPointer = this->stack->loadInt(-1 * sizeof(int));

	this->stack->setStackPointer(prevStackPointer);
	this->stack->setBottomPointer(prevBottomPointer);
	this->byteProgram->setPosition(prevProgramPointer);

	this->stack->pushBytes(returnBytes, size);
}

void VirtualMachine::executeProgram(){
	std::cout << "------------Starting Program------------" << std::endl;
	
	int mainPosition = this->byteProgram->getNextInt();
	this->byteProgram->setPosition(mainPosition);
	
	int numberOfParameters = this->byteProgram->getNextInt();
	int functionSpace = this->byteProgram->getNextInt();
	this->byteProgram->setPosition(mainPosition + numberOfParameters + 2 * sizeof(int));
	this->stack->setBottomPointer(0);
	this->stack->setStackPointer(functionSpace);
	

	bool notFinished = true;
	while (notFinished) {
		char opcode = this->byteProgram->getNextOpCode();
		switch (opcode) {
		case OpCode::JMP:
			this->byteProgram->setPosition(this->byteProgram->getNextInt());
			break;
		case OpCode::JMP_TRUE:
			if(this->stack->popBool()) this->byteProgram->setPosition(this->byteProgram->getNextInt());
			else this->byteProgram->getNextInt(); // to prevent vm from interpreting jmp position as opcode
			break;
		case OpCode::JMP_FALSE:
			if (!this->stack->popBool()) this->byteProgram->setPosition(this->byteProgram->getNextInt());
			else this->byteProgram->getNextInt(); // to prevent vm from interpreting jmp position as opcode
			break;
		case OpCode::CALL_FUNCTION:
			this->callFunction(this->byteProgram->getNextInt());
			break;
		case OpCode::RETURN:
			break;
		case OpCode::RETURN_32:
			if (this->stack->getBottomPointer() != 0) { // main without recursion
				this->returnFunction(4);
			}
			else {
				notFinished = false;
			}
			break;
		case OpCode::FUNCTION_END:
			std::cout << "Tried to access code after end of current function." << std::endl
				<< "This is usually caused by a missing return statement" << std::endl
				<< "--->Terminating program" << std::endl;
			system("pause");
			exit(1);
			break;
		case OpCode::LOAD_GLOBAL_32:
			break;
		case OpCode::LOAD_CONSTANT_32:
			this->stack->pushBytes(this->byteProgram->getNextBytes(4), 4);
			//this->stack->pushInt(this->byteProgram->getNextInt());
			break;
		case OpCode::LOAD_CONSTANT_8:
			this->stack->pushBytes(this->byteProgram->getNextBytes(1), 1);
			break;
		case OpCode::I_PRINT:
			std::cout << this->stack->popInt() << std::endl;
			break;
		case OpCode::F_PRINT:
			std::cout << this->stack->popFloat() << std::endl;
			break;
		case OpCode::BY_PRINT:
			std::cout << this->stack->popByte() << std::endl;
			break;
		case OpCode::BO_PRINT:
			std::cout << (this->stack->popBool() ? "true" : "false") << std::endl;
			break;
		case OpCode::I_LOAD:
			this->stack->pushInt(this->stack->loadInt(this->byteProgram->getNextInt()));
			break;
		case OpCode::I_STORE:
			this->stack->storeInt(this->stack->popInt(), this->byteProgram->getNextInt());
			break;
		case OpCode::I_ADD:
			this->stack->pushInt(this->stack->popInt() + this->stack->popInt());
			break;
		case OpCode::I_SUB:
			this->stack->pushInt(this->stack->popInt() - this->stack->popInt());
			break;
		case OpCode::I_MUL:
			
			this->stack->pushInt(this->stack->popInt() * this->stack->popInt());
			break;
		case OpCode::I_DIV:
			this->stack->pushInt(this->stack->popInt() / this->stack->popInt());
			break;
		case OpCode::I_MOD:
			this->stack->pushInt(this->stack->popInt() %  this->stack->popInt());
			break;
		case OpCode::I_INC:
			break;
		case OpCode::I_LESS_THAN:
			this->stack->pushBool(this->stack->popInt() < this->stack->popInt());
			break;
		case OpCode::I_LESS_EQUAL:
			this->stack->pushBool(this->stack->popInt() <= this->stack->popInt());
			break;
		case OpCode::I_BIGGER_THAN:
			this->stack->pushBool(this->stack->popInt() > this->stack->popInt());
			break;
		case OpCode::I_Bigger_EQUAL:
			this->stack->pushBool(this->stack->popInt() >= this->stack->popInt());
			break;
		case OpCode::I_EQUAL:
			this->stack->pushBool(this->stack->popInt() == this->stack->popInt());
			break;
		case OpCode::I_NOT_EQUAL:
			this->stack->pushBool(this->stack->popInt() != this->stack->popInt());
			break;
		case OpCode::I_FROM_F:
			this->stack->pushInt(static_cast<int>(this->stack->popFloat()));
			break;
		case OpCode::F_LOAD:
			this->stack->pushFloat(this->stack->loadFloat(this->byteProgram->getNextInt()));
			break;
		case OpCode::F_STORE:
			this->stack->storeFloat(this->stack->popFloat(), this->byteProgram->getNextInt());
			break;
		case OpCode::F_ADD:
			this->stack->pushFloat(this->stack->popFloat() + this->stack->popFloat());
			break;
		case OpCode::F_SUB:
			this->stack->pushFloat(this->stack->popFloat() - this->stack->popFloat());
			break;
		case OpCode::F_MUL:
			this->stack->pushFloat(this->stack->popFloat() * this->stack->popFloat());
			break;
		case OpCode::F_DIV:
			this->stack->pushFloat(this->stack->popFloat() / this->stack->popFloat());
			break;
		case OpCode::F_MOD:
			this->stack->pushFloat(std::fmod(this->stack->popFloat(), this->stack->popFloat()));
			break;
		case OpCode::F_INC:
			break;
		case OpCode::F_LESS_THAN:
			this->stack->pushBool(this->stack->popFloat() < this->stack->popFloat());
			break;
		case OpCode::F_LESS_EQUAL:
			this->stack->pushBool(this->stack->popFloat() <= this->stack->popFloat());
			break;
		case OpCode::F_BIGGER_THAN:
			this->stack->pushBool(this->stack->popFloat() > this->stack->popFloat());
			break;
		case OpCode::F_Bigger_EQUAL:
			this->stack->pushBool(this->stack->popFloat() >= this->stack->popFloat());
			break;
		case OpCode::F_EQUAL:
			this->stack->pushBool(this->stack->popFloat() == this->stack->popFloat());
			break;
		case OpCode::F_NOT_EQUAL:
			this->stack->pushBool(this->stack->popFloat() != this->stack->popFloat());
			break;
		case OpCode::F_FROM_I:
			this->stack->pushFloat(static_cast<float>(this->stack->popInt()));
			break;
		case OpCode::BY_LOAD:
			this->stack->pushByte(this->stack->loadChar(this->byteProgram->getNextInt()));
			break;
		case OpCode::BY_STORE:
			this->stack->storeChar(this->stack->popByte(), this->byteProgram->getNextInt());
			break;
		case OpCode::BY_ADD:
			this->stack->pushByte(this->stack->popByte() + this->stack->popByte());
			break;
		case OpCode::BY_SUB:
			this->stack->pushByte(this->stack->popByte() - this->stack->popByte());
			break;
		case OpCode::BY_MUL:
			this->stack->pushByte(this->stack->popByte() * this->stack->popByte());
			break;
		case OpCode::BY_DIV:
			this->stack->pushByte(this->stack->popByte() / this->stack->popByte());
			break;
		case OpCode::BY_MOD:
			this->stack->pushByte(this->stack->popByte() % this->stack->popByte());
			break;
		case OpCode::BY_INC:
			break;
		case OpCode::BY_LESS_THAN:
			this->stack->pushBool(this->stack->popByte() < this->stack->popByte());
			break;
		case OpCode::BY_LESS_EQUAL:
			this->stack->pushBool(this->stack->popByte() <= this->stack->popByte());
			break;
		case OpCode::BY_BIGGER_THAN:
			this->stack->pushBool(this->stack->popByte() > this->stack->popByte());
			break;
		case OpCode::BY_Bigger_EQUAL:
			this->stack->pushBool(this->stack->popByte() >= this->stack->popByte());
			break;
		case OpCode::BY_EQUAL:
			this->stack->pushBool(this->stack->popByte() == this->stack->popByte());
			break;
		case OpCode::BY_NOT_EQUAL:
			this->stack->pushBool(this->stack->popByte() != this->stack->popByte());
			break;
		case OpCode::BO_LOAD:
			this->stack->pushBool(this->stack->loadBool(this->byteProgram->getNextInt()));
			break;
		case OpCode::BO_STORE:
			this->stack->storeBool(this->stack->popBool(), this->byteProgram->getNextInt());
			break;
		case OpCode::BO_EQUAL:
			this->stack->pushBool(this->stack->popBool() == this->stack->popBool());
			break;
		case OpCode::BO_NOT_EQUAL:
			this->stack->pushBool(this->stack->popBool() != this->stack->popBool());
			break;
		case OpCode::BO_AND:
			this->stack->pushBool(this->stack->popBool() && this->stack->popBool());
			break;
		case OpCode::BO_OR:
			this->stack->pushBool(this->stack->popBool() || this->stack->popBool());
			break;
		case OpCode::BO_XOR:
			this->stack->pushBool(this->stack->popBool() | this->stack->popBool());
			break;
		case OpCode::BO_NEGATE:
			this->stack->pushBool(!this->stack->popBool());
			break;
		case OpCode::BO_FROM_I:
			this->stack->pushBool(!this->stack->popInt());
			break;
		case OpCode::BO_FROM_F:
			this->stack->pushBool(!this->stack->popFloat());
		default:
			std::cout << "ERROR: unknown/not implemented operation code " << (int)opcode << std::endl;
			exit(1);
		}
	}
	std::cout << "-------------Ending Program-------------" << std::endl;
}
