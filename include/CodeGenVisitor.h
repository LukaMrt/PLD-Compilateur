#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "SymbolTableVisitor.h"

#include <map>
#include <string>

class CodeGenVisitor : public ifccBaseVisitor {
    public:
        CodeGenVisitor(const std::map<std::string, SymbolTableVisitor::VariableInfo> &symbolTable)
            : symbolTable(symbolTable) {}

        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
        virtual antlrcpp::Any visitReturn_statement(ifccParser::Return_statementContext *ctx) override;
        virtual antlrcpp::Any visitVariable_creation(ifccParser::Variable_creationContext *ctx) override;
        virtual antlrcpp::Any visitVariable_assignment(ifccParser::Variable_assignmentContext *ctx) override;

    private:
        std::map<std::string, SymbolTableVisitor::VariableInfo> symbolTable;
};

