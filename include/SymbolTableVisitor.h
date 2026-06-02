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

        virtual antlrcpp::Any visitFunction(ifccParser::FunctionContext *ctx) override;
        virtual antlrcpp::Any visitFunction_variable_declaration(ifccParser::Function_variable_declarationContext *ctx) override;
        virtual antlrcpp::Any visitVariable_definition_with_instruction(ifccParser::Variable_definition_with_instructionContext *ctx) override;
        virtual antlrcpp::Any visitVariable_definition_without_instruction(ifccParser::Variable_definition_without_instructionContext *ctx) override;
        virtual antlrcpp::Any visitInstruction(ifccParser::InstructionContext *ctx) override;
        virtual antlrcpp::Any visitVariable_expression(ifccParser::Variable_expressionContext *ctx) override;
        std::map<std::string, VariableInfo> getSymbolTable() const { return symbolTable; }

private:
        std::map<std::string, VariableInfo> symbolTable;
        int nextOffset = -4;

        bool isDeclared(const std::string &varName)
        {
                return symbolTable.find(varName) != symbolTable.end();
        }

        void declareVariable(const std::string &varName)
        {
                if (isDeclared(varName))
                {
                        std::cerr << "Error: variable '" << varName << "' is already declared." << std::endl;
                        exit(1);
                }
                symbolTable[varName] = {nextOffset, false};
                nextOffset -= 4;
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
                symbolTable[varName].used = true;
        }
};
