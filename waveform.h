// waveform.h
// Power Quality Waveform Analyser
// Author: Naser Alkheedy (23042991)
// UWE 2025-26
//
// Header file for the WaveformSample struct and analysis functions

#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <stdint.h>
#include <stddef.h>   /* size_t */

// constants for analysis
#define NOMINAL_VOLTAGE       230.0    /* UK nominal RMS voltage (V)       */
#define TOLERANCE_LOW         207.0    /* EN 50160: -10% of 230 V          */
#define TOLERANCE_HIGH        253.0    /* EN 50160: +10% of 230 V          */
#define CLIP_THRESHOLD        324.9    /* Sensor hard-limit detection (V)  */
#define NOMINAL_FREQUENCY      50.0    /* UK grid frequency (Hz)           */

// Struct to hold a single row of data from the csv
typedef struct {
    double timestamp;         /* Time of sample (seconds)              */
    double phase_A_voltage;   /* Phase A instantaneous voltage (V)     */
    double phase_B_voltage;   /* Phase B instantaneous voltage (V)     */
    double phase_C_voltage;   /* Phase C instantaneous voltage (V)     */
    double line_current;      /* Line current magnitude (A)            */
    double frequency;         /* Instantaneous frequency estimate (Hz) */
    double power_factor;      /* Power factor (0–1)                    */
    double thd_percent;       /* Total Harmonic Distortion (%)         */
} WaveformSample;

// Struct to hold the results for one phase
typedef struct {
    char   name[16];          /* Phase label e.g. "Phase A"            */
    double rms;               /* RMS voltage (V)                       */
    double peak_to_peak;      /* Peak-to-peak voltage (V)              */
    double dc_offset;         /* Mean voltage / DC offset (V)          */
    int    clipped_count;     /* Number of clipped samples             */
    int    compliant;         /* 1 = within ±10% of 230 V, 0 = not    */
    double std_dev;           /* Standard deviation (V) [Merit ext.]   */
    uint8_t status_flags;     /* Bitmask: bit0=clipping, bit1=OOT      */
} PhaseResults;

/* Status flag bit definitions */
#define FLAG_CLIPPING      (1 << 0)   /* Bit 0: clipping detected       */
#define FLAG_OUT_OF_TOL    (1 << 1)   /* Bit 1: out of tolerance        */

// System wide metrics (not just for one phase)
typedef struct {
    double mean_frequency;    /* Mean frequency over all samples (Hz)  */
    double min_frequency;     /* Minimum frequency in dataset (Hz)     */
    double max_frequency;     /* Maximum frequency in dataset (Hz)     */
    double mean_power_factor; /* Mean power factor                     */
    double min_power_factor;  /* Minimum power factor                  */
    double max_power_factor;  /* Maximum power factor                  */
    double mean_thd;          /* Mean THD (%)                          */
    double min_thd;           /* Minimum THD (%)                       */
    double max_thd;           /* Maximum THD (%)                       */
    int    total_clipped;     /* Total clipped samples across all phase*/
} SystemMetrics;

// function prototypes

// calculate RMS voltage using the pointer to the array
double compute_rms(const WaveformSample *samples, int n, size_t offset);

// calculate peak to peak voltage (max - min)
double compute_peak_to_peak(const WaveformSample *samples, int n, size_t offset);

// calculate the dc offset (mean voltage)
double compute_dc_offset(const WaveformSample *samples, int n, size_t offset);

// count how many samples hit the clipping threshold
int count_clipped(const WaveformSample *samples, int n, size_t offset);

// check if RMS is within 10% of 230V
int check_compliance(double rms);

// calc standard deviation (for Merit extension)
double compute_std_dev(const WaveformSample *samples, int n, size_t offset);

// calculate system metrics like freq and thd
void compute_system_metrics(const WaveformSample *samples, int n, SystemMetrics *metrics);

// run all analyses for a specific phase
void analyse_phase(const WaveformSample *samples, int n, size_t offset,
                   const char *label, PhaseResults *result);

#endif /* WAVEFORM_H */
