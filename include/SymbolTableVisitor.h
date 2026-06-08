#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "struct/Type.h"

#include <map>
#include <string>
#include <vector>

class SymbolTableVisitor : public ifccBaseVisitor
{
public:
        struct VariableInfo
        {
                bool used;
                Type type;
        };

        struct FunctionInfo
        {
                Type returnType;
                std::vector<Type> parameterTypes;
        };

        virtual antlrcpp::Any visitFunction(ifccParser::FunctionContext *ctx) override;
        virtual antlrcpp::Any visitFunction_parameter_declaration(ifccParser::Function_parameter_declarationContext *ctx) override;
        virtual antlrcpp::Any visitVariable_definition_with_instruction(ifccParser::Variable_definition_with_instructionContext *ctx) override;
        virtual antlrcpp::Any visitVariable_definition_without_instruction(ifccParser::Variable_definition_without_instructionContext *ctx) override;
        virtual antlrcpp::Any visitInstruction(ifccParser::InstructionContext *ctx) override;
        virtual antlrcpp::Any visitVariable_expression(ifccParser::Variable_expressionContext *ctx) override;
        virtual antlrcpp::Any visitFunction_call(ifccParser::Function_callContext *ctx) override;
        std::map<std::string, VariableInfo> getSymbolTable(const std::string &funcName) const { return allSymbolTables.at(funcName); }
        std::map<std::string, std::map<std::string, VariableInfo>> getAllSymbolTables() const { return allSymbolTables; }
        std::map<std::string, FunctionInfo> getFunctionTable() const { return functionTable; }

private:
        std::map<std::string, std::map<std::string, VariableInfo>> allSymbolTables;
        std::map<std::string, FunctionInfo> functionTable;
        std::string currentFunction;

        std::map<std::string, VariableInfo> &currentTable() { return allSymbolTables[currentFunction]; }

        bool isDeclared(const std::string &varName)
        {
                return currentTable().find(varName) != currentTable().end();
        }

        void declareVariable(const std::string &varName, Type type)
        {
                if (type == Type::VOID)
                {
                        std::cerr << "Error: variable '" << varName << "' declared void." << std::endl;
                        exit(1);
                }
                if (isDeclared(varName))
                {
                        std::cerr << "Error: variable '" << varName << "' is already declared." << std::endl;
                        exit(1);
                }
                currentTable()[varName] = {false, type};
        }

        void checkDeclared(const std::string &varName)
        {
                if (!isDeclared(varName))
                {
                        std::cerr << "Error: variable '" << varName << "' is not declared." << std::endl;
                        exit(1);
                }
        }

        void useVariable(const std::string &varName)
        {
                checkDeclared(varName);
                currentTable()[varName].used = true;
        }
};
