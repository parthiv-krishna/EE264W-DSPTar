

#include <Arduino.h>
#include "eq.h"
#include "utility/dspinst.h"

void EQ::update(void)
{
    if (!_setup) {
        return; // not ready
    }

    audio_block_t *block;
    int32_t b0, b1, b2, a1, a2, sum;
    uint32_t in2, out2, bprev, aprev, flag;
    uint32_t *data, *end;
    int32_t *state;
    block = receiveWritable();
    if (!block) {
        return; // did not receive an allocated block
    }
    end = (uint32_t *)(block->data) + AUDIO_BLOCK_SAMPLES/2;
    state = (int32_t *)_coeffs;
    do {
        b0 = *state++;
        b1 = *state++;
        b2 = *state++;
        a1 = *state++;
        a2 = *state++;
        bprev = *state++;
        aprev = *state++;
        sum = *state & 0x3FFF;
        data = end - AUDIO_BLOCK_SAMPLES/2;
        do {
            in2 = *data;
            sum = signed_multiply_accumulate_32x16b(sum, b0, in2);
            sum = signed_multiply_accumulate_32x16t(sum, b1, bprev);
            sum = signed_multiply_accumulate_32x16b(sum, b2, bprev);
            sum = signed_multiply_accumulate_32x16t(sum, a1, aprev);
            sum = signed_multiply_accumulate_32x16b(sum, a2, aprev);
            out2 = signed_saturate_rshift(sum, 16, 14);
            sum &= 0x3FFF;
            sum = signed_multiply_accumulate_32x16t(sum, b0, in2);
            sum = signed_multiply_accumulate_32x16b(sum, b1, in2);
            sum = signed_multiply_accumulate_32x16t(sum, b2, bprev);
            sum = signed_multiply_accumulate_32x16b(sum, a1, out2);
            sum = signed_multiply_accumulate_32x16t(sum, a2, aprev);
            bprev = in2;
            aprev = pack_16b_16b(
                signed_saturate_rshift(sum, 16, 14), out2);

            // "first order noise shaping":
            // http://www.earlevel.com/main/2003/02/28/biquads/

            sum &= 0x3FFF;
            bprev = in2;
            *data++ = aprev;
        } while (data < end);
        flag = *state & 0x80000000;
        *state++ = sum | flag;
        *(state-2) = aprev;
        *(state-3) = bprev;
    } while (flag);
    transmit(block);
    release(block);
}

void EQ::setCoefficients(uint32_t stage, const int *coefficients)
{
    if (stage >= 4) {
        return; // only upto 4 stages
    }
    int32_t *dest = _coeffs + stage * 8;
    _setup = false; // "lock"
    if (stage > 0) {
        dest[-1] |= 0x80000000;
    }
    for (int i = 0; i < 3; i++) {
        dest[i] = coefficients[i];
    }
    for (int i = 3; i < 5; i++) {
        dest[i] = -1 * coefficients[i];
    }
    dest[6] &= 0x80000000; // flags
    _setup = true;
}

void EQ::setCoefficients(uint32_t stage, const double *coefficients) {
    int coeff[5];
    coeff[0] = coefficients[0] * TWO_TO_THE_30;
    coeff[1] = coefficients[1] * TWO_TO_THE_30;
    coeff[2] = coefficients[2] * TWO_TO_THE_30;
    coeff[3] = coefficients[3] * TWO_TO_THE_30;
    coeff[4] = coefficients[4] * TWO_TO_THE_30;
    setCoefficients(stage, coeff);
}



// Common filter types from Audio EQ Cookbook
// https://www.w3.org/TR/audio-eq-cookbook/ 
void setLowpass(uint32_t stage, float frequency, float q = SQRT2_OVER_2) {
    int coeff[5];
    double w0 = frequency * (2.0f * PI / AUDIO_SAMPLE_RATE_EXACT);
    double sinW0 = sin(w0);
    double alpha = sinW0 / ((double)q * 2.0);
    double cosW0 = cos(w0);
    double scale = TWO_TO_THE_30 / (1.0 + alpha);
    coeff[0] = ((1.0 - cosW0) / 2.0) * scale;    // b0
    coeff[1] = (1.0 - cosW0) * scale;            // b1
    coeff[2] = coeff[0];                         // b2
    coeff[3] = (-2.0 * cosW0) * scale;           // a1
    coeff[4] = (1.0 - alpha) * scale;            // a2
    setCoefficients(stage, coef);
}

void setHighpass(uint32_t stage, float frequency, float q = SQRT2_OVER_2) {
    int coeff[5];
    double w0 = frequency * (2.0f * PI / AUDIO_SAMPLE_RATE_EXACT);
    double sinW0 = sin(w0);
    double alpha = sinW0 / ((double)q * 2.0);
    double cosW0 = cos(w0);
    double scale = TWO_TO_THE_30 / (1.0 + alpha);
    coeff[0] = ((1.0 + cosW0) / 2.0) * scale;    // b0
    coeff[1] = -(1.0 + cosW0) * scale;           // b1
    coeff[2] = coeff[0];                         // b2
    coeff[3] = (-2.0 * cosW0) * scale;           // a1
    coeff[4] = (1.0 - alpha) * scale;            // a2
    setCoefficients(stage, coef);
}

void setBandpass(uint32_t stage, float frequency, float q = 1.0) {
    int coeff[5];
    double w0 = frequency * (2.0f * PI / AUDIO_SAMPLE_RATE_EXACT);
    double sinW0 = sin(w0);
    double alpha = sinW0 / ((double)q * 2.0);
    double cosW0 = cos(w0);
    double scale = TWO_TO_THE_30 / (1.0 + alpha);
    coeff[0] = alpha * scale;                    // b0
    coeff[1] = 0;                                // b1
    coeff[2] = (-alpha) * scale;                 // b2
    coeff[3] = (-2.0 * cosW0) * scale;           // a1
    coeff[4] = (1.0 - alpha) * scale;            // a2
    setCoefficients(stage, coef);
}

void setNotch(uint32_t stage, float frequency, float q = 1.0) {
    int coeff[5];
    double w0 = frequency * (2.0f * PI / AUDIO_SAMPLE_RATE_EXACT);
    double sinW0 = sin(w0);
    double alpha = sinW0 / ((double)q * 2.0);
    double cosW0 = cos(w0);
    double scale = TWO_TO_THE_30 / (1.0 + alpha);
    coeff[0] = scale;                            // b0
    coeff[1] = (-2.0 * cosW0) * scale;           // b1
    coeff[2] = coeff[0];                         // b2
    coeff[3] = (-2.0 * cosW0) * scale;           // a1
    coeff[4] = (1.0 - alpha) * scale;            // a2
    setCoefficients(stage, coef);
}

void setLowShelf(uint32_t stage, float frequency, float gain, float slope = 1.0f) {
    int coeff[5];
    double a = pow(10.0, gain/40.0f);
    double w0 = frequency * (2.0f * PI / AUDIO_SAMPLE_RATE_EXACT);
    double sinW0 = sin(w0);
    double cosW0 = cos(w0);
    
    // intermediate values
    double sinsq = sinW0 * sqrt( (pow(a,2.0)+1.0)*(1.0/(double)slope-1.0)+2.0*a );
    double aMinus = (a-1.0)*cosW0;
    double aPlus = (a+1.0)*cosW0;

    double scale = TWO_TO_THE_30 / ( (a+1.0) + aMinus + sinsq);
    coeff[0] = a * ((a+1.0) - aMinus + sinsq) * scale;         // b0
    coeff[1] = 2.0 * a * ((a-1.0) - aPlus) * scale;            // b1
    coeff[2] = a * ((a+1.0) - aMinus - sinsq) * scale;         // b2
    coeff[3] = -2.0 * ((a-1.0) + aPlus) * scale;               // a1
    coeff[4] = ((a+1.0) + aMinus - sinsq) * scale;             // a2
    setCoefficients(stage, coef);
}

void setHighShelf(uint32_t stage, float frequency, float gain, float slope = 1.0f) {
    int coeff[5];
    double a = pow(10.0, gain/40.0f);
    double w0 = frequency * (2.0f * PI / AUDIO_SAMPLE_RATE_EXACT);
    double sinW0 = sin(w0);
    double cosW0 = cos(w0);

    // intermediate values 
    double sinsq = sinW0 * sqrt( (pow(a,2.0)+1.0)*(1.0/(double)slope-1.0)+2.0*a );
    double aMinus = (a-1.0)*cosW0;
    double aPlus = (a+1.0)*cosW0;

    double scale = TWO_TO_THE_30 / ( (a+1.0) - aMinus + sinsq);
    coeff[0] = a * ((a+1.0) + aMinus + sinsq) * scale;      // b0
    coeff[1] = -2.0 * a * ((a-1.0) + aPlus) * scale;        // b1
    coeff[2] = a * ((a+1.0) + aMinus - sinsq) * scale;      // b2
    coeff[3] = 2.0 * ((a-1.0) - aPlus) * scale;             // a1
    coeff[4] = ((a+1.0) - aMinus - sinsq) * scale;          // a2
    setCoefficients(stage, coef);
}
