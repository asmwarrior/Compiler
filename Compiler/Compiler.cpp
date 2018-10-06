// Compiler.cpp: Definiert den Einstiegspunkt für die Konsolenanwendung.
//
#include "stdafx.h"
#include <iostream>
#include "Lexer.h"

int main(int numberArgs, const char ** arguments)
{
	std::cout << "Starting Compiler" << std::endl;
	Lexer lexer = Lexer("main.z");
	lexer.getTokenList();
	system("pause");
	return 0;
}

