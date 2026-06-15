#include "instructions/Reference.h"

void Reference::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
