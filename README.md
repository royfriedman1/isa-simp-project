# 🖥️ SIMP Assembler & Simulator

Educational project implementing a simplified **MIPS-like ISA** in C.  
Includes a full **assembler**, **processor simulator**, and **hardware device emulation** (disk, timer, LEDs, 7-seg display, monitor).  
Developed as part of the *Computer Organization* course at Tel Aviv University.

---

## 📖 Overview

This project simulates a simplified computer system (SIMP) designed for educational use.  
It consists of two main components:

- **Assembler** – Translates SIMP assembly source files (`.asm`) into machine code (`imemin.txt`, `dmemin.txt`).  
- **Simulator** – Executes the machine code in a **fetch–decode–execute cycle**, updating memory, registers, and hardware devices (I/O).  

Hardware devices supported in the simulation:  
- Timer with interrupts  
- Disk with DMA read/write  
- LEDs and 7-segment display  
- Text/graphic monitor (YUV format)  
- External interrupts (IRQ0–2)  

---

## 📌 Project Structure
- **`asm/` – Assembler**  
  Translates SIMP assembly (`.asm`) into machine code (`imemin.txt`, `dmemin.txt`).

- **`sim/` – Simulator**  
  Executes machine code in a **fetch–decode–execute cycle** and simulates hardware.

- **`tests/` – Example Programs**
  | Program    | Purpose |
  |------------|---------|
  | `mulmat`   | Matrix multiplication (4×4) |
  | `binom`    | Recursive binomial coefficient |
  | `circle`   | Midpoint circle drawing algorithm |
  | `disktest` | Disk sector shifting with DMA |

- **`docs/` – Documentation**  
  Contains the full project specification

---

## ⚡ Quick Start

### 1. Build
Open the Visual Studio solution in `asm/` and in `sim/`.  
Build each project with **x64 / Debug**.  
Binaries will appear in `asm/bin/` and `sim/bin/`.

### 2. Assemble & Simulate
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
- `imemin.txt` – Instruction memory (48-bit lines, 4096 max)
- `dmemin.txt` – Data memory (32-bit words)
- `diskin.txt` – Initial disk state (128 sectors × 512B)
- `irq2in.txt` – IRQ2 activation cycles

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
- `monitor.yuv` – Binary monitor output (YUV, 256×256 grayscale)

---

## 🧩 Instruction Set & Encoding

The SIMP processor uses a **single instruction format** (48 bits wide) for all operations.  
Bit fields are assigned as follows:

```
47:40   39:36   35:32   31:28   27:24   23:12        11:0
opcode   rd      rs      rt      rm     immediate1   immediate2
```

- **opcode** – 8 bits (operation code)  
- **rd, rs, rt, rm** – 4-bit register indices  
- **immediate1** – 12 bits  
- **immediate2** – 12 bits  

---

### Supported Instructions

| Opcode | Mnemonic | Meaning |
|--------|----------|---------|
| 0      | add      | R[rd] = R[rs] + R[rt] + R[rm] |
| 1      | sub      | R[rd] = R[rs] – R[rt] – R[rm] |
| 2      | mac      | R[rd] = R[rs] × R[rt] + R[rm] |
| 3      | and      | R[rd] = R[rs] & R[rt] & R[rm] |
| 4      | or       | R[rd] = R[rs] \| R[rt] \| R[rm] |
| 5      | xor      | R[rd] = R[rs] ^ R[rt] ^ R[rm] |
| 6      | sll      | R[rd] = R[rs] << R[rt] |
| 7      | sra      | R[rd] = R[rs] >> R[rt], arithmetic shift with sign extension |
| 8      | srl      | R[rd] = R[rs] >> R[rt], logical shift |
| 9      | beq      | if (R[rs] == R[rt]) PC = R[rm][11:0] |
| 10     | bne      | if (R[rs] != R[rt]) PC = R[rm][11:0] |
| 11     | blt      | if (R[rs] < R[rt]) PC = R[rm][11:0] |
| 12     | bgt      | if (R[rs] > R[rt]) PC = R[rm][11:0] |
| 13     | ble      | if (R[rs] ≤ R[rt]) PC = R[rm][11:0] |
| 14     | bge      | if (R[rs] ≥ R[rt]) PC = R[rm][11:0] |
| 15     | jal      | R[rd] = PC + 1 ; PC = R[rm][11:0] |
| 16     | lw       | R[rd] = MEM[R[rs] + R[rt]] + R[rm] |
| 17     | sw       | MEM[R[rs] + R[rt]] = R[rm] + R[rd] |
| 18     | reti     | PC = IORegister[7] |
| 19     | in       | R[rd] = IORegister[R[rs] + R[rt]] |
| 20     | out      | IORegister[R[rs] + R[rt]] = R[rm] |
| 21     | halt     | Halt execution, exit simulator |

---

### Hardware I/O Registers

The processor supports I/O through the `in` and `out` instructions, which access hardware registers.  
All registers are initialized to 0 on reset.

| Addr | Name         | Bits | Description |
|------|--------------|------|-------------|
| 0    | irq0enable   | 1    | IRQ0 enable |
| 1    | irq1enable   | 1    | IRQ1 enable |
| 2    | irq2enable   | 1    | IRQ2 enable |
| 3    | irq0status   | 1    | IRQ0 status flag |
| 4    | irq1status   | 1    | IRQ1 status flag |
| 5    | irq2status   | 1    | IRQ2 status flag |
| 6    | irqhandler   | 12   | PC of interrupt handler |
| 7    | irqreturn    | 12   | PC of interrupt return |
| 8    | clks         | 32   | Cycle counter (increments every cycle, rolls over at 0xffffffff) |
| 9    | leds         | 32   | LED outputs (bit i drives LED i) |
| 10   | display7seg  | 32   | 8-digit 7-seg display (4 bits per digit) |
| 11   | timerenable  | 1    | Timer enabled (1) or disabled (0) |
| 12   | timercurrent | 32   | Current timer counter |
| 13   | timermax     | 32   | Maximum timer value |
| 14   | diskcmd      | 2    | 0 = none, 1 = read, 2 = write |
| 15   | disksector   | 7    | Sector number (0–127) |
| 16   | diskbuffer   | 12   | Memory address of buffer (DMA, 128 words) |
| 17   | diskstatus   | 1    | 0 = free, 1 = busy |
| 18–19| reserved     | –    | Reserved |
| 20   | monitoraddr  | 16   | Pixel address in frame buffer |
| 21   | monitordata  | 8    | Pixel luminance (0–255) |
| 22   | monitorcmd   | 1    | 1 = write pixel to monitor |

---

## ⚠️ Limitations & Assumptions

- All instructions execute in **1 cycle** (single-cycle design).  
- No pipelining, hazards, or stalls are simulated.  
- No support for floating-point operations.  
- Interrupts are **not nested** – only one interrupt can be handled at a time.  
- Disk I/O is simplified using DMA transfer of 128 words per sector.  
- The monitor outputs grayscale pixels only (0–255 luminance).  
