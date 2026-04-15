// io.h
// Power Quality Waveform Analyser
// Author: Naser Alkheedy (23042991)
//
// File to handle reading the CSV and saving the results


#ifndef IO_H
#define IO_H

#include "waveform.h"

// loads the csv data into a dynamically allocated array
// make sure to free() the pointer that this returns when done
WaveformSample *load_csv(const char *filepath, int *count);

// saves the calculated metrics to a text file
int write_results(const char *output_path,
                  const PhaseResults phases[3],
                  const SystemMetrics *system,
                  int n_samples);

#endif /* IO_H */
