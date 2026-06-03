#include "instructions/Add.h"

void Add::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
