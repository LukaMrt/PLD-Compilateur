#include "IRGeneratorVisitor.h"
#include "Block.h"
#include "instructions/Add.h"
#include "instructions/Subtract.h"
#include "instructions/Multiply.h"
#include "instructions/Divide.h"
#include "instructions/Modulo.h"
#include "instructions/Copy.h"
#include "instructions/LoadConstant.h"
#include "instructions/Negate.h"

static std::string asString(antlrcpp::Any any)
{
    return std::any_cast<std::string>(any);
}

antlrcpp::Any IRGeneratorVisitor::visitFunction(ifccParser::FunctionContext *ctx)
{
    std::string funcName = ctx->IDENTIFIER()->getText();
    Type returnType = stringToType(ctx->TYPE()->getText());
    cfg = new ControlFlowGraph(funcName);
    cfg->addVariable("$return", returnType);

    Block *block = new Block(cfg, funcName + "_entry");
    cfg->addBlock(block);
    block->addInstruction(new LoadConstant(block, returnType, "$return", 0));

    visitChildren(ctx);

    return 0;
}

antlrcpp::Any IRGeneratorVisitor::visitVariable_definition_without_instruction(ifccParser::Variable_definition_without_instructionContext *ctx)
{
    std::string varName = ctx->IDENTIFIER()->getText();
    Type type = symbolTable.at(varName).type;
    cfg->addVariable(varName, type);

    Block *block = cfg->getCurrentBlock();
    block->addInstruction(new LoadConstant(block, type, varName, 0));
    return 0;
}

antlrcpp::Any IRGeneratorVisitor::visitVariable_definition_with_instruction(ifccParser::Variable_definition_with_instructionContext *ctx)
{
    std::string varName = ctx->IDENTIFIER()->getText();
    Type type = symbolTable.at(varName).type;
    cfg->addVariable(varName, type);

    std::string srcVar = asString(visit(ctx->instruction()));
    Block *block = cfg->getCurrentBlock();
    block->addInstruction(new Copy(block, type, varName, srcVar));
    return 0;
}

antlrcpp::Any IRGeneratorVisitor::visitInstruction(ifccParser::InstructionContext *ctx)
{
    std::string srcVar = asString(visit(ctx->expression()));

    Block *block = cfg->getCurrentBlock();
    for (auto &id : ctx->IDENTIFIER())
    {
        std::string varName = id->getText();
        block->addInstruction(new Copy(block, symbolTable.at(varName).type, varName, srcVar));
    }

    return srcVar;
}

antlrcpp::Any IRGeneratorVisitor::visitReturn_statement(ifccParser::Return_statementContext *ctx)
{
    std::string srcVar = asString(visit(ctx->expression()));

    Block *block = cfg->getCurrentBlock();
    block->addInstruction(new Copy(block, cfg->getVar("$return").type, "$return", srcVar));
    return 0;
}

antlrcpp::Any IRGeneratorVisitor::visitConstant_expression(ifccParser::Constant_expressionContext *ctx)
{
    int value = std::stoi(ctx->CONSTANT()->getText());
    std::string tmp = cfg->addTempVariable(Type::INT32);

    Block *block = cfg->getCurrentBlock();
    block->addInstruction(new LoadConstant(block, Type::INT32, tmp, value));
    return tmp;
}

antlrcpp::Any IRGeneratorVisitor::visitCharacter_expression(ifccParser::Character_expressionContext *ctx)
{
    // Le texte du token CHARACTER est "'A'" : le caractère utile est à l'indice 1.
    int value = ctx->CHARACTER()->getText()[1];
    std::string tmp = cfg->addTempVariable(Type::CHAR);

    Block *block = cfg->getCurrentBlock();
    block->addInstruction(new LoadConstant(block, Type::CHAR, tmp, value));
    return tmp;
}

antlrcpp::Any IRGeneratorVisitor::visitVariable_expression(ifccParser::Variable_expressionContext *ctx)
{
    return ctx->IDENTIFIER()->getText();
}

antlrcpp::Any IRGeneratorVisitor::visitUnary_operation(ifccParser::Unary_operationContext *ctx)
{
    std::string srcVar = asString(visit(ctx->expression()));
    Type type = promote(cfg->getVar(srcVar).type, cfg->getVar(srcVar).type);
    std::string tmp = cfg->addTempVariable(type);

    Block *block = cfg->getCurrentBlock();
    if (ctx->op->getType() == ifccParser::MINUS)
    {
        block->addInstruction(new Negate(block, type, tmp, srcVar));
    }
    else if (ctx->op->getType() == ifccParser::NOT)
    {
        // !x  ≡  (x == 0) : à implémenter quand les instructions de comparaison seront ajoutées
        block->addInstruction(new LoadConstant(block, type, tmp, 0));
    }

    return tmp;
}

antlrcpp::Any IRGeneratorVisitor::visitBracketed_expression(ifccParser::Bracketed_expressionContext *ctx)
{
    return visit(ctx->instruction());
}

antlrcpp::Any IRGeneratorVisitor::visitAdditive_expression(ifccParser::Additive_expressionContext *ctx)
{
    std::string left = asString(visit(ctx->expression(0)));
    std::string right = asString(visit(ctx->expression(1)));
    Type type = promote(cfg->getVar(left).type, cfg->getVar(right).type);
    std::string tmp = cfg->addTempVariable(type);

    Block *block = cfg->getCurrentBlock();
    if (ctx->op->getType() == ifccParser::PLUS)
    {
        block->addInstruction(new Add(block, type, tmp, left, right));
    }
    else
    {
        block->addInstruction(new Subtract(block, type, tmp, left, right));
    }

    return tmp;
}

antlrcpp::Any IRGeneratorVisitor::visitMultiplicative_expression(ifccParser::Multiplicative_expressionContext *ctx)
{
    std::string left = asString(visit(ctx->expression(0)));
    std::string right = asString(visit(ctx->expression(1)));
    Type type = promote(cfg->getVar(left).type, cfg->getVar(right).type);
    std::string tmp = cfg->addTempVariable(type);

    Block *block = cfg->getCurrentBlock();
    if (ctx->op->getType() == ifccParser::TIMES)
    {
        block->addInstruction(new Multiply(block, type, tmp, left, right));
    }
    else if (ctx->op->getType() == ifccParser::DIVIDE)
    {
        block->addInstruction(new Divide(block, type, tmp, left, right));
    }
    else
    {
        block->addInstruction(new Modulo(block, type, tmp, left, right));
    }

    return tmp;
}
