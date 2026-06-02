#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

#include <map>
#include <string>

class SymbolTableVisitor : public ifccBaseVisitor
{
public:
        struct VariableInfo
        {
                int offset;
                bool used;
        };

        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
        virtual antlrcpp::Any visitVariable_assignment(ifccParser::Variable_assignmentContext *ctx) override;
        virtual antlrcpp::Any visitVariable_creation(ifccParser::Variable_creationContext *ctx) override;
        virtual antlrcpp::Any visitVariable_expression(ifccParser::Variable_expressionContext *ctx) override;

        std::map<std::string, VariableInfo> getSymbolTable() const { return symbolTable; }

private:
        std::map<std::string, VariableInfo> symbolTable;
        int nextOffset = -4;
        bool isDeclared(const std::string &varName) { return symbolTable.find(varName) != symbolTable.end(); }
};
