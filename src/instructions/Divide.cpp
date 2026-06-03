#include "instructions/Divide.h"

void Divide::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
