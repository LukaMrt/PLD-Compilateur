#include "instructions/GreaterOrEqual.h"

void GreaterOrEqual::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
