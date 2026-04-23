// main.c
// Power Quality Waveform Analyser
// Author: Naser Alkheedy (23042991)
//
// Entry point. Handles args and calls the other functions.
//
// gcc -std=c99 -Wall -Wextra main.c waveform.c io.c -lm -o power_quality_analyser


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>   /* offsetof */

#include "waveform.h"
#include "io.h"

#define OUTPUT_FILE "results.txt"

int main(int argc, char *argv[])
{
    // check command line args
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <csv_filepath>\n", argv[0]);
        fprintf(stderr, "Example: %s power_quality_log.csv\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_file = argv[1];

    printf("Power Quality Waveform Analyser\n");
    printf("Naser Alkheedy (23042991)\n");
    printf("---\n");
    printf("Loading: %s\n", input_file);

    // load the csv into dynamically allocated array
    int n_samples = 0;
    WaveformSample *samples = load_csv(input_file, &n_samples);

    if (samples == NULL || n_samples == 0) {
        fprintf(stderr, "ERROR: Failed to load data from '%s'.\n", input_file);
        return EXIT_FAILURE;
    }

    printf("Loaded %d samples successfully.\n\n", n_samples);

    // analyse each phase
    // offsetof gets the byte offset so we can pass it to the generic function
    PhaseResults phases[3];

    analyse_phase(samples, n_samples,
                  offsetof(WaveformSample, phase_A_voltage),
                  "Phase A", &phases[0]);

    analyse_phase(samples, n_samples,
                  offsetof(WaveformSample, phase_B_voltage),
                  "Phase B", &phases[1]);

    analyse_phase(samples, n_samples,
                  offsetof(WaveformSample, phase_C_voltage),
                  "Phase C", &phases[2]);

    // get the system metrics
    SystemMetrics system;
    compute_system_metrics(samples, n_samples, &system);

    // add up clipped samples for all phases
    system.total_clipped = phases[0].clipped_count
                         + phases[1].clipped_count
                         + phases[2].clipped_count;

    // print summary to console
    printf("Phase | RMS (V) | Pk-Pk (V) | DC Off (V) | Clipped | Status\n");

    for (int i = 0; i < 3; i++) {
        printf("%s | %.4f | %.4f | %.6f | %d | %s\n",
               phases[i].name,
               phases[i].rms,
               phases[i].peak_to_peak,
               phases[i].dc_offset,
               phases[i].clipped_count,
               phases[i].compliant ? "COMPLIANT" : "NOT COMPLIANT");
    }

    printf("\nSystem Metrics:\n");
    printf("  Mean Frequency   : %.4f Hz\n", system.mean_frequency);
    printf("  Mean Power Factor: %.4f\n",    system.mean_power_factor);
    printf("  Mean THD         : %.4f %%\n", system.mean_thd);
    printf("  Total Clipped    : %d samples\n\n", system.total_clipped);

    // write out to the text file
    int write_ok = write_results(OUTPUT_FILE, phases, &system, n_samples);
    if (write_ok == 0) {
        printf("Results written to: %s\n", OUTPUT_FILE);
    } else {
        fprintf(stderr, "WARNING: Could not write results file.\n");
    }

    // free the malloc memory
    free(samples);
    samples = NULL; // just to be safe

    printf("Done. Memory freed.\n");
    return EXIT_SUCCESS;
}
