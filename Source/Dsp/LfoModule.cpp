#include "LfoModule.hpp"
#include <cmath>
#include <algorithm>

namespace MckDsp {
    LfoModule::LfoModule()
    {

    }
    LfoModule::~LfoModule()
    {

    }

    void LfoModule::prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        m_sampleRate = sampleRate;
        m_samplesPerBlock = samplesPerBlock;
    }

    double LfoModule::processSample()
    {
        double phase = static_cast<double>(m_curTick)/static_cast<double>(m_ticks);
        m_rect = m_curTick > m_ticks / 2 ? 0.0 : 1.0;
        m_curTick = (m_curTick + 1) % m_ticks;
        m_sine = 0.5 + std::sin(M_PI_2 * 2.0 * phase) / 2.0;
        return m_rect * m_shape + m_sine * (1.0 - m_shape);
    }

    void LfoModule::processBlock(double *writePtr)
    {

    }

    void LfoModule::setSpeed(double speed)
    {
        m_speed = std::min(speed, m_sampleRate / 2.0);
        m_ticks = static_cast<size_t>(std::round(m_sampleRate / m_speed));
    }

    void LfoModule::setShape(double shape)
    {
        m_shape = shape;
    }

    void LfoModule::calcUpdate()
    {
    }
}