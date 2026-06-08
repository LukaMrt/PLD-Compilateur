#include "instructions/LesserOrEqual.h"

void LesserOrEqual::generate(Backend &backend, std::ostream &output)
{
    backend.emit(this, output);
}
