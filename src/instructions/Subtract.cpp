#include "instructions/Subtract.h"

void Subtract::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
