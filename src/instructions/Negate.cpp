#include "instructions/Negate.h"

void Negate::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
