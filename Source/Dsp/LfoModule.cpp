#include "LfoModule.hpp"
#include <cmath>
#include <algorithm>

namespace MckDsp
{
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
        double phase = static_cast<double>(m_curTick) / static_cast<double>(m_ticks);
        m_rect = m_curTick > m_ticks / 2 ? -1.0 : 1.0;
        m_curTick = (m_curTick + 1) % m_ticks;
        m_sine = std::sin(M_PI * 2.0 * phase);
        return m_rect * m_shape + m_sine * (1.0 - m_shape);
    }

    void LfoModule::processBlock(double *writePtr)
    {
    }

    void LfoModule::setSpeed(double speed)
    {
        m_speed = std::min(speed, m_sampleRate / 2.0);
        double curPhase = static_cast<double>(m_curTick)/static_cast<double>(m_ticks);
        double newTicks = m_sampleRate / m_speed;
        m_ticks = static_cast<size_t>(std::round(newTicks));
        m_curTick = static_cast<size_t>(std::round(curPhase * newTicks));
    }

    void LfoModule::setShape(double shape)
    {
        m_shape = shape;
    }

    void LfoModule::calcUpdate()
    {
    }
}