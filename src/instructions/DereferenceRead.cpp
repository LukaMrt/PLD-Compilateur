#include "instructions/DereferenceRead.h"

void DereferenceRead::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
