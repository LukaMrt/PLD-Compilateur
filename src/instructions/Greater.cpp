#include "instructions/Greater.h"

void Greater::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
