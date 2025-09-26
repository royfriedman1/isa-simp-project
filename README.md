# 🖥️ SIMP Assembler & Simulator

Educational project implementing a simplified **MIPS-like ISA** in C.  
Includes a full **assembler**, **processor simulator**, and **hardware device emulation** (disk, timer, LEDs, 7-seg display, monitor).  
Developed as part of the *Computer Organization* course at Tel Aviv University.

---

## 📌 Project Structure
- **`asm/` – Assembler**  
  Translates SIMP assembly (`.asm`) into machine code (`imemin.txt`, `dmemin.txt`).

- **`sim/` – Simulator**  
  Executes machine code in a **fetch–decode–execute cycle** and simulates hardware:
  - Arithmetic, memory, and branching instructions  
  - Interrupts (IRQ0–2)  
  - Disk with DMA  
  - LEDs & 7-segment display  
  - Monitor output (text + YUV)  

- **`tests/` – Example Programs**
  | Program    | Purpose |
  |------------|---------|
  | `mulmat`   | Matrix multiplication with MAC |
  | `binom`    | Recursive binomial coefficient |
  | `circle`   | Midpoint circle drawing algorithm |
  | `disktest` | Disk sector shifting with DMA |

- **`docs/` – Documentation**  
  Contains the full project specification:  
[ISA Project Documentation (PDF)](ISA%20Project%20Documentation.pdf)

---

## ⚡ Quick Start

### 1. Build
Open the Visual Studio solution in `asm/` and in `sim/`.  
Build each project with **x64 / Debug**.  
Binaries will appear in `asm/bin/` and `sim/bin/`.

### 2. Assemble a Program
From the folder of your program (e.g., `tests/mulmat`):


```bat
..\..\asm\bin\asm.exe mulmat.asm imemin.txt dmemin.txt
..\..\sim\bin\sim.exe imemin.txt dmemin.txt diskin.txt irq2in.txt ^
  dmemout.txt regout.txt trace.txt hwregtrace.txt cycles.txt leds.txt ^
  display7seg.txt diskout.txt monitor.txt monitor.yuv
```

---

## 📂 Input & Output Files

**Inputs**
- `imemin.txt` – Instruction memory
- `dmemin.txt` – Data memory
- `diskin.txt` – Disk contents
- `irq2in.txt` – IRQ2 event timings

**Outputs**
- `dmemout.txt` – Final data memory
- `regout.txt` – Registers R3–R15
- `trace.txt` – Instruction trace
- `hwregtrace.txt` – HW register activity
- `cycles.txt` – Cycle count
- `leds.txt` – LED changes
- `display7seg.txt` – 7-seg display log
- `diskout.txt` – Final disk contents
- `monitor.txt` – Monitor pixels (hex)
- `monitor.yuv` – Monitor output (binary)
