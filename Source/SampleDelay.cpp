#include "SampleDelay.hpp"

#include <cmath>
#include <cstddef>

namespace MckDsp
{
    SampleDelay::SampleDelay()
    {
    }

    SampleDelay::~SampleDelay()
    {
    }

    void SampleDelay::prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        resizeBuffer(sampleRate, m_maxDelayInMs);
    }

    double SampleDelay::processSample(double in)
    {
        if (m_len == 0)
        {
            return in;
        }

        unsigned readIdx = (m_idx + m_len - m_delayInSamples) % m_len;

        m_buf[m_idx] = m_fb * m_buf[readIdx] + in;

        m_idx = (m_idx + 1) % m_len;

        return m_mix * m_buf[readIdx] + (1.0 - m_mix) * in;
    }

    void SampleDelay::setMaxDelayInMs(double maxDelayInMs)
    {
        resizeBuffer(m_sampleRate, maxDelayInMs);
    }

    void SampleDelay::setDelayInMs(double delayInMs)
    {
        m_delayInMs = std::min(delayInMs, m_maxDelayInMs);
        m_delayInSamples = static_cast<unsigned>(std::round(m_delayInMs / 1000.0 * m_sampleRate));
    }

    void SampleDelay::setMix(double mix)
    {
        m_mix = std::min(1.0, std::max(0.0, mix));
    }

    void SampleDelay::setFeedback(double fb)
    {
        m_fb = std::min(1.0, std::max(0.0, fb));
    }

    void SampleDelay::resizeBuffer(double sampleRate, double maxDelayInMs)
    {
        unsigned maxDly = static_cast<unsigned>(std::ceil(maxDelayInMs / 1000.0 * sampleRate));

        if (maxDly > m_len)
        {
            m_buf.resize(maxDly, 0.0);
            m_len = maxDly;
        }
        m_sampleRate = sampleRate;
        m_maxDelayInSamples = maxDly;
        m_maxDelayInMs = maxDelayInMs;
    }
}