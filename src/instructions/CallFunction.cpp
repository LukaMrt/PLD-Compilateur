#include "instructions/CallFunction.h"

void CallFunction::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
