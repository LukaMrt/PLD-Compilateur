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
#include "instructions/BitwiseAnd.h"
#include "instructions/BitwiseOr.h"
#include "instructions/BitwiseXor.h"
#include "instructions/CallFunction.h"
#include "instructions/Equal.h"
#include "instructions/NotEqual.h"

static std::string asString(antlrcpp::Any any)
{
    return std::any_cast<std::string>(any);
}

bool IRGeneratorVisitor::isConstant(const std::string &v, int &out) const
{
    auto it = knownConstants.find(v);
    if (it == knownConstants.end())
    {
        return false;
    }
    out = it->second;
    return true;
}

std::string IRGeneratorVisitor::emitConstant(Type type, int value)
{
    std::string tmp = currentCFG->addTempVariable(type);
    Block *block = currentCFG->getCurrentBlock();
    block->addInstruction(new LoadConstant(block, type, tmp, value));
    knownConstants[tmp] = value;
    return tmp;
}

antlrcpp::Any IRGeneratorVisitor::visitFunction(ifccParser::FunctionContext *ctx)
{
    std::string funcName = ctx->IDENTIFIER()->getText();
    symbolTable = allSymbolTables.at(funcName);
    Type returnType = stringToType(ctx->TYPE()->getText());
    functionReturnTypes[funcName] = returnType;
    cfgs[funcName] = new ControlFlowGraph(funcName);
    currentCFG = cfgs[funcName];
    currentCFG->addVariable("$return", returnType);

    // Le bloc de sortie est créé ici mais n'est pas ajouté à la liste des blocs ;
    // il est émis séparément par generateASM juste avant l'épilogue.
    Block *exitBlock = new Block(currentCFG, funcName + "_exit");
    currentCFG->setExitBlock(exitBlock);

    Block *block = new Block(currentCFG, funcName + "_entry");
    currentCFG->addBlock(block);
    block->addInstruction(new LoadConstant(block, returnType, "$return", 0));

    visitChildren(ctx);

    return 0;
}

antlrcpp::Any IRGeneratorVisitor::visitFunction_parameter_declaration(ifccParser::Function_parameter_declarationContext *ctx)
{
    std::string varName = ctx->IDENTIFIER()->getText();
    Type type = symbolTable.at(varName).type;
    currentCFG->addParameter(varName, type);
    currentCFG->addVariable(varName, type);
    return 0;
}

antlrcpp::Any IRGeneratorVisitor::visitVariable_definition_without_instruction(ifccParser::Variable_definition_without_instructionContext *ctx)
{
    std::string varName = ctx->IDENTIFIER()->getText();
    Type type = symbolTable.at(varName).type;
    currentCFG->addVariable(varName, type);

    Block *block = currentCFG->getCurrentBlock();
    block->addInstruction(new LoadConstant(block, type, varName, 0));
    return 0;
}

antlrcpp::Any IRGeneratorVisitor::visitVariable_definition_with_instruction(ifccParser::Variable_definition_with_instructionContext *ctx)
{
    std::string varName = ctx->IDENTIFIER()->getText();
    Type type = symbolTable.at(varName).type;
    currentCFG->addVariable(varName, type);

    std::string srcVar = asString(visit(ctx->instruction()));
    Block *block = currentCFG->getCurrentBlock();
    block->addInstruction(new Copy(block, type, varName, srcVar));
    return 0;
}

antlrcpp::Any IRGeneratorVisitor::visitInstruction(ifccParser::InstructionContext *ctx)
{
    std::string srcVar = asString(visit(ctx->expression()));

    Block *block = currentCFG->getCurrentBlock();
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

    Block *block = currentCFG->getCurrentBlock();
    block->addInstruction(new Copy(block, currentCFG->getVar("$return").type, "$return", srcVar));
    block->setTrueCaseBlock(currentCFG->getExitBlock());

    std::string deadLabel = currentCFG->getLabel() + "_dead" + std::to_string(deadBlockCount++);
    Block *deadBlock = new Block(currentCFG, deadLabel);
    currentCFG->addBlock(deadBlock);

    return 0;
}

antlrcpp::Any IRGeneratorVisitor::visitConstant_expression(ifccParser::Constant_expressionContext *ctx)
{
    int value = std::stoi(ctx->CONSTANT()->getText());
    return emitConstant(Type::INT32, value);
}

antlrcpp::Any IRGeneratorVisitor::visitCharacter_expression(ifccParser::Character_expressionContext *ctx)
{
    // Le texte du token CHARACTER est "'A'" : le caractère utile est à l'indice 1.
    int value = ctx->CHARACTER()->getText()[1];
    return emitConstant(Type::CHAR, value);
}

antlrcpp::Any IRGeneratorVisitor::visitVariable_expression(ifccParser::Variable_expressionContext *ctx)
{
    return ctx->IDENTIFIER()->getText();
}

antlrcpp::Any IRGeneratorVisitor::visitFunction_call(ifccParser::Function_callContext *ctx)
{
    std::string funcName = ctx->IDENTIFIER()->getText();

    std::vector<std::string> args;
    for (auto expression : ctx->expression())
    {
        args.push_back(asString(visit(expression)));
    }

    auto it = functionReturnTypes.find(funcName);
    Type type = it != functionReturnTypes.end() ? it->second : Type::INT32;
    std::string tmp = currentCFG->addTempVariable(type);
    Block *block = currentCFG->getCurrentBlock();
    block->addInstruction(new CallFunction(block, type, tmp, funcName, args));
    return tmp;
}

antlrcpp::Any IRGeneratorVisitor::visitUnary_operation(ifccParser::Unary_operationContext *ctx)
{
    std::string srcVar = asString(visit(ctx->expression()));
    Type type = promote(currentCFG->getVar(srcVar).type, currentCFG->getVar(srcVar).type);

    int srcValue;
    if (ctx->op->getType() == ifccParser::MINUS && isConstant(srcVar, srcValue))
    {
        return emitConstant(type, -srcValue);
    }

    std::string tmp = currentCFG->addTempVariable(type);
    Block *block = currentCFG->getCurrentBlock();
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
    Type type = promote(currentCFG->getVar(left).type, currentCFG->getVar(right).type);
    bool isAddition = ctx->op->getType() == ifccParser::PLUS;

    int leftValue, rightValue;
    bool leftIsConstant = isConstant(left, leftValue);
    bool rightIsConstant = isConstant(right, rightValue);

    if (leftIsConstant && rightIsConstant)
    {
        return emitConstant(type, isAddition ? leftValue + rightValue : leftValue - rightValue);
    }
    // Élément neutre 0 : x + 0, x - 0, 0 + x
    if (rightIsConstant && rightValue == 0)
    {
        return left;
    }
    if (leftIsConstant && leftValue == 0 && isAddition)
    {
        return right;
    }

    std::string tmp = currentCFG->addTempVariable(type);
    Block *block = currentCFG->getCurrentBlock();
    if (isAddition)
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
    Type type = promote(currentCFG->getVar(left).type, currentCFG->getVar(right).type);
    size_t op = ctx->op->getType();

    int leftValue, rightValue;
    bool leftIsConstant = isConstant(left, leftValue);
    bool rightIsConstant = isConstant(right, rightValue);

    // Pliage : on évite de plier une division/modulo par zéro (laissée au runtime).
    bool divisionByZero = (op != ifccParser::TIMES) && rightIsConstant && rightValue == 0;
    if (leftIsConstant && rightIsConstant && !divisionByZero)
    {
        int result = op == ifccParser::TIMES  ? leftValue * rightValue
                     : op == ifccParser::DIVIDE ? leftValue / rightValue
                                                : leftValue % rightValue;
        return emitConstant(type, result);
    }

    if (op == ifccParser::TIMES)
    {
        // Élément absorbant : x * 0 == 0
        if ((leftIsConstant && leftValue == 0) || (rightIsConstant && rightValue == 0))
        {
            return emitConstant(type, 0);
        }
        // Élément neutre : x * 1, 1 * x
        if (rightIsConstant && rightValue == 1)
        {
            return left;
        }
        if (leftIsConstant && leftValue == 1)
        {
            return right;
        }
    }
    else if (op == ifccParser::DIVIDE && rightIsConstant && rightValue == 1)
    {
        // Élément neutre : x / 1
        return left;
    }

    std::string tmp = currentCFG->addTempVariable(type);
    Block *block = currentCFG->getCurrentBlock();
    if (op == ifccParser::TIMES)
    {
        block->addInstruction(new Multiply(block, type, tmp, left, right));
    }
    else if (op == ifccParser::DIVIDE)
    {
        block->addInstruction(new Divide(block, type, tmp, left, right));
    }
    else
    {
        block->addInstruction(new Modulo(block, type, tmp, left, right));
    }

    return tmp;
}

antlrcpp::Any IRGeneratorVisitor::visitBitwise_and_expression(ifccParser::Bitwise_and_expressionContext *ctx)
{
    std::string left = asString(visit(ctx->expression(0)));
    std::string right = asString(visit(ctx->expression(1)));
    Type type = promote(currentCFG->getVar(left).type, currentCFG->getVar(right).type);

    int leftValue, rightValue;
    bool leftIsConstant = isConstant(left, leftValue);
    bool rightIsConstant = isConstant(right, rightValue);

    if (leftIsConstant && rightIsConstant)
    {
        return emitConstant(type, leftValue & rightValue);
    }
    // Élément absorbant : x & 0 == 0
    if ((leftIsConstant && leftValue == 0) || (rightIsConstant && rightValue == 0))
    {
        return emitConstant(type, 0);
    }

    std::string tmp = currentCFG->addTempVariable(type);
    Block *block = currentCFG->getCurrentBlock();
    block->addInstruction(new BitwiseAnd(block, type, tmp, left, right));
    return tmp;
}

antlrcpp::Any IRGeneratorVisitor::visitBitwise_or_expression(ifccParser::Bitwise_or_expressionContext *ctx)
{
    std::string left = asString(visit(ctx->expression(0)));
    std::string right = asString(visit(ctx->expression(1)));
    Type type = promote(currentCFG->getVar(left).type, currentCFG->getVar(right).type);

    int leftValue, rightValue;
    bool leftIsConstant = isConstant(left, leftValue);
    bool rightIsConstant = isConstant(right, rightValue);

    if (leftIsConstant && rightIsConstant)
    {
        return emitConstant(type, leftValue | rightValue);
    }
    // Élément neutre : x | 0, 0 | x
    if (rightIsConstant && rightValue == 0)
    {
        return left;
    }
    if (leftIsConstant && leftValue == 0)
    {
        return right;
    }

    std::string tmp = currentCFG->addTempVariable(type);
    Block *block = currentCFG->getCurrentBlock();
    block->addInstruction(new BitwiseOr(block, type, tmp, left, right));
    return tmp;
}

antlrcpp::Any IRGeneratorVisitor::visitBitwise_xor_expression(ifccParser::Bitwise_xor_expressionContext *ctx)
{
    std::string left = asString(visit(ctx->expression(0)));
    std::string right = asString(visit(ctx->expression(1)));
    Type type = promote(currentCFG->getVar(left).type, currentCFG->getVar(right).type);

    int leftValue, rightValue;
    bool leftIsConstant = isConstant(left, leftValue);
    bool rightIsConstant = isConstant(right, rightValue);

    if (leftIsConstant && rightIsConstant)
    {
        return emitConstant(type, leftValue ^ rightValue);
    }
    // Élément neutre : x ^ 0, 0 ^ x
    if (rightIsConstant && rightValue == 0)
    {
        return left;
    }
    if (leftIsConstant && leftValue == 0)
    {
        return right;
    }

    std::string tmp = currentCFG->addTempVariable(type);
    Block *block = currentCFG->getCurrentBlock();
    block->addInstruction(new BitwiseXor(block, type, tmp, left, right));
    return tmp;
}

antlrcpp::Any IRGeneratorVisitor::visitEqual_expression(ifccParser::Equal_expressionContext *ctx)
{
    std::string left = asString(visit(ctx->expression(0)));
    std::string right = asString(visit(ctx->expression(1)));
    Type type = Type::INT32;

    int leftValue, rightValue;
    bool leftIsConstant = isConstant(left, leftValue);
    bool rightIsConstant = isConstant(right, rightValue);

    std::string tmp;
    size_t op = ctx->op->getType();

    if (op == ifccParser::COMPARE_EQUAL)
    {
        if (leftIsConstant && rightIsConstant)
        {
            return emitConstant(type, leftValue == rightValue);
        }
        tmp = cfg->addTempVariable(type);
        Block *block = cfg->getCurrentBlock();
        block->addInstruction(new Equal(block, type, tmp, left, right));
    }
    else if (op == ifccParser::NOT_EQUAL)
    {
        if (leftIsConstant && rightIsConstant)
        {
            return emitConstant(type, leftValue != rightValue);
        }
        tmp = cfg->addTempVariable(type);
        Block *block = cfg->getCurrentBlock();
        block->addInstruction(new NotEqual(block, type, tmp, left, right));
    }

    return tmp;
}