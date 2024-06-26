#include "sample.h"

Sample::Sample(const float value, const int time)
{
    this->time = time;
    this->value = value;
}

Sample::Sample(const int rawValue, const int maxRawValue, const float maxVoltValue, const int time)
{
    this->value = rawValue * (maxVoltValue / (float)maxRawValue);
    this->time = time;
}

FrequencySample::FrequencySample(const float value, const int frequency)
{
    this->value = value;
    this->frequency = frequency;
}
