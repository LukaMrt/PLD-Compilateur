#include "instructions/LoadConstant.h"

void LoadConstant::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
