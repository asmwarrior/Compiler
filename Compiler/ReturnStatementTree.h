#pragma once

#include "StatementTree.h"
#include "Expressions.h"

class ReturnStatementTree : public StatementTree {
public:
	ReturnStatementTree(ExpressionTree * expr) {
		this->expr = expr;
	}
	ExpressionTree * expr;

	bool checkForErrors(ScopeHelper * s) override {
		DataType returnType = this->expr->checkDatatype();
		if (returnType == Error) {
			
			this->error("Error in return statement");
		}
		else {
			return true;
		}
	}
};
