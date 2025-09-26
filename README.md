# 🖥️ SIMP Assembler & Simulator

Educational project implementing a simplified **MIPS-like ISA** in C.  
Includes a full **assembler**, **processor simulator**, and **hardware device emulation** (disk, timer, LEDs, 7-seg display, monitor).

---

## 📌 Overview
- **Assembler (`asm/`)**  
  Translates SIMP assembly code into 12-digit hexadecimal machine instructions (`imemin.txt`, `dmemin.txt`).

- **Simulator (`sim/`)**  
  Executes the binary instructions with a **fetch–decode–execute cycle**, supporting:
  - Arithmetic, branching, memory operations  
  - Interrupts (IRQ0–2)  
  - Disk with DMA  
  - LEDs & 7-segment display  
  - Monitor output (text + YUV)

- **Test Programs (`tests/`)**
  | Program    | Purpose |
  |------------|---------|
  | `mulmat`   | Matrix multiplication using MAC |
  | `binom`    | Recursive binomial coefficient |
  | `circle`   | Midpoint circle drawing algorithm |
  | `disktest` | Disk sector shifting with DMA |

- **Documentation (`docs/`)**  
  Contains the official project specification.

---

## ⚙️ Build & Run

### Build
Open the Visual Studio solutions in `asm/` and `sim/`, then build with **x64 / Debug**.

### Run Example (Matrix Multiplication)
From inside `tests/mulmat`:

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
