#include "instructions/Lesser.h"

void Lesser::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
