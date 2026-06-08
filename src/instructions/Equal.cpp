#include "instructions/Equal.h"

void Equal::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
