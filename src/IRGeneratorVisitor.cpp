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
#include "instructions/Lesser.h"
#include "instructions/Greater.h"
#include "instructions/LesserOrEqual.h"
#include "instructions/GreaterOrEqual.h"
#include "instructions/Reference.h"
#include "instructions/DereferenceRead.h"
#include "instructions/DereferenceWrite.h"

#include <set>

// Fonctions de la libc tolérées sans définition dans le fichier source.
static const std::set<std::string> libcFunctions = {
    "putchar",
    "getchar",
};

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
    currentFunction = ctx->IDENTIFIER()->getText();
    Type returnType = stringToType(ctx->TYPE()->getText());
    functionReturnTypes[currentFunction] = returnType;
    cfgs[currentFunction] = new ControlFlowGraph(currentFunction);
    currentCFG = cfgs[currentFunction];
    currentCFG->addVariable("$return", returnType);

    // Le bloc de sortie est créé ici mais n'est pas ajouté à la liste des blocs ;
    // il est émis séparément par generateASM juste avant l'épilogue.
    Block *exitBlock = new Block(currentCFG, currentFunction + "_exit");
    currentCFG->setExitBlock(exitBlock);

    Block *block = new Block(currentCFG, currentFunction + "_entry");
    currentCFG->addBlock(block);
    block->addInstruction(new LoadConstant(block, returnType, "$return", 0));

    if (functionTable.find(currentFunction) != functionTable.end())
    {
        std::cerr << "Error: function '" << currentFunction << "' is already defined." << std::endl;
        exit(1);
    }
    
    // Initialize with a vector containing one empty map (first scope for function parameters and local variables)
    allSymbolTables[currentFunction] = {{}};
    functionTable[currentFunction] = {stringToType(ctx->TYPE()->getText()), {}};

    this->visitChildren(ctx);

    // Print all unused variables collected during function processing
    for (const auto &unusedVar : unusedVariables)
    {
        if (unusedVar.first == currentFunction)
        {
            std::cerr << "Warning: variable '" << unusedVar.second << "' defined but never used." << std::endl;
        }
    }

    return 0;
}

antlrcpp::Any IRGeneratorVisitor::visitFunction_parameter_declaration(ifccParser::Function_parameter_declarationContext *ctx)
{
    std::string varName = ctx->IDENTIFIER()->getText();
    int pointerDepth = ctx->TIMES().size();
    
    Type type = stringToType(ctx->TYPE()->getText());

    this->declareVariable(varName, type, pointerDepth);
    functionTable[currentFunction].parameterTypes.push_back(type);

    currentCFG->addParameter(varName, type, pointerDepth);
    currentCFG->addVariable(varName, type, pointerDepth);
    return 0;
}

antlrcpp::Any IRGeneratorVisitor::visitVariable_definition_without_instruction(ifccParser::Variable_definition_without_instructionContext *ctx)
{
    std::string varName = ctx->left_value()->IDENTIFIER()->getText();
    // Type type = symbolTable.at(varName).type;
    int pointerDepth = ctx->left_value()->TIMES().size();

    auto declaration = dynamic_cast<ifccParser::Variable_declarationContext *>(ctx->parent);

    this->declareVariable(varName, stringToType(declaration->TYPE()->getText()), pointerDepth);

    Type type = findVariable(varName)->type;
    currentCFG->addVariable(varName, type, pointerDepth);

    Block *block = currentCFG->getCurrentBlock();
    block->addInstruction(new LoadConstant(block, type, varName, 0));
    return 0;
}

antlrcpp::Any IRGeneratorVisitor::visitVariable_definition_with_instruction(ifccParser::Variable_definition_with_instructionContext *ctx)
{
    std::string varName = ctx->left_value()->IDENTIFIER()->getText();
    // Type type = stringToType(ctx->TYPE()->getText());
    
    int pointerDepth = ctx->left_value()->TIMES().size();

    // Le TYPE n'est pas sur le variable_definition mais sur son parent
    // variable_declaration ('int' partagé par 'int a, b, c;').
    auto declaration = dynamic_cast<ifccParser::Variable_declarationContext *>(ctx->parent);

    this->declareVariable(varName, stringToType(declaration->TYPE()->getText()), pointerDepth);

    auto vis = visit(ctx->expression());
    std::string srcVar = asString(vis);

    Type type = findVariable(varName)->type;

    currentCFG->addVariable(varName, type, pointerDepth);

    Block *block = currentCFG->getCurrentBlock();
    block->addInstruction(new Copy(block, type, varName, srcVar));
    return visit(ctx->expression());
}


antlrcpp::Any IRGeneratorVisitor::visitAssignment(ifccParser::AssignmentContext *ctx)
{
    std::string srcVar = asString(visit(ctx->expression()));
    Block *block = currentCFG->getCurrentBlock();

    // Un lvalue sans étoile désigne directement une variable : copie directe.
    // Avec étoile(s), on calcule l'adresse cible puis on écrit à travers elle.
    if (ctx->left_value()->TIMES().empty())
    {
        std::string destVar = ctx->left_value()->IDENTIFIER()->getText();
        block->addInstruction(new Copy(block, currentCFG->getVar(destVar).type, destVar, srcVar));
    }
    else
    {
        std::string addr = evalAddress(ctx->left_value());
        block->addInstruction(new DereferenceWrite(block, currentCFG->getVar(addr).type, addr, srcVar));
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
    std::string varName = ctx->IDENTIFIER()->getText();
    this->useVariable(varName);

    return varName;
}

antlrcpp::Any IRGeneratorVisitor::visitFunction_call(ifccParser::Function_callContext *ctx)
{
    std::string funcName = ctx->IDENTIFIER()->getText();

    std::vector<std::string> args;
    for (auto expression : ctx->expression())
    {
        args.push_back(asString(visit(expression)));
    }

    auto it = functionTable.find(funcName);
    if (it == functionTable.end() && libcFunctions.find(funcName) == libcFunctions.end())
    {
        std::cerr << "Error: function '" << funcName << "' is not defined." << std::endl;
        exit(1);
    }

    // L'arité n'est vérifiée que pour les fonctions définies localement
    // (la signature des fonctions libc n'est pas connue).
    if (it != functionTable.end())
    {
        size_t expected = it->second.parameterTypes.size();
        size_t actual = ctx->expression().size();
        if (expected != actual)
        {
            std::cerr << "Error: function '" << funcName << "' expects " << expected
                      << " argument(s) but " << actual << " were given." << std::endl;
            exit(1);
        }
    }

    auto it_ = functionReturnTypes.find(funcName);
    Type type = it_ != functionReturnTypes.end() ? it_->second : Type::INT32;
    std::string tmp = currentCFG->addTempVariable(type);
    Block *block = currentCFG->getCurrentBlock();
    block->addInstruction(new CallFunction(block, type, tmp, funcName, args));
    return tmp;
}

antlrcpp::Any IRGeneratorVisitor::visitUnary_operation(ifccParser::Unary_operationContext *ctx)
{
    std::string srcVar = asString(visit(ctx->expression()));
    Variable srcInfo = currentCFG->getVar(srcVar);
    Type type = promote(srcInfo.type, srcInfo.type);

    int srcValue;
    if (ctx->op->getType() == ifccParser::MINUS && isConstant(srcVar, srcValue))
    {
        return emitConstant(type, -srcValue);
    }

    // Le résultat de `&x` est un pointeur (profondeur +1) ; celui de `*p` est
    // ce que pointe p (profondeur -1). Le temporaire doit porter cette
    // profondeur pour que le backend lui réserve 8 octets et émette movq.
    int tmpDepth = 0;
    if (ctx->op->getType() == ifccParser::BITWISE_AND)
        tmpDepth = srcInfo.pointerDepth + 1;
    else if (ctx->op->getType() == ifccParser::TIMES)
        tmpDepth = srcInfo.pointerDepth - 1;

    std::string tmp = currentCFG->addTempVariable(srcInfo.type, tmpDepth);
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
    else if (ctx->op->getType() == ifccParser::BITWISE_AND)
    {
        block->addInstruction(new Reference(block, type, tmp, srcVar));
    }
    else if (ctx->op->getType() == ifccParser::TIMES)
    {
        block->addInstruction(new DereferenceRead(block, type, tmp, srcVar));
    }

    return tmp;
}


std::string IRGeneratorVisitor::evalAddress(ifccParser::Left_valueContext *lv)
{
    // adresse(x)   = x           (la variable contient déjà l'adresse pour *x)
    // adresse(*p)  = valeur(p)   → 0 déréférencement intermédiaire
    // adresse(**pp)= valeur(*pp) → 1 déréférencement intermédiaire
    // Règle générale : pour n étoiles, n-1 DereferenceRead successifs.
    std::string cur = lv->IDENTIFIER()->getText();
    int depth = lv->TIMES().size();

    for (int i = 1; i < depth; ++i)
    {
        Variable curInfo = currentCFG->getVar(cur);
        // Chaque déréférencement intermédiaire produit lui-même une adresse
        // (profondeur -1) : sans ça le pointeur serait tronqué à 4 octets.
        std::string tmp = currentCFG->addTempVariable(curInfo.type, curInfo.pointerDepth - 1);
        Block *block = currentCFG->getCurrentBlock();
        block->addInstruction(new DereferenceRead(block, curInfo.type, tmp, cur));
        cur = tmp;
    }

    return cur;
}

antlrcpp::Any IRGeneratorVisitor::visitBracketed_expression(ifccParser::Bracketed_expressionContext *ctx)
{
    return visit(ctx->expression());
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

antlrcpp::Any IRGeneratorVisitor::visitStatement(ifccParser::StatementContext *ctx) {
    if (ctx->IF()) {
        // Save the current block that will contain the condition.
        Block *currentBlock = currentCFG->getCurrentBlock();

        Block *afterIfBlock = new Block(currentCFG, currentCFG->getCurrentBlock()->getLabel() + "_if_end" /*+ std::to_string(ifBlockCount)*/);

        currentCFG->getCurrentBlock()->setFalseCaseBlock(afterIfBlock);

        visit(ctx->expression());
        Block *trueBlock = new Block(currentCFG, currentCFG->getCurrentBlock()->getLabel() + "_if_true" /*+ std::to_string(ifBlockCount)*/);
        currentCFG->addBlock(trueBlock);
        visit(ctx->following_condition(0));
        currentCFG->getCurrentBlock()->setTrueCaseBlock(afterIfBlock);

        if (ctx->ELSE()) {

            // Reset state
            currentCFG->setCurrentBlock(currentBlock);

            Block *falseBlock = new Block(currentCFG, currentCFG->getCurrentBlock()->getLabel() + "_if_false" /*+ std::to_string(ifBlockCount)*/);
            currentCFG->getCurrentBlock()->setFalseCaseBlock(falseBlock);
            currentCFG->addBlock(falseBlock);
            visit(ctx->following_condition(1));
        }
        
        currentCFG->addBlock(afterIfBlock);
        return 0;
    } else if (ctx->WHILE()) {
        Block *testBlock = new Block(currentCFG, currentCFG->getCurrentBlock()->getLabel() + "_while_test" /*+ std::to_string(whileBlockCount)*/);
        Block *bodyBlock = new Block(currentCFG, currentCFG->getCurrentBlock()->getLabel() + "_while_body" /*+ std::to_string(whileBlockCount)*/);
        Block *afterWhileBlock = new Block(currentCFG, currentCFG->getCurrentBlock()->getLabel() + "_while_end" /*+ std::to_string(whileBlockCount)*/);

        currentCFG->addBlock(testBlock);
        visit(ctx->expression());
        currentCFG->getCurrentBlock()->setFalseCaseBlock(afterWhileBlock);
        currentCFG->addBlock(bodyBlock);
        visit(ctx->following_condition(0));
        currentCFG->getCurrentBlock()->setTrueCaseBlock(testBlock);

        currentCFG->addBlock(afterWhileBlock);
        return 0;
    } else {
        return visitChildren(ctx);
    }
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
        tmp = currentCFG->addTempVariable(type);
        Block *block = currentCFG->getCurrentBlock();
        block->addInstruction(new Equal(block, type, tmp, left, right));
    }
    else if (op == ifccParser::NOT_EQUAL)
    {
        if (leftIsConstant && rightIsConstant)
        {
            return emitConstant(type, leftValue != rightValue);
        }
        tmp = currentCFG->addTempVariable(type);
        Block *block = currentCFG->getCurrentBlock();
        block->addInstruction(new NotEqual(block, type, tmp, left, right));
    }

    return tmp;
}

antlrcpp::Any IRGeneratorVisitor::visitComparison_expression(ifccParser::Comparison_expressionContext *ctx)
{
    std::string left = asString(visit(ctx->expression(0)));
    std::string right = asString(visit(ctx->expression(1)));
    Type type = Type::INT32;

    int leftValue, rightValue;
    bool leftIsConstant = isConstant(left, leftValue);
    bool rightIsConstant = isConstant(right, rightValue);

    std::string tmp;
    size_t op = ctx->op->getType();

    if (op == ifccParser::LESSER)
    {
        if (leftIsConstant && rightIsConstant)
        {
            return emitConstant(type, leftValue < rightValue);
        }
        tmp = currentCFG->addTempVariable(type);
        Block *block = currentCFG->getCurrentBlock();
        block->addInstruction(new Lesser(block, type, tmp, left, right));
    }
    else if (op == ifccParser::GREATER)
    {
        if (leftIsConstant && rightIsConstant)
        {
            return emitConstant(type, leftValue > rightValue);
        }
        tmp = currentCFG->addTempVariable(type);
        Block *block = currentCFG->getCurrentBlock();
        block->addInstruction(new Greater(block, type, tmp, left, right));
    }
    else if (op == ifccParser::LESSER_OR_EQUAL)
    {
        if (leftIsConstant && rightIsConstant)
        {
            return emitConstant(type, leftValue <= rightValue);
        }
        tmp = currentCFG->addTempVariable(type);
        Block *block = currentCFG->getCurrentBlock();
        block->addInstruction(new LesserOrEqual(block, type, tmp, left, right));
    }
    else if (op == ifccParser::GREATER_OR_EQUAL)
    {
        if (leftIsConstant && rightIsConstant)
        {
            return emitConstant(type, leftValue >= rightValue);
        }
        tmp = currentCFG->addTempVariable(type);
        Block *block = currentCFG->getCurrentBlock();
        block->addInstruction(new GreaterOrEqual(block, type, tmp, left, right));
    }

    return tmp;
}

antlrcpp::Any IRGeneratorVisitor::visitLeft_value(ifccParser::Left_valueContext *ctx)
{
    this->checkDeclared(ctx->IDENTIFIER()->getText());
    return visitChildren(ctx);
}

antlrcpp::Any IRGeneratorVisitor::visitBlock(ifccParser::BlockContext *ctx)
{
    pushScope(); // Push a new scope for the block

    // Visit all statements in the block
    for (auto statement : ctx->statement())
    {
        visit(statement);
    }

    popScope(); // Pop the scope after finishing the block

    return 0;
}