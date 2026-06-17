/*#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "utils/Type.h"
#include "utils/Variable.h"

#include <map>
#include <string>
#include <vector>

class SymbolTableVisitor : public ifccBaseVisitor
{
public:
        struct FunctionInfo
        {
                Type returnType;
                std::vector<Type> parameterTypes;
        };

        virtual antlrcpp::Any visitFunction(ifccParser::FunctionContext *ctx) override;
        virtual antlrcpp::Any visitFunction_parameter_declaration(ifccParser::Function_parameter_declarationContext *ctx) override;
        virtual antlrcpp::Any visitVariable_definition_with_instruction(ifccParser::Variable_definition_with_instructionContext *ctx) override;
        virtual antlrcpp::Any visitVariable_definition_without_instruction(ifccParser::Variable_definition_without_instructionContext *ctx) override;
        virtual antlrcpp::Any visitLeft_value(ifccParser::Left_valueContext *ctx) override;
        virtual antlrcpp::Any visitVariable_expression(ifccParser::Variable_expressionContext *ctx) override;
        virtual antlrcpp::Any visitFunction_call(ifccParser::Function_callContext *ctx) override;
        std::vector<std::map<std::string, Variable>> getSymbolTable(const std::string &funcName) const { return allSymbolTables.at(funcName); }
        std::map<std::string, std::vector<std::map<std::string, Variable>>> getAllSymbolTables() const { return allSymbolTables; }
        std::map<std::string, FunctionInfo> getFunctionTable() const { return functionTable; }
        std::vector<std::pair<std::string, std::string>> getUnusedVariables() const { return unusedVariables; }

private:
        std::map<std::string, std::vector<std::map<std::string, Variable>>> allSymbolTables;
        std::map<std::string, FunctionInfo> functionTable;
        std::vector<std::pair<std::string, std::string>> unusedVariables; // (function_name, variable_name)
        std::string currentFunction;

        // Scope management helpers
        std::map<std::string, Variable> &currentScope() { return allSymbolTables[currentFunction].back(); }

        Variable *findVariable(const std::string &varName)
        {
                // Search from the innermost scope (end) to the outermost scope (beginning)
                auto &scopes = allSymbolTables[currentFunction];
                for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
                {
                        auto found = it->find(varName);
                        if (found != it->end())
                        {
                                return &found->second;
                        }
                }
                return nullptr;
        }

        void pushScope()
        {
                allSymbolTables[currentFunction].push_back(std::map<std::string, Variable>());
        }

        void popScope()
        {
                // Check for unused variables in the current scope before popping
                for (auto &entry : currentScope())
                {
                        if (!entry.second.used)
                        {
                                unusedVariables.push_back({currentFunction, entry.first});
                        }
                }
                allSymbolTables[currentFunction].pop_back();
        }

        bool isDeclaredInCurrentScope(const std::string &varName)
        {
                return currentScope().find(varName) != currentScope().end();
        }

        bool isDeclared(const std::string &varName)
        {
                return findVariable(varName) != nullptr;
        }

        void declareVariable(const std::string &varName, Type type, int pointerDepth = 0)
        {
                if (type == Type::VOID)
                {
                        std::cerr << "Error: variable '" << varName << "' declared void." << std::endl;
                        exit(1);
                }
                if (isDeclaredInCurrentScope(varName))
                {
                        std::cerr << "Error: variable '" << varName << "' is already declared." << std::endl;
                        exit(1);
                }
                currentScope()[varName] = Variable(type, pointerDepth);
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
                Variable *var = findVariable(varName);
                if (var != nullptr)
                {
                        var->used = true;
                }
        }
};
*/