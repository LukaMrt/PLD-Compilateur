#include <vector>
#include <string>
#include <iostream>
#include <map>

#include "Instruction.h"
#include "Backend.h"

using namespace std;

class BasicBlock
{
public:
	BasicBlock(Function *Function, string entry_label);
	void generateASM(Backend &backend, ostream &output);

	void addInstruction(Instruction *instruction);

	string getLabel() { return label; }
	BasicBlock *getTrueCaseBlock() { return true_case_block; }
	BasicBlock *getFalseCaseBlock() { return false_case_block; }

	Function *getFunction() { return Function; }
	vector<Instruction *> getInstructions() { return instructions; }

private:
	Function *Function;					/** < the Function where this block belongs */
	string label;						/**< label of the block, also will be the label in the generated code */
	BasicBlock *true_case_block;				/**< pointer to the next block, true branch. If nullptr, return from procedure */
	BasicBlock *false_case_block;			/**< pointer to the next block, false branch. If null_ptr, the block ends with an unconditional jump */
	string test_variable_name;			/** < when generating IR code for an if(expr) or while(expr) etc, store here the name of the variable that holds the value of expr */
	vector<Instruction *> instructions; /** < the instructions themselves. */
};