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

### Option A — CLion (JetBrains, recommended)

1. Open CLion → **File → Open** → select the `power_quality_analyser/` folder.
2. CLion detects `CMakeLists.txt` automatically. Click **Load CMake Project**.
3. Select the `power_quality_analyser` run configuration from the dropdown.
4. Click the **Build** button (hammer icon), then **Run** (play icon).
5. To pass the CSV file, go to **Run → Edit Configurations** and add the full
   path to `power_quality_log.csv` in the **Program arguments** field.

### Option B — Command-line with gcc

```bash
# From inside the power_quality_analyser/ directory:
gcc -std=c99 -Wall -Wextra main.c waveform.c io.c -lm -o power_quality_analyser

# Run (pass CSV as command-line argument):
./power_quality_analyser power_quality_log.csv
```

### Option C — CMake command-line

```bash
mkdir build && cd build
cmake ..
make
./power_quality_analyser ../power_quality_log.csv
```

---

## Expected output

The program prints a summary table to the console and writes `results.txt`:

```
Phase        RMS (V)    Pk-Pk (V)  DC Off (V)  Clipped Status
-----        -------    ---------  ----------  ------- ------
Phase A      229.xxxx   650.xxxx   0.000000          x COMPLIANT
Phase B      229.xxxx   650.xxxx   0.000000          x COMPLIANT
Phase C      229.xxxx   650.xxxx   0.000000          x COMPLIANT
```

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

[https://github.com/NaserAlkheedy/power-quality-analyser](https://github.com/NaserAlkheedy/power-quality-analyser)

---

## Dependencies

- GCC or Clang with C99 support
- CMake 3.15+
- Standard C library + `math.h` (linked with `-lm`)
