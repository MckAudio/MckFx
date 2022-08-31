#include "SampleDelay.hpp"

namespace MckDsp
{
    SampleDelay::SampleDelay()
    {
    }

    SampleDelay::~SampleDelay()
    {
    }
    double SampleDelay::Process(double in)
    {
        return in;
    }
}