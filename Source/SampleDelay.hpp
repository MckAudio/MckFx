#pragma once

namespace MckDsp
{
    class SampleDelay
    {
        SampleDelay();
        ~SampleDelay();

        double Process(double in);
    };
}