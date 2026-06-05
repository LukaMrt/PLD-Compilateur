#include "instructions/NotEqual.h"

void NotEqual::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
