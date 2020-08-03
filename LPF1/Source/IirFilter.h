//
//  IirFilter.h
//  LPF1
//
//  Created by Sean Wells-Rutherford on 11/7/19.
//

#ifndef IirFilter_h
#define IirFilter_h

#define USE_PARAM_RAMP
//#define USE_COEF_RAMP

class IirFilter
{
public:
    IirFilter();
    IirFilter(int framesize, float samplerate);
    ~IirFilter();
    
    void Process(const float* input, float* output);
    
    void RampCoefficients();
    void RampUserParameters();
    
    inline void ToggleEnable() { enable  = !enable; };
    inline void SetFrameSize(int frameSz) { frameSize = frameSz; };
    
    void SetSampleRate(float fs);
    void SetCutoff(float frequency);
    void SetGain(float gain);
    void SetQ(float Q);
    
private:
    void CalculateCoefficients(float cutoffFreq, float leveldB, float resonance);

    float numCoeffsCurrent[3];  // feed-forward coefficients currently in use
    float numCoeffsTarget[3];   // feed-forward coefficients calculated after user input
    float denCoeffsCurrent[2];  // feedback coefficients currently in use
    float denCoeffsTarget[2];   // feedback coefficients calculated after user input
    float states[2];            // IIR filter implemented as Direct Form II to use less memory
                                // In floating point no need to worry about overflow
    int frameSize;              // number of samples per frame
    
    bool enable;
    
    float sampleRate;
    float currentCutoff;
    float targetCutoff;
    float currentGain;
    float targetGain;
    float currentQ;
    float targetQ;
    
    const float RAMP_FBK;       // smoothing coefficient when user changes parameters
};

#endif /* IirFilter_h */
