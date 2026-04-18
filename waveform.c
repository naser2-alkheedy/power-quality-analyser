// waveform.c
// Power Quality Waveform Analyser
// Author: Naser Alkheedy (23042991)
//
// All the math functions for the waveform analysis are here.
// I used pointer math so I don't have to copy-paste the same function for each phase.

#include "waveform.h"

#include <math.h>
#include <string.h>
#include <stdio.h>

// helper function to get voltage using pointer offset
static inline double get_voltage(const WaveformSample *s, size_t offset)
{
    /* Treat the struct's base address as raw bytes, step forward by 'offset'
       bytes, then interpret that location as a double. */
    return *((const double *)((const char *)s + offset));
}

// compute RMS voltage
// squares the samples, gets the mean, then takes the square root
double compute_rms(const WaveformSample *samples, int n, size_t offset)
{
    double sum_sq = 0.0;
    const WaveformSample *ptr = samples;           /* pointer to current sample */

    for (int i = 0; i < n; i++, ptr++) {
        double v = get_voltage(ptr, offset);
        sum_sq += v * v;                           /* accumulate v^2            */
    }
    return sqrt(sum_sq / (double)n);
}

// compute peak to peak (max minus min)
double compute_peak_to_peak(const WaveformSample *samples, int n, size_t offset)
{
    const WaveformSample *ptr = samples;
    double v_max = get_voltage(ptr, offset);
    double v_min = v_max;

    ptr++;                                         /* advance past first sample */
    for (int i = 1; i < n; i++, ptr++) {
        double v = get_voltage(ptr, offset);
        if (v > v_max) v_max = v;
        if (v < v_min) v_min = v;
    }
    return v_max - v_min;
}

// compute the dc offset (just the mean of the voltage)
double compute_dc_offset(const WaveformSample *samples, int n, size_t offset)
{
    double sum = 0.0;
    const WaveformSample *ptr = samples;

    for (int i = 0; i < n; i++, ptr++) {
        sum += get_voltage(ptr, offset);
    }
    return sum / (double)n;
}

// count how many times the voltage hits the threshold limit
int count_clipped(const WaveformSample *samples, int n, size_t offset)
{
    int count = 0;
    const WaveformSample *ptr = samples;

    for (int i = 0; i < n; i++, ptr++) {
        double v = get_voltage(ptr, offset);
        if (v >= CLIP_THRESHOLD || v <= -CLIP_THRESHOLD) {
            count++;
        }
    }
    return count;
}

// checks if rms is between 207V and 253V
int check_compliance(double rms)
{
    return (rms >= TOLERANCE_LOW && rms <= TOLERANCE_HIGH) ? 1 : 0;
}

// calculate std dev using a two pass algorithm
// first get the mean, then do the variance
double compute_std_dev(const WaveformSample *samples, int n, size_t offset)
{
    /* Pass 1: compute mean */
    double mean = compute_dc_offset(samples, n, offset);

    /* Pass 2: accumulate squared deviations */
    double sum_sq_dev = 0.0;
    const WaveformSample *ptr = samples;

    for (int i = 0; i < n; i++, ptr++) {
        double dev = get_voltage(ptr, offset) - mean;
        sum_sq_dev += dev * dev;
    }
    return sqrt(sum_sq_dev / (double)n);
}

// get min max and mean for frequency, PF, and THD
void compute_system_metrics(const WaveformSample *samples, int n, SystemMetrics *metrics)
{
    double sum_freq = 0.0, sum_pf = 0.0, sum_thd = 0.0;
    double min_freq, max_freq, min_pf, max_pf, min_thd, max_thd;

    /* Initialise min/max from first sample */
    const WaveformSample *ptr = samples;
    min_freq = max_freq = ptr->frequency;
    min_pf   = max_pf   = ptr->power_factor;
    min_thd  = max_thd  = ptr->thd_percent;

    for (int i = 0; i < n; i++, ptr++) {
        sum_freq += ptr->frequency;
        sum_pf   += ptr->power_factor;
        sum_thd  += ptr->thd_percent;

        if (ptr->frequency    < min_freq) min_freq = ptr->frequency;
        if (ptr->frequency    > max_freq) max_freq = ptr->frequency;
        if (ptr->power_factor < min_pf)   min_pf   = ptr->power_factor;
        if (ptr->power_factor > max_pf)   max_pf   = ptr->power_factor;
        if (ptr->thd_percent  < min_thd)  min_thd  = ptr->thd_percent;
        if (ptr->thd_percent  > max_thd)  max_thd  = ptr->thd_percent;
    }

    metrics->mean_frequency    = sum_freq / (double)n;
    metrics->min_frequency     = min_freq;
    metrics->max_frequency     = max_freq;
    metrics->mean_power_factor = sum_pf   / (double)n;
    metrics->min_power_factor  = min_pf;
    metrics->max_power_factor  = max_pf;
    metrics->mean_thd          = sum_thd  / (double)n;
    metrics->min_thd           = min_thd;
    metrics->max_thd           = max_thd;
}

// helper to run everything for one phase
void analyse_phase(const WaveformSample *samples, int n, size_t offset,
                   const char *label, PhaseResults *result)
{
    strncpy(result->name, label, sizeof(result->name) - 1);
    result->name[sizeof(result->name) - 1] = '\0';

    result->rms           = compute_rms(samples, n, offset);
    result->peak_to_peak  = compute_peak_to_peak(samples, n, offset);
    result->dc_offset     = compute_dc_offset(samples, n, offset);
    result->clipped_count = count_clipped(samples, n, offset);
    result->compliant     = check_compliance(result->rms);
    result->std_dev       = compute_std_dev(samples, n, offset);

    /* Build bitwise status flags */
    result->status_flags = 0;
    if (result->clipped_count > 0)  result->status_flags |= FLAG_CLIPPING;
    if (!result->compliant)         result->status_flags |= FLAG_OUT_OF_TOL;
}
