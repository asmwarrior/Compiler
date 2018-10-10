// Compiler.cpp: Definiert den Einstiegspunkt für die Konsolenanwendung.
//
#include "stdafx.h"
#include <iostream>
#include "Lexer.h"
#include "IdentifierTree.h"


int main(int numberArgs, const char ** arguments){

	
	std::cout << "Starting Compiler" << std::endl;
	Lexer lexer = Lexer("main.z");
	std::string source = "";
	for (std::string &s : lexer.source->textList) source += s + "\n";
	std::string line = source;
	for (Token & t : lexer.getLineToken(line)) {
		std::cout << t.getDescription() << std::endl;
	}
	/*
	int lineNumber = 0;
	for (; lineNumber < line.length(); lineNumber++) {
		if (line.at(lineNumber) == ' ') continue;
		Token test = lexer.findInteger(line, lineNumber, lineNumber);
		std::cout << test.getDescription() << std::endl;
	}
	*/
	system("pause"); // FOR TESTING
	return 1; // FOR TESTING
	lexer.getTokenList();
	system("pause");
	return 0;
}

