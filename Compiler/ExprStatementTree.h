#pragma once

#include "StatementTree.h"
#include "ExpressionTree.h"

class ExprStatementTree : public StatementTree {
public:
	ExprStatementTree(ExpressionTree * expression) {
		this->expression = expression;
	}
	ExpressionTree * expression;

	bool checkForErrors(ScopeHelper * s) override {
		if (this->expression->checkDatatype() == Error) {
			this->error("Error in expression statement");
		}
		else {
			return true;
		}
	}
};