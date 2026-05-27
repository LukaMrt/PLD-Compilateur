#include "CodeGenVisitor.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx)
{
#ifdef __APPLE__
    std::cout << ".globl _main\n";
    std::cout << " _main: \n";
#else
    std::cout << ".globl main\n";
    std::cout << " main: \n";
#endif

    // Prologue
    std::cout << "    # Prologue\n";
    std::cout << "    push %rbp         # save %rbp on the stack\n";
    std::cout << "    mov %rsp, %rbp    # define %rbp for the current function\n";
    std::cout << "\n";
    
    this->visit(ctx->return_stmt());
    
    // Epilogue
    std::cout << "\n";
    std::cout << "    # Epilogue\n";
    std::cout << "    pop %rbp         # restore %rbp from the stack\n";
    std::cout << "    ret\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    int retval = stoi(ctx->CONST()->getText());

#ifdef __aarch64__
    std::cout << "    mov w0, #" << retval << "\n";
#else
    std::cout << "    movl $" << retval << ", %eax\n";
#endif

    return 0;
}
