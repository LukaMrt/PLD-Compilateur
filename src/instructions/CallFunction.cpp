#include "instructions/CallFunction.h"

using namespace std;

void CallFunction::generate(Backend &backend, std::ostream &output) {
    backend.emit(this, output);
}

string CallFunction::getFunctionName() const {
    return functionName;
}

vector<Variable> CallFunction::getArguments() const {
    return arguments;
}
