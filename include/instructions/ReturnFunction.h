#pragma once

#include "Instruction.h"
#include "struct/Variable.h"
#include "ControlFlowGraph.h"

using namespace std;

class CallFunction : public Instruction {
    public:
        void generate(Backend &backend, std::ostream &output) override ;
        
        void debug(std::ostream &output) const override;

        string getFunctionName() const;

    private:
        string functionName;
};