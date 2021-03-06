#pragma once

#include "ExpressionTree.h"
#include "Token.h"

class PreUnaryTree : public ExpressionTree {
public:
	PreUnaryTree(ExpressionTree * right, Token * unaryOperator) {
		this->right = right;
		this->unaryOperator = unaryOperator;
	}

	void output() override {
		right->output();
		std::cout << "PreUnary (Negate)" << std::endl;
	}

	DataType checkDatatype(ScopeHelper * s) override {
		if (right->checkDatatype(s) == Bool) {
			this->type = Bool;
			return Bool;
		}
		return Error;
	}

	void writeCode(CodeGenerator * c) override;

	ExpressionTree * right;
	Token * unaryOperator;
};
