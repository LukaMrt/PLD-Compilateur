#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "antlr4-runtime.h"
#include "generated/ifccLexer.h"
#include "generated/ifccParser.h"
#include "generated/ifccBaseVisitor.h"

#include "SymbolTableVisitor.h"
#include "IRGeneratorVisitor.h"
#include "backend/ARMBackend.h"

using namespace antlr4;

int main(int argn, const char **argv)
{
    bool debugIR = false;
    std::string filename;

    for (int i = 1; i < argn; ++i)
    {
        std::string arg(argv[i]);
        if (arg == "--debug-ir")
            debugIR = true;
        else
            filename = arg;
    }

    std::stringstream in;
    if (!filename.empty())
    {
        std::ifstream lecture(filename);
        if (!lecture.good())
        {
            std::cerr << "error: cannot read file: " << filename << std::endl;
            exit(1);
        }
        in << lecture.rdbuf();
    }
    else
    {
        std::cerr << "usage: ifcc [--debug-ir] path/to/file.c" << std::endl;
        exit(1);
    }

    ANTLRInputStream input(in.str());

    ifccLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    tokens.fill();

    if (lexer.getNumberOfSyntaxErrors() != 0)
    {
        std::cerr << "error: syntax error during lexing" << std::endl;
        exit(1);
    }

    ifccParser parser(&tokens);
    tree::ParseTree *tree = parser.axiom();

    if (parser.getNumberOfSyntaxErrors() != 0)
    {
        std::cerr << "error: syntax error during parsing" << std::endl;
        exit(1);
    }

    SymbolTableVisitor symbolTableVisitor;
    symbolTableVisitor.visit(tree);

    auto functionTable = symbolTableVisitor.getFunctionTable();
    if (functionTable.find("main") == functionTable.end())
    {
        std::cerr << "error: no 'main' function defined" << std::endl;
        exit(1);
    }

    IRGeneratorVisitor irVisitor(symbolTableVisitor.getAllSymbolTables());
    irVisitor.visit(tree);

    if (irVisitor.getDeadBlockCount() > 0)
        std::cerr << "warning: " << irVisitor.getDeadBlockCount() << " unreachable code block(s) detected\n";

    if (debugIR)
        for (const auto &[name, cfg] : irVisitor.getCFGs())
            cfg->debug(std::cerr);

    ARMBackend backend;
    for (const auto &[name, cfg] : irVisitor.getCFGs())
        cfg->generateASM(backend, std::cout);

    return 0;
}
