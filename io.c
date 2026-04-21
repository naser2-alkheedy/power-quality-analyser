// io.c
// Power Quality Waveform Analyser
// Author: Naser Alkheedy (23042991)
//
// Functions to load the CSV file and write the report to a txt file

#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Maximum length for a single CSV line */
#define MAX_LINE_LEN 512

// helper function to count lines so we know how much memory to malloc
static int count_csv_rows(const char *filepath)
{
    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) return -1;

    char line[MAX_LINE_LEN];
    /* Skip header */
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return 0;
    }

    int rows = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        /* Only count non-empty lines */
        if (strlen(line) > 1) rows++;
    }
    fclose(fp);
    return rows;
}

// loads data from the csv file into the WaveformSample struct
// uses a two phase approach: count the lines first, then malloc the space, then parse the data
WaveformSample *load_csv(const char *filepath, int *count)
{
    *count = 0;

    // Phase 1: get row count to malloc the array
    int n_rows = count_csv_rows(filepath);
    if (n_rows <= 0) {
        fprintf(stderr, "ERROR: Could not open or read file '%s'.\n", filepath);
        return NULL;
    }

    // malloc the array
    WaveformSample *samples = (WaveformSample *)malloc(
                                  (size_t)n_rows * sizeof(WaveformSample));
    if (samples == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed for %d samples.\n", n_rows);
        return NULL;
    }

    // Phase 2: reopen and parse the data
    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: Could not re-open file '%s'.\n", filepath);
        free(samples);
        return NULL;
    }

    char line[MAX_LINE_LEN];
    /* Skip the header row */
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        free(samples);
        return NULL;
    }

    int idx = 0;
    while (idx < n_rows && fgets(line, sizeof(line), fp) != NULL) {
        if (strlen(line) <= 1) continue;   /* skip blank lines */

        WaveformSample *s = samples + idx; /* pointer to current struct   */

        /* Parse all 8 fields with sscanf */
        int parsed = sscanf(line,
                            "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",
                            &s->timestamp,
                            &s->phase_A_voltage,
                            &s->phase_B_voltage,
                            &s->phase_C_voltage,
                            &s->line_current,
                            &s->frequency,
                            &s->power_factor,
                            &s->thd_percent);

        if (parsed == 8) {
            idx++;
        } else {
            fprintf(stderr, "WARNING: Skipping malformed row %d (parsed %d fields).\n",
                    idx + 2, parsed);
        }
    }

    fclose(fp);
    *count = idx;
    return samples;
}

// writes everything into a text file
int write_results(const char *output_path,
                  const PhaseResults phases[3],
                  const SystemMetrics *system,
                  int n_samples)
{
    FILE *fp = fopen(output_path, "w");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: Could not open output file '%s'.\n", output_path);
        return -1;
    }

    // print out the report header
    fprintf(fp, "Power Quality Analyser Report\n");
    fprintf(fp, "Author: Naser Alkheedy (23042991)\n");
    fprintf(fp, "Total samples: %d\n", n_samples);
    fprintf(fp, "\n");

    // print per-phase stuff
    for (int i = 0; i < 3; i++) {
        const PhaseResults *p = &phases[i];

        fprintf(fp, "--- %s ---\n", p->name);
        fprintf(fp, "  RMS Voltage      : %.4f V\n", p->rms);
        fprintf(fp, "  Peak-to-Peak     : %.4f V\n", p->peak_to_peak);
        fprintf(fp, "  DC Offset        : %.8f V\n", p->dc_offset);
        fprintf(fp, "  Standard Deviation: %.4f V\n", p->std_dev);
        fprintf(fp, "  Clipped Samples  : %d\n", p->clipped_count);
        fprintf(fp, "  EN 50160 Status  : %s (%.1f V – %.1f V)\n",
                p->compliant ? "COMPLIANT" : "NON-COMPLIANT",
                TOLERANCE_LOW, TOLERANCE_HIGH);

        /* Decode and print bitwise status flags */
        fprintf(fp, "  Status Flags     : 0x%02X  [", p->status_flags);
        fprintf(fp, "bit0(clip)=%d ", (p->status_flags & FLAG_CLIPPING)   ? 1 : 0);
        fprintf(fp, "bit1(oot)=%d",   (p->status_flags & FLAG_OUT_OF_TOL) ? 1 : 0);
        fprintf(fp, "]\n\n");
    }

    // print system metrics
    fprintf(fp, "--- System Metrics ---\n");
    fprintf(fp, "  Total Clipped Samples : %d\n\n", system->total_clipped);

    fprintf(fp, "  Frequency (Hz)\n");
    fprintf(fp, "    Mean  : %.4f Hz  (nominal 50.000 Hz)\n", system->mean_frequency);
    fprintf(fp, "    Min   : %.4f Hz\n", system->min_frequency);
    fprintf(fp, "    Max   : %.4f Hz\n\n", system->max_frequency);

    fprintf(fp, "  Power Factor\n");
    fprintf(fp, "    Mean  : %.4f  (ideal 1.0)\n", system->mean_power_factor);
    fprintf(fp, "    Min   : %.4f\n", system->min_power_factor);
    fprintf(fp, "    Max   : %.4f\n\n", system->max_power_factor);

    fprintf(fp, "  Total Harmonic Distortion (%%)\n");
    fprintf(fp, "    Mean  : %.4f %%  (EN 50160 limit: 8%%)\n", system->mean_thd);
    fprintf(fp, "    Min   : %.4f %%\n", system->min_thd);
    fprintf(fp, "    Max   : %.4f %%\n\n", system->max_thd);

    fprintf(fp, "End of report\n");

    fclose(fp);
    return 0;
}
