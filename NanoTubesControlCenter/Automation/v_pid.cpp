#include "stdafx.h"
#include "v_pid.h"

VPid::VPid(double proportional, double integral, double differential,
           double period, double minSaturation, double maxSaturation)
    : m_proportional(proportional),
      m_integral(integral),
      m_differential(differential),
      m_period(period),
      m_minSaturation(minSaturation),
      m_maxSaturation(maxSaturation)
{
    m_lastResult = m_lastInput = 0;
}

double VPid::getValue(double input)
{
    double sum = m_lastResult + m_integral * m_period * input;
    double diff = m_differential / m_period * (input - m_lastInput);
    double result = m_proportional * input + sum + diff;

    result = min(m_minSaturation, result);
    result = max(m_maxSaturation, result);

    m_lastResult = sum;
    m_lastInput = input;

    return result;
}
