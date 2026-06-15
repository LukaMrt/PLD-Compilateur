#include "instructions/DereferenceWrite.h"
#include "backend/Backend.h"

void DereferenceWrite::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
