// Compiler.cpp: Definiert den Einstiegspunkt für die Konsolenanwendung.
//
#include "stdafx.h"
#include <iostream>
#include "SourceFile.h"

int main(int numberArgs, const char ** arguments)
{
	std::cout << "Starting Compiler" << std::endl;
	SourceFile f = SourceFile("main.z");
	system("pause");
	return 0;
}

