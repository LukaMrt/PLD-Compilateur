#include "instructions/Copy.h"

void Copy::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
