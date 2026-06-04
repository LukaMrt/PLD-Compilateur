#include "instructions/BitwiseXor.h"

void BitwiseXor::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
