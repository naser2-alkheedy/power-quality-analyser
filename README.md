# Power Quality Waveform Analyser

**Module:** UGMFGT-15-1 Programming for Engineers  
**Author:** Naser Alkheedy  
**Student ID:** 23042991  
**University:** University of the West of England, 2025–26

---

## What this program does

Reads a 3-phase industrial power quality log file (`power_quality_log.csv`),
computes voltage metrics for each phase, detects anomalies (clipping, tolerance
violations), and writes a structured plain-text report to `results.txt`.

---

## How to Build and Run


### Command-line with gcc

```bash
# From inside the power_quality_analyser/ directory:
gcc -std=c99 -Wall -Wextra main.c waveform.c io.c -lm -o power_quality_analyser

# Run (pass CSV as command-line argument):
./power_quality_analyser power_quality_log.csv
```



---

## Expected output

The program prints a summary table to the console and writes `results.txt`:



---

## File structure

```
power_quality_analyser/
├── main.c          Entry point; handles args and orchestrates calls
├── waveform.c      All analysis functions (RMS, peak-to-peak, etc.)
├── waveform.h      WaveformSample struct definition + prototypes
├── io.c            CSV loading and results file writing
├── io.h            I/O function prototypes
├── CMakeLists.txt  CMake build configuration
├── README.md       This file
└── logbook.docx    Development logbook
```

---

## GitHub Repository

[https://github.com/naser2-alkheedy/power-quality-analyser](https://github.com/naser2-alkheedy/power-quality-analyser)

---

## Dependencies

- GCC or Clang with C99 support
- CMake 3.15+
- Standard C library + `math.h` (linked with `-lm`)
