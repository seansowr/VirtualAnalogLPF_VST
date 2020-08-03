//
//  IirFilter.cpp
//  LPF1
//
//  Created by Sean Wells-Rutherford on 11/7/19.
//

#include <cmath>
#include "IirFilter.h"

#define PI 3.14159265

/*
 * IirFilter default constructor
 *
 * Creates IirFilter object with pass-thru coefficients
 */
IirFilter::IirFilter() : RAMP_FBK(0.95f)
{
    frameSize = 1;
    enable = true;
    sampleRate = 48.0e3f;
    
    // Init to pass thru
    numCoeffsCurrent[0] = numCoeffsTarget[0] = 1.0f;
    numCoeffsCurrent[1] = numCoeffsTarget[1] = 0.0f;
    numCoeffsCurrent[2] = numCoeffsTarget[2] = 0.0f;
    denCoeffsCurrent[0] = denCoeffsTarget[0] = 0.0f;
    denCoeffsCurrent[1] = denCoeffsTarget[1] = 0.0f;
    
    states[0] = states[1] = 0.0f;
    
    currentCutoff = targetCutoff = 1.0e3f;
    currentGain = targetGain = 0.0f;
    currentQ = targetQ = 0.7071068f;
}

/*
 * IirFilter alternate constructor
 *
 * Creates IirFilter object with cutoff of 1kHz
 *
 * Inputs:
 * framesize    number of samples per frame
 * samplerate   samples processed per second
 */
IirFilter::IirFilter(int framesize, float samplerate) : RAMP_FBK(0.95f)
{
    frameSize = framesize;
    enable = true;
    sampleRate = samplerate;
    
    // Init to fc = 1kHz, unity passband gain, and Q = 1/sqrt(2)
    currentCutoff = targetCutoff = 1.0e3f;
    currentGain = targetGain = 0.0f;
    currentQ = targetQ = 0.7071068f;
    CalculateCoefficients(currentCutoff, currentGain, currentQ);
    numCoeffsCurrent[0] = numCoeffsTarget[0];
    numCoeffsCurrent[1] = numCoeffsTarget[1];
    numCoeffsCurrent[2] = numCoeffsTarget[2];
    denCoeffsCurrent[0] = denCoeffsTarget[0];
    denCoeffsCurrent[1] = denCoeffsTarget[1];
    
    states[0] = states[1] = 0.0f;
}

/*
 * IirFilter deconstructor
 */
IirFilter::~IirFilter()
{
    // No memory allocated on heap; no cleanup necessary
}

/*
 * IirFilter Process
 *
 * Filters input frame using second order IIR filter
 * in direct form II
 *
 * Inputs:
 * input        pointer to input samples
 *
 * Outputs:
 * output       pointer to output samples
 */
void IirFilter::Process(const float *input, float *output)
{
    if (enable)
    {
        for (int n = 0; n < frameSize; n++)
        {
            // Calculate output using DFII difference eqs
            float s = input[n] - denCoeffsCurrent[0]*states[0] - denCoeffsCurrent[1]*states[1];
            output[n] = numCoeffsCurrent[0]*s + numCoeffsCurrent[1]*states[0] + numCoeffsCurrent[2]*states[1];
            states[1] = states[0];
            states[0] = s;
        }
    }
    else
    {
        for (int n = 0; n < frameSize; n++)
        {
            // if disabled, just pass thru
            output[n] = input[n];
        }
    }
}

/*
 * IirFilter CalculateCoefficients
 *
 * Calculates new coefficients when parameters are changed
 *
 * Inputs:
 * cutoffFreq       -3dB cutoff frequency
 * leveldB          passband gain in dB
 * resonance        Q value for resonant filter
 */
void IirFilter::CalculateCoefficients(float cutoffFreq, float leveldB, float resonance)
{
    // Pre-warp cutoff frequency
    float omega = tanf(PI*cutoffFreq/sampleRate);
    
    // Calculate s-domain warped coefficients
    // e.g.
    // H(s) = beta / s^2 + alpha1*s + alpha2
    float beta = omega*omega;
    float alpha1 = omega/resonance;
    float alpha2 = beta;
    
    // calculate z-domain coefficients via bilinear transform
    float b0 = beta/(1.0f + alpha1 + alpha2);
    float b1 = 2.0f*b0;
    float b2 = b0;
    float a1 = (2.0f*alpha2 - 2.0)/(1.0f + alpha1 + alpha2);
    float a2 = (1.0f - alpha1 + alpha2)/(1.0f + alpha1 + alpha2);
    denCoeffsTarget[0] = a1;
    denCoeffsTarget[1] = a2;
    
    // apply gain
    float gain = powf(10.0, leveldB/20.0f);
    numCoeffsTarget[0] = gain*b0;
    numCoeffsTarget[1] = gain*b1;
    numCoeffsTarget[2] = gain*b2;
    
#ifdef USE_PARAM_RAMP
    denCoeffsCurrent[0] = denCoeffsTarget[0];
    denCoeffsCurrent[1] = denCoeffsTarget[1];
    numCoeffsCurrent[0] = numCoeffsTarget[0];
    numCoeffsCurrent[1] = numCoeffsTarget[1];
    numCoeffsCurrent[2] = numCoeffsTarget[2];
#endif
}

/*
 * IirFilter RampCoefficients
 *
 * Slowly ramps old coefficients to new coefficients to avoid
 * audio artefacts
 *
 */
void IirFilter::RampCoefficients()
{
    // slowly ramp coefficients to user input to avoid artefacts
    numCoeffsCurrent[0] = RAMP_FBK*numCoeffsCurrent[0] + (1.0f - RAMP_FBK)*numCoeffsTarget[0];
    numCoeffsCurrent[1] = RAMP_FBK*numCoeffsCurrent[1] + (1.0f - RAMP_FBK)*numCoeffsTarget[1];
    numCoeffsCurrent[2] = RAMP_FBK*numCoeffsCurrent[2] + (1.0f - RAMP_FBK)*numCoeffsTarget[2];
    denCoeffsCurrent[0] = RAMP_FBK*denCoeffsCurrent[0] + (1.0f - RAMP_FBK)*denCoeffsTarget[0];
    denCoeffsCurrent[1] = RAMP_FBK*denCoeffsCurrent[1] + (1.0f - RAMP_FBK)*denCoeffsTarget[1];
}

/*
 * IirFilter RampUserParameters
 *
 * Slowly ramps old parameters to new parameters to avoid
 * audio artefacts. This method is more expensive but is
 * better for avoiding instability when changing parameters
 *
 */
void IirFilter::RampUserParameters()
{
    // slowly ramp parameters to user inputs to avoid artefacts
    currentCutoff = RAMP_FBK*currentCutoff + (1.0f - RAMP_FBK)*targetCutoff;
    currentGain = RAMP_FBK*currentGain + (1.0f - RAMP_FBK)*targetGain;
    currentQ = RAMP_FBK*currentQ + (1.0f - RAMP_FBK)*targetQ;
    CalculateCoefficients(currentCutoff, currentGain, currentQ);
}

/*
 * IirFilter SetSampleRate
 *
 * Sets sample rate and recalculates coefficients
 *
 */
void IirFilter::SetSampleRate(float fs)
{
    sampleRate = fs;
    CalculateCoefficients(currentCutoff, currentGain, currentQ);
}

/*
 * IirFilter SetCutoff
 *
 * Sets cutoff frequency and recalculates coefficients
 *
 */
void IirFilter::SetCutoff(float frequency)
{
#ifdef USE_COEF_RAMP
    currentCutoff = frequency;
    CalculateCoefficients(currentCutoff, currentGain, currentQ);
#else
    targetCutoff = frequency;
#endif
}

/*
 * IirFilter SetGain
 *
 * Sets passband gain and recalculates coefficients
 *
 */
void IirFilter::SetGain(float gain)
{
#ifdef USE_COEF_RAMP
    currentGain = gain;
    CalculateCoefficients(currentCutoff, currentGain, currentQ);
#else
    targetGain = gain;
#endif
}

/*
 * IirFilter SetQ
 *
 * Sets resonance and recalculates coefficients
 *
 */
void IirFilter::SetQ(float Q)
{
#ifdef USE_COEF_RAMP
    currentQ = Q;
    CalculateCoefficients(currentCutoff, currentGain, currentQ);
#else
    targetQ = Q;
#endif
}
