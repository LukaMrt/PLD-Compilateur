#include "instructions/BitwiseOr.h"

void BitwiseOr::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
