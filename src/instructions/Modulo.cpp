#include "instructions/Modulo.h"

void Modulo::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
