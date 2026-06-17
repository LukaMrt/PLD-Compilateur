#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "SymbolTableVisitor.h"
#include "ControlFlowGraph.h"
#include "utils/Variable.h"
#include "utils/Type.h"

#include <map>
#include <string>
#include <vector>

class IRGeneratorVisitor : public ifccBaseVisitor
{
public:
    IRGeneratorVisitor()
        : currentCFG(nullptr) {}

    ControlFlowGraph *getCurrentCFG() const { return currentCFG; }

    virtual antlrcpp::Any visitFunction(ifccParser::FunctionContext *ctx) override;
    virtual antlrcpp::Any visitFunction_parameter_declaration(ifccParser::Function_parameter_declarationContext *ctx) override;
    virtual antlrcpp::Any visitReturn_statement(ifccParser::Return_statementContext *ctx) override;
    virtual antlrcpp::Any visitVariable_definition_with_instruction(ifccParser::Variable_definition_with_instructionContext *ctx) override;
    virtual antlrcpp::Any visitVariable_definition_without_instruction(ifccParser::Variable_definition_without_instructionContext *ctx) override;
    virtual antlrcpp::Any visitAssignment(ifccParser::AssignmentContext *ctx) override;
    virtual antlrcpp::Any visitConstant_expression(ifccParser::Constant_expressionContext *ctx) override;
    virtual antlrcpp::Any visitCharacter_expression(ifccParser::Character_expressionContext *ctx) override;
    virtual antlrcpp::Any visitVariable_expression(ifccParser::Variable_expressionContext *ctx) override;
    virtual antlrcpp::Any visitFunction_call(ifccParser::Function_callContext *ctx) override;
    virtual antlrcpp::Any visitUnary_operation(ifccParser::Unary_operationContext *ctx) override;
    virtual antlrcpp::Any visitBracketed_expression(ifccParser::Bracketed_expressionContext *ctx) override;
    virtual antlrcpp::Any visitAdditive_expression(ifccParser::Additive_expressionContext *ctx) override;
    virtual antlrcpp::Any visitMultiplicative_expression(ifccParser::Multiplicative_expressionContext *ctx) override;
    virtual antlrcpp::Any visitBitwise_and_expression(ifccParser::Bitwise_and_expressionContext *ctx) override;
    virtual antlrcpp::Any visitBitwise_or_expression(ifccParser::Bitwise_or_expressionContext *ctx) override;
    virtual antlrcpp::Any visitBitwise_xor_expression(ifccParser::Bitwise_xor_expressionContext *ctx) override;
    virtual antlrcpp::Any visitStatement(ifccParser::StatementContext *ctx) override;
    virtual antlrcpp::Any visitEqual_expression(ifccParser::Equal_expressionContext *ctx) override;
    virtual antlrcpp::Any visitComparison_expression(ifccParser::Comparison_expressionContext *ctx) override;
    virtual antlrcpp::Any visitLeft_value(ifccParser::Left_valueContext *ctx) override;
    virtual antlrcpp::Any visitBlock(ifccParser::BlockContext *ctx) override;

private:
    // Optimisation par pliage de constantes : associe un nom de variable temporaire
    // à sa valeur connue à la compilation. Ne contient que des temps non mutables.
    std::map<std::string, int> knownConstants;

    // Renvoie true si v est une constante connue ; place alors sa valeur dans out.
    bool isConstant(const std::string &v, int &out) const;
    // Émet un LoadConstant dans un nouveau temp, l'enregistre comme constant et renvoie son nom.
    std::string emitConstant(Type type, int value);

    // Calcule l'adresse cible d'un lvalue : n-1 DereferenceRead pour n étoiles,
    // et renvoie le nom du temporaire (ou de la variable) contenant l'adresse.
    std::string evalAddress(ifccParser::Left_valueContext *lv);

    //std::map<std::string, std::map<std::string, Variable>> allSymbolTables;
    //std::map<std::string, Variable> symbolTable;
    std::map<std::string, ControlFlowGraph *> cfgs;
    // Type de retour de chaque fonction, renseigné lors de la visite de sa définition.
    std::map<std::string, Type> functionReturnTypes;
    ControlFlowGraph *currentCFG;
    int deadBlockCount = 0;

public:
    const std::map<std::string, ControlFlowGraph *> &getCFGs() const { return cfgs; }
    int getDeadBlockCount() const { return deadBlockCount; }

    // Symbol table management

    struct FunctionInfo
    {
        Type returnType;
        std::vector<Type> parameterTypes;
    };

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
        //std::cerr << "Searching for variable '" << varName << "' in function '" << currentFunction << "'." << std::endl;
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
        {
            // for (const auto &entry : *it)
            // {
            //     std::cerr << "Checking variable '" << entry.first << "' in current scope." << std::endl;
            // }
            auto found = it->find(varName);
            if (found != it->end())
            {
                return &found->second;
            }
        }
        // std::cerr << "Variable '" << varName << "' not found in any scope of function '" << currentFunction << "'." << std::endl;
        return nullptr;
    }

    void pushScope()
    {
        // Print all var in current scope before pushing a new one
        if (!allSymbolTables[currentFunction].empty())
        {
            std::cerr << "Current scope variables for function '" << currentFunction << "':" << std::endl;
            for (const auto &entry : currentScope()) 
            {
                std::cerr << "  Variable '" << entry.first << "' of type " << static_cast<int>(entry.second.type) << " with pointer depth " << entry.second.pointerDepth << std::endl;
            }
        } else {
            std::cerr << "No current scope variables for function '" << currentFunction << "'." << std::endl;
        }
        std::cerr << "Pushing new scope for function '" << currentFunction << "'." << std::endl;
        allSymbolTables[currentFunction].push_back(std::map<std::string, Variable>());
    }

    void popScope()
    {
        std::cerr << "Popping scope for function '" << currentFunction << "'." << std::endl;
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
        // List all variables in the current scope for debugging
        std::cerr << "Checking if variable '" << varName << "' is declared in the current scope of function '" << currentFunction << "'." << std::endl;
        for (const auto &entry : currentScope())
        {
            std::cerr << "  Variable '" << entry.first << "' of type " << static_cast<int>(entry.second.type) << " with pointer depth " << entry.second.pointerDepth << std::endl;
        }

        bool res = currentScope().find(varName) != currentScope().end();
        std::cerr << "Variable '" << varName << "' is " << res<< " in the current scope of function '" << currentFunction << "'." << std::endl;

        return res;
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
