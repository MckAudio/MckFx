#pragma once

#include <vector>

namespace MckDsp
{
    class SampleDelay
    {
        public:
        SampleDelay();
        ~SampleDelay();

        void prepareToPlay(double sampleRate, int samplesPerBlock = 0);

        double processSample(double in);

        void setMaxDelayInMs(double maxDelayInMs);
        double getMaxDelayInMs() { return m_maxDelayInMs; };

        void setDelayInMs(double delayInMs);

        void setMix(double mix);

        void setFeedback(double fb);

        private:
            void resizeBuffer(double sampleRate, double maxDelayInMs);

            double m_sampleRate { 0 };

            double m_mix { 0.0 };
            double m_fb { 0.0 };

            double m_maxDelayInMs { 1000.0 };
            unsigned m_maxDelayInSamples { 0 };

            double m_delayInMs { 0.0 };
            unsigned m_delayInSamples { 0 };

            unsigned m_len { 0 };
            unsigned m_idx { 0 };
            std::vector<double> m_buf {};
    };
}