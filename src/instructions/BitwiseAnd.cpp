#include "instructions/BitwiseAnd.h"

void BitwiseAnd::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
