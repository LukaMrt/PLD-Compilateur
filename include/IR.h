#ifndef IR_H
#define IR_H

#include <vector>
#include <string>
#include <iostream>
#include <map>

using namespace std;

typedef enum
{
	INT32
} Type;

struct Variable
{
	Type type;
	int index;
};

class Block;
class Function;
class Backend;

class Instruction
{
public:
	Instruction(Block *block, Type type) : block(block), type(type) {}
	virtual ~Instruction() = default;

	virtual void generate(Backend &backend, ostream &output) = 0;

	Block *getBlock() const { return block; }
	Type getType() const { return type; }

protected:
	Type type;
	Block *block;
};

/**  The class for a block */

/* A few important comments.
	 IRInstr has no jump instructions.
	 cmp_* instructions behaves as an arithmetic two-operand instruction (add or mult),
	  returning a boolean value (as an int)

	 Assembly jumps are generated as follows:
	 Block::gen_asm() first calls IRInstr::gen_asm() on all its instructions, and then
			if  true_case_block  is a  nullptr,
			the epilogue is generated
		else if false_case_block is a nullptr,
		  an unconditional jmp to the true_case_block branch is generated
				else (we have two successors, hence a branch)
		  an instruction comparing the value of testVariableName to true is generated,
					followed by a conditional branch to the false_case_block branch,
					followed by an unconditional branch to the true_case_block branch
	 The attribute testVariableName itself is defined when converting
  the if, while, etc of the AST  to IR.

Possible optimization:
	 a cmp_* comparison instructions, if it is the last instruction of its block,
	   generates an actual assembly comparison
	   followed by a conditional jump to the exit_false branch
*/

class Block
{
public:
	Block(Function *Function, string entry_label);
	void generate(Backend &backend, ostream &output);

	void addInstruction(Instruction *instruction);

	string getLabel() { return label; }
	Block *getTrueCaseBlock() { return true_case_block; }
	Block *getFalseCaseBlock() { return false_case_block; }

	Function *getFunction() { return Function; }
	vector<Instruction *> getInstructions() { return instructions; }

private:
	Function *Function;					/** < the Function where this block belongs */
	string label;						/**< label of the block, also will be the label in the generated code */
	Block *true_case_block;				/**< pointer to the next block, true branch. If nullptr, return from procedure */
	Block *false_case_block;			/**< pointer to the next block, false branch. If null_ptr, the block ends with an unconditional jump */
	string test_variable_name;			/** < when generating IR code for an if(expr) or while(expr) etc, store here the name of the variable that holds the value of expr */
	vector<Instruction *> instructions; /** < the instructions themselves. */
};

/** The class for the control flow graph, also includes the symbol table */

/* A few important comments:
	 The entry block is the one with the same label as the AST function name.
	   (it could be the first of blocks, or it could be defined by an attribute value)
	 The exit block is the one with both exit pointers equal to nullptr.
	 (again it could be identified in a more explicit way)

 */
class Function
{
public:
	Function(string label);

	string label;
	
	void generate(Backend &backend, ostream &output);
	
	// variable map management
	void addVariable(string name, Type type);
	string addTempVariable(Type type); // returns the name of the new variable
	Variable getVar(string name);
	
	// blocks management
	void addBlock(Block *block);
	Block *currentBlock;

protected:
	map<string, Variable> variableMap;
	vector<Block *> blocks;
};

#endif
