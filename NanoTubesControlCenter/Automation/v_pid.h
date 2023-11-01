#ifndef __V_PID_H
#define __V_PID_H

class VPid
{
public:

    VPid(double proportional, double integral, double differential,
         double period, double minSaturation, double maxSaturation);

    double getValue(double input);

private:

    double m_lastResult;
    double m_lastInput;

    double m_proportional;
    double m_integral;
    double m_differential;
    double m_period;
    double m_minSaturation;
    double m_maxSaturation;
};

#endif /* __V_PID_H */
