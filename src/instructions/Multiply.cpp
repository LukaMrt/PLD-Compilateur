#include "instructions/Multiply.h"

void Multiply::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
