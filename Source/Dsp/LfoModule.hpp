#pragma once

#include <cstddef>

namespace MckDsp
{
    class LfoModule
    {
        public:
        LfoModule();
        ~LfoModule();

        void prepareToPlay(double sampleRate, int samplesPerBlock);

        double processSample();

        void processBlock(double *writePtr);

        void setSpeed(double speed);

        void setShape(double shape);

        private:

        void calcUpdate();

        double m_sampleRate{0};
        size_t m_samplesPerBlock{0};

        double m_speed{0.5};
        double m_shape{0.0};

        size_t m_ticks{0};
        size_t m_curTick{0};

        double m_sine{0.0};
        double m_rect{0.0};
    };
}