# x86 16-Bit Real Mode Cheat Sheet

## ModR/M Byte

8-bit follower for variable-length addressing (after opcode).

Specifies op mode (reg vs. mem), registers, and effective address (EA).

**Bit Structure**:

| 7 6 | 5 4 3 | 2 1 0 |
|-----|-------|-------|
| mod |  reg  | r/m   |

- `mod` (7-6): Addressing mode.
- `reg` (5-3): Register operand.
- `r/m` (2-0): Register or memory mode (EA calc).

## Segment Registers 

### Overview

- **Mode**: 16-bit real mode (8086/8088, emulated in later x86).
- **Memory Model**: Segmented addressing; 1 MB total (2^20 bytes), 64 KB segments (16-byte aligned).
- **Address Calculation**: Physical = `(Segment × 16) + Offset` (both 16-bit values).
- **Segments**: Overlapping possible; no protection (unlike protected mode).
- **Registers**: 4 segment registers (CS, DS, SS, ES); loaded via specific instructions (e.g., `MOV`).

### Memory Addressing

- **Segment**: Base address = `segment << 4` (or `segment * 16`).
- **Offset**: 0x00000xFFFF (64 KB max).
- **Defaults**:
  - Code: `CS:IP`
  - Stack: `SS:SP`
  - Data: `DS:BX/SI/DI`; `ES:DI` (strings).
- **Overrides**: Prefixes like `CS:`, `DS:`, etc. 
  - `MOV AX, CS:[BX]` / `movl %cs:(%bx), %eax`

### Segment Registers Table

| Register      | `reg` bits | Purpose                           | Default Usage                  | Load Notes                                                |
|---------------|------------|-----------------------------------|--------------------------------|-----------------------------------------------------------|
| **ES** - Extra| 000        | Auxiliary data (e.g., strings)    | String operations              | `MOV ES, reg` / `movw %reg, %es`; no core default.        |
| **CS** - Code | 001        | Executable code segment           | `JMP`/`CALL` (far); code fetch | Indirect (via `JMP`/`CALL` only).                         |
| **SS** - Stack| 010        | Stack (calls, interrupts, locals) | `PUSH`/`POP`/`CALL`/`RET`      | `MOV SS, reg` / `movw %reg, %ss`; no immediate IP change. |
| **DS** - Data | 011        | Global/static data (vars)         | General data ops               | `MOV DS, reg` / `movw %reg, %ds`; override w/ prefixes.   |

**Limitations**: No FS/GS (added in 32-bit); segments wrap/overlap.

### ModR/M Byte for Segments

- `reg` (5-3): Segment encoding (for `MOV Sreg, r/m` / `MOV r/m, Sreg`).
- `r/m` (2-0): Other operand (GPR or mem mode).

#### Relevant Opcodes are Only

- 0x8E `MOV Sreg, r/m`
- 0x8C `MOV r/m, Sreg`

## General-Purpose Registers (GPRs)

8 registers (16-bit each); used for arithmetic, data movement, addressing.

**ModR/M Role**: Encodes GPRs in `reg` and `r/m` fields
  - `reg`: Destination/source register
  - `r/m`: Source/destination register/memory-mode

### GPR Encoding Table

| Register | Full Name     | 8-Bit Low/High | `reg` / `r/m` | Order | Common Use            |
|----------|---------------|----------------|---------------|-------|-----------------------|
| **AX**   | Accumulator X | AL/AH          | 000           | 0     | Math, I/O, MUL/DIV.   |
| **CX**   | Counter X     | CL/CH          | 001           | 1     | Loops (LOOP), shifts. |
| **DX**   | Data X        | DL/DH          | 010           | 2     | I/O, MUL/DIV high.    |
| **BX**   | Base X        | BL/BH          | 011           | 3     | Base addressing.      |
| **SP**   | Stack Pointer | (N/A)          | 100           | 4     | Stack ops (PUSH/POP). |
| **BP**   | Base Pointer  | (N/A)          | 101           | 5     | Stack frames.         |
| **SI**   | Source Index  | (N/A)          | 110           | 6     | String source.        |
| **DI**   | Dest Index    | (N/A)          | 111           | 7     | String dest.          |

**Notes**:

- [Disp] = absolute offset (DS:Disp).
- When r/m=101 and mod=00, the effective address is `[BP]` (not `[DI]`). BP and [BP] modes use `SS` by default; override with segment prefixes.
- Displacement modes (mod=01/10) add 8-bit or 16-bit signed offsets to the base address.

### Tips

- **Defaults**: Many ops use AX implicitly (e.g., MUL AX).
- **Index Regs**: SI/DI auto-increment in strings (e.g., MOVSB).
- **Refs**: Avoid direct SP/BP in some arith (flags issues).

## 8-Bit Register Encoding

Some 8 bit instructions have a base primary opcode which an offset must be added to in order to select a different register.

| Register | Byte Type | Decimal Offset | 3-Bit Value (Binary) |
|----------|-----------|----------------|----------------------|
| AL       | Low       | 0              | 000                  |
| CL       | Low       | 1              | 001                  |
| DL       | Low       | 2              | 010                  |
| BL       | Low       | 3              | 011                  |
| AH       | High      | 4              | 100                  |
| CH       | High      | 5              | 101                  |
| DH       | High      | 6              | 110                  |
| BH       | High      | 7              | 111                  |

**Example**: `MOV AH, 0x42`  offset 4, opcode `0xB4`, encoding: `B4 42` (high byte of AX only).

**Note**: Only applicable to the GPRs and not the Index Registers `di` and `si`.

## ModR/M Memory Modes

**Memory Modes**: When mod is `11`, computes EA = Segment:Offset (defaults to DS).

**Real Mode**: 16-bit addresses; disp 8/16-bit; no scaling (unlike 32-bit).

### Mod Field (Addressing Modes)

| `mod` bits | Disp Bytes | Mode Description                             |
|------------|------------|----------------------------------------------|
| 00         | 0          | No displacement; direct mem or reg-indirect. |
| 01         | 1          | 8-bit signed displacement + indirect.        |
| 10         | 2          | 16-bit signed displacement + indirect.       |

### Reg and R/M Fields

The ModR/M byte contains:
- **`reg`**: Selects a register operand (or opcode extension for single-operand instructions).  
- **`r/m`**: Selects the other operand: a register (mod=11) or memory address (mod=00/01/10).

#### Memory Addressing (mod=00/01/10)

The `r/m` field selects from 8 predefined addressing modes:

| r/m | Base | Index | Default Segment |
|-----|------|-------|-----------------|
| 000 | BX   | SI    | DS              |
| 001 | BX   | DI    | DS              |
| 010 | BP   | SI    | SS              |
| 011 | BP   | DI    | SS              |
| 100 |      | SI    | DS              |
| 101 |      | DI    | DS              |
| 110 | BP++ |       | SS              |
| 111 | BX   |       | DS              |

++ - if mod is 00, direct addressing with 2 displacement bytes, no Base, uses DS segment

**Segment Override**: Preceed the opcode to override the default segment register

| Opcode Prefix | Segment Override |
|---------------|------------------|
| 26            | ES               |
| 2E            | CS               |
| 36            | SS               |
| 3E            | DS               |


## x86 16-bit Real Mode: BIOS Handoff State to MBR (at 0x7C00)

Upon BIOS completion (POST, boot device scan), the 512-byte MBR sector is loaded to physical address 0x7C00 and verified (signature 0xAA55 at bytes 510-511).
Control transfers via a **far jump** (e.g., `JMP 0x0000:0x7C00` or `JMP 0x07C0:0x0000`), setting the effective address to 0x7C00.
Registers are **not standardized**assume undefined/random except `DL`.
Interrupts are typically enabled (`IF=1`), so use `CLI` immediately.
The bootloader must initialize segments and stack explicitly.

| Register | BIOS Handoff Value | Notes |
|----------|--------------------|-------|
| IP | 0x7C00 or 0x0000 | From far jump to 0x7C00; e.g., `IP=0x7C00` with `CS=0x0000`, or `IP=0x0000` with `CS=0x07C0`. Normalize with far jump in MBR if needed. |
| CS | 0x0000 or 0x07C0 | Adjusted by BIOS to point to 0x7C00; variations existset explicitly to 0x0000 in MBR for consistency. |
| DS, ES,  | Undefined (often 0x0000) | Not guaranteed; may be non-zeroset to 0x0000 explicitly. |
| SS | Undefined (often 0x0000) | Set explicitly (e.g., `SS=0x0000`); unsafe until `SP` is also set. |
| SP | Undefined/random | Often near/below 0x7C00; set explicitly before stack use (e.g., `SP=0x7BFF` for safety, or `SP=0x9000` for more space). Stack grows downward from SP. |
| AX, BX, CX, DX (excl. DL) SI, DI, BP | Undefined/random | Initialize as needed; no assumptions. |
| DL | Boot device number (e.g., 0x80 for HDD0) | Generally reliable (99%+ cases), but rare BIOS bugs may occur (e.g., 0x00)save immediately (e.g., `PUSH DX`) for `INT 0x13`. |
| FLAGS | Varies (`IF=1` typically) | Interrupts enabled by BIOS; use `CLI` immediately, then `STI` after initialization. Other flags (CF, ZF, DF, etc.) are undefined/random. |

## IBM PC VGA Text Mode Memory Cheat Sheet

### Video Memory Location

| Property              | Value              |
|-----------------------|--------------------|
| Base Address          | `0xB8000` (real mode) |
| Screen Size           | 80 columns × 25 rows (standard) |
| Bytes per Character   | 2                  |

**Memory Offset Formula:**
```
offset = (row * 80 + column) * 2
```

### Character Memory Structure

Each character position requires 2 consecutive bytes:

1. **CHARACTER BYTE** - Contains ASCII code of character to display. Range: `0x00-0xFF` (0-255). Example: `0x41` = 'A', `0x30` = '0', `0x20` = space
2. **ATTRIBUTE BYTE** - Controls foreground color, intensity, background color, and blink

Endian Note: When writing machine code ensure to swap the order for little endian.

### Attribute Byte Bit Layout

**Bit Layout**:

| 7  | 6   | 5   | 4   | 3 | 2   | 1   | 0   |
|----|-----|-----|-----|---|-----|-----|-----|
| BL | BG2 | BG1 | BG0 | I | FG2 | FG1 | FG0 |

| Bits   | Name                  | Description |
|--------|-----------------------|----|
| 0-2    | FG2-FG0               | Foreground color (0-7) |
| 3      | I                     | Foreground intensity (0=normal, 1=bright) |
| 4-6    | BG2-BG0               | Background color (0-7) |
| 7      | B                     | Blink flag (0=normal, 1=blink) |

### Color Palette (8 Colors)

| Value | Name      | Value | Name      | Value | Name      | Value | Name    |
|-------|-----------|-------|-----------|-------|-----------|-------|---------|
|   0   | Black     |   1   | Blue      |   2   | Green     |   3   | Cyan    |
|   4   | Red       |   5   | Magenta   |   6   | Yellow    |   7   | White   |

**INTENSITY:** Add `0x08` to base color value for bright version

Example: `0x02` = Dark Green, `0x0A` = Bright Green

### Common Attribute Byte Examples

| Decimal | Hex    | Foreground Color (bright) | Background Color |
|---------|--------|---------------------------|------------------|
|    7    | `0x07` | White (normal)            | Black            |
|   15    | `0x0F` | White (bright)            | Black            |
|   10    | `0x0A` | Green (bright)            | Black            |
|   12    | `0x0C` | Red (bright)              | Black            |
|   14    | `0x0E` | Yellow (bright)           | Black            |
|   11    | `0x0B` | Cyan (bright)             | Black            |
|   13    | `0x0D` | Magenta (bright)          | Black            |
|    9    | `0x09` | Blue (bright)             | Black            |
|   32    | `0x20` | Black (normal)            | Green            |
|   64    | `0x40` | Black (normal)            | Red              |
|   96    | `0x60` | Black (normal)            | Yellow           |
|  112    | `0x70` | Black (normal)            | White            |
|  135    | `0x87` | White (bright) + BLINK    | Black            |

### Attribute Byte Calculator

**Formula:** `(background << 4) | (intensity << 3) | foreground`

**Example:** Bright green foreground on red background
```
background = 4 (red)
intensity = 1 (bright)
foreground = 2 (green)

Result = (4 << 4) | (1 << 3) | 2 = 0x4A = 74 decimal
```

### Quick Reference Table

To create attribute byte for "Text on Background":

| FG / BG | BLACK | BLUE  | GREEN | CYAN  | RED   | MAGENTA | YELLOW | WHITE |
|---------|-------|-------|-------|-------|-------|---------|--------|-------|
| BLACK   | 0x00  | 0x10  | 0x20  | 0x30  | 0x40  |  0x50   | 0x60   | 0x70  |
| BLUE    | 0x01  | 0x11  | 0x21  | 0x31  | 0x41  |  0x51   | 0x61   | 0x71  |
| GREEN   | 0x02  | 0x12  | 0x22  | 0x32  | 0x42  |  0x52   | 0x62   | 0x72  |
| CYAN    | 0x03  | 0x13  | 0x23  | 0x33  | 0x43  |  0x53   | 0x63   | 0x73  |
| RED     | 0x04  | 0x14  | 0x24  | 0x34  | 0x44  |  0x54   | 0x64   | 0x74  |
| MAGENTA | 0x05  | 0x15  | 0x25  | 0x35  | 0x45  |  0x55   | 0x65   | 0x75  |
| YELLOW  | 0x06  | 0x16  | 0x26  | 0x36  | 0x46  |  0x56   | 0x66   | 0x76  |
| WHITE   | 0x07  | 0x17  | 0x27  | 0x37  | 0x47  |  0x57   | 0x67   | 0x77  |

**Notes:**
- Add `0x08` to foreground color value for bright intensity
- Add `0x80` to enable blinking

### Practical Examples

#### Example 1: Bright White 'A' on Black Background

| Component        | Value        | Description           |
|------------------|--------------|-----------------------|
| Character Byte   | `0x41`       | ASCII 'A'             |
| Attribute Byte   | `0x0F`       | Bright white on black |
| Memory           | [0x41, 0x0F] |                       |

#### Example 2: Blinking Red 'X' on Yellow Background

| Component        | Value        | Description                    |
|------------------|--------------|--------------------------------|
| Character Byte   | `0x58`       | ASCII 'X'                      |
| Attribute Byte   | `0xEC`       | Red (bright) on yellow + blink |
| Memory           | [0x58, 0xEC] |                                |

#### Example 3: Dark Blue '0' on White Background

| Component        | Value        | Description        |
|------------------|--------------|--------------------|
| Character Byte   | `0x30`       | ASCII '0'          |
| Attribute Byte   | `0x71`       | Dark blue on white |
| Memory           | [0x30, 0x71] |                    |

### Memory Writing In Assembly

#### Example 1: Write bright green 'G' at top-left (0,0)

<table>
<tr>
<td width="50%">

**Intel Syntax (IBM):**
```asm
mov ax, 0xB800        ; Video memory segment
mov es, ax
xor di, di            ; Offset 0 (top-left)
mov al, 'G'           ; Character
mov ah, 0x0A          ; Bright green on black
mov word [es:di], ax
```
</td>
<td width="50%">

**AT&T Syntax:**
```gas
movw    $0xB800, %ax    # Video memory segment
movw    %ax, %es
xorw    %di, %di        # Offset 0 (top-left)
movb    $'G', %al       # Character
movb    $0x0A, %ah      # Bright green on black
movw    %ax, %es:(%di)
```
</td>
</tr>
</table>

#### Example 2: Move to position (10, 5)

<table>
<tr>
<td width="50%">

**Intel Syntax (IBM):**
```asm
; Calculate offset: (5 * 80 + 10) * 2 = 820
mov di, (5 * 80 + 10) * 2
mov word [es:di], ax
```
</td>
<td width="50%">

**AT&T Syntax:**
```gas
## Calculate offset: (5 * 80 + 10) * 2 = 820
movw    $(5 * 80 + 10) * 2, %di
movw    %ax, %es:(%di)
```
</td>
</tr>
</table>

#### Example 3: Write string "Hello" starting at (0, 0)

<table>
<tr>
<td width="50%">

**Intel Syntax (IBM):**
```asm
mov ax, 0xB800       ; Video memory segment
mov es, ax
xor di, di           ; Offset 0 (top-left)
mov ah, 0x0F         ; Bright white on black

mov al, 'H'
mov word [es:di], ax
add di, 2

mov al, 'e'
mov word [es:di], ax
add di, 2

mov al, 'l'
mov word [es:di], ax
add di, 2

mov al, 'l'
mov word [es:di], ax
add di, 2

mov al, 'o'
mov word [es:di], ax
```
</td>
<td width="50%">

**AT&T Syntax:**
```gas
movw    $0xB800, %ax    # Video memory segment
movw    %ax, %es
xorw    %di, %di        # Offset 0 (top-left)
movb    $0x0F, %ah      # Bright white on black

movb    $'H', %al
movw    %ax, %es:(%di)
addw    $2, %di

movb    $'e', %al
movw    %ax, %es:(%di)
addw    $2, %di

movb    $'l', %al
movw    %ax, %es:(%di)
addw    $2, %di

movb    $'l', %al
movw    %ax, %es:(%di)
addw    $2, %di

movb    $'o', %al
movw    %ax, %es:(%di)
```
</td>
</tr>
</table>

### Tips

- Video memory address starts at `0xB8000`
- Each character needs 2 bytes: `[ASCII][ATTRIBUTE]`
- Use offset formula: `(row * 80 + col) * 2`
- Foreground colors: bits 0-2, add `0x08` for bright
- Background colors: bits 4-6 (no intensity bit)
- Bit 7 enables blinking
- Standard screen is 80×25 characters
- Memory is directly writable in real mode or protected mode

## BIOS Interrupt Services Cheat Sheet

Check the carry flag (CF) for errors on I/O ops (CF=0 success).
Data from standard references like Ralf Brown's Interrupt List (RBIL) and IBM PC technical docs.

**Boot-time Register States**: At bootloader entry (after BIOS loads sector to 0x7C00): DL=boot drive number (00h=floppy, 80h=first HDD, 81h=second HDD, etc.).
CS:IP typically 0000h:7C00h.
Other registers undefinedinitialize as needed.

Interrupts are invoked with `INT <number>`.

Preserve registers state with stack (don't forget to initialize) if needed before interrupt instruction.

Availability varies by BIOS version/hardware (e.g., AT vs. XT) so see compatibility table in next section.

### INT 10h: Video Services

Controls display modes, text/graphics output, and cursor. Most useful for early console I/O. Includes VESA BIOS Extensions (VBE) for modern graphics.

| AH | AL/Other | Function | Inputs | Outputs | Notes |
|----|----------|----------|--------|---------|-------|
| 00h | AL=mode | Set video mode | AL=mode (e.g., 00h=40x25 text, 13h=320x200x256) | None | Clears screen; modes 013h common. |
| 01h | CH=start, CL=end | Set cursor size | CH=scanline start (031), CL=end (start31) | None | Bits 57 of CH for cursor options (e.g., insert). |
| 02h | BH=page, DL=col, DH=row | Set cursor position | BH=page (07), DX=position | None | Default page 0. |
| 03h | BH=page | Get cursor position | BH=page | DH=row, DL=col, CH=start, CL=end | Returns current cursor. |
| 05h | AL=page | Select active display page | AL=page (07) | None | For text modes only. |
| 06h | AL=lines, BH=attr, CX=upper-left, DX=lower-right | Scroll up | AL=lines to scroll (0=clear), BH=fill attr | None | CX/DX=coords (CH=upper row, CL=left col, etc.). |
| 07h | Same as 06h | Scroll down | Same as 06h | None | Negative scroll effect. |
| 08h | BH=page | Read char/attr at cursor | BH=page | AL=char, AH=attr | Text modes. |
| 09h | AL=char, BH=page, BL=attr, CX=count | Write char + attr | As named | None | No cursor advance. |
| 0Ah | AL=char, BH=page, BL=attr, CX=count | Write char only | As named | None | Attr unchanged. |
| 0Bh | BH=page, BL=color | Set palette | BL=0 (norm), 1 (intens) | None | Sub: 00h=set norm, 01h=set intens. |
| 0Ch | AL=color, BH=page, CX=x, DX=y | Write pixel | AL=color, BH=page, CX=column (x), DX=row (y) (graphics) | None | Modes 46,1113. |
| 0Dh | BH=page, CX=x, DX=y | Read pixel | BH=page, CX=column (x), DX=row (y) | AL=color | Graphics modes. |
| 0Eh | AL=char | Teletype output | AL=char (ASCII) | None | Advances cursor; handles scroll. Best for simple print. |
| 0Fh | BH=page | Get video mode | BH=page | AL=mode, AH=columns | - |
| 10h | - | Palette functions | See sub (e.g., AH=10h, AL=00h=load DAC) | Varies | Advanced: e.g., 01h=select palette. |
| 11h | - | Font loading | E.g., AL=01h=8x8 font load | Varies | For custom chars. |
| 12h | - | Alternate functions | E.g., AL=10h=EGA info | Varies | Hardware-specific. |
| 13h | ES:BP=string, CX=bytes, AL=mode, BH=page, BL=attr, DX=pos | Write string | As named (mode 0=attr, 1=no attr, 2=repeat) | None | AT-compatible; efficient for text. |
| 4Fh | 00h | Get VBE info | ES:DI=VbeInfoBlock (512 bytes) | AL=4Fh, AH=status (00h=success) | Returns VESA capabilities. |
| 4Fh | 01h | Get mode info | CX=mode number, ES:DI=ModeInfoBlock (256 bytes) | AL=4Fh, AH=status | Mode details. |
| 4Fh | 02h | Set VBE mode | BX=mode (bit 15=1: linear framebuffer, bit 14=1: preserve memory) | AL=4Fh, AH=status | Modern graphics modes. |

### INT 11h: Equipment Configuration

| AH | Function | Inputs | Outputs | Notes |
|----|----------|--------|---------|-------|
| - | Get equipment list | None | AX=flags (bit0=floppy,1=math copro,23=video,45=drives,6=keyboard,7=printer,1415=RAM pages) | Single call; e.g., bits 45 = #floppy drives (03). |

### INT 12h: Memory Size

| AH | Function | Inputs | Outputs | Notes |
|----|----------|--------|---------|-------|
| - | Get conventional memory | None | AX=KB (up to 640) | Quick; for stack/heap sizing. |

### INT 13h: Disk Services

Low-level block I/O (CHS or LBA). DL=drive (0x00=floppy0, 0x80=HDD0). Reset with 00h before ops. CH=low 8 bits of cylinder (01023); CL bits 0-5=sector (1-63), bits 6-7=high 2 bits of cylinder; DH=head (0255).

| AH | Function | Inputs | Outputs | Notes |
|----|----------|--------|---------|-------|
| 00h | Reset disk | DL=drive | CF=0 success | Clears errors. |
| 01h | Get status | DL=drive | AH=status (0=OK,1=invalid), CF=1 error | Post-read/write check. |
| 02h | Read sectors | AL=#secs (1128), CH/DH/CL=loc, ES:BX=buf | CF=0, AL=transferred | CHS; max ~8GB. |
| 03h | Write sectors | Same as 02h | Same | Destructive. |
| 04h | Verify sectors | Same as 02h (no buf) | Same | No data transfer. |
| 05h | Format track | DL=drive, CH=track, DH=head, ES:BX=address field buffer | CF=0 | Floppy; ES:BX points to format buffer defining sector layout. |
| 08h | Get params | DL=drive | CH=low 8 bits of max cyl, CL bits 0-5=max sector (1-63), bits 6-7=high 2 bits of max cyl, DH=max head, DL=#drives; ES:DI=EDD params | - |
| 0Fh | Set disk type | AL=media type | None | Floppy format prep. |
| 15h | Get disk type | DL=drive | AH=00h (not present), 01h (floppy no change detect), 02h (floppy with change detect), 03h (hard disk); CX:DX=# 512-byte sectors (for type 03h) | - |
| 16h | Media change? | DL=drive | AH=0 changed,1 no,FF=unsupported | Floppy only. |
| 17h | Set media type | AL=type (as 15h) | None | - |
| 41h | Extensions check | BX=55AAh, DL=drive | BX=AA55h (verify BX changed from 55AAh), AH=vers, CX=flags (bit0=LBA) | EDD support. |
| 42h | Ext read (LBA) | DS:SI=DAP (Disk Addr Packet); DAP structure at DS:SI: Offset 00h: byte - packet size (10h or 18h); 01h: byte - reserved (0); 02h: word - #sectors; 04h: word - buffer offset; 06h: word - buffer segment; 08h: qword - starting LBA | CF=0 | LBA48 in later BIOS. |
| 43h | Ext write | Same as 42h | Same | - |
| 44h | Ext verify | Same as 42h | Same | - |
| 45h | Lock/unlock | DL=drive, AL=0 (unlock) or 1 (lock) | CF=0 | Prevent eject. |
| 46h | Eject media | DL=drive | CF=0 | Removable only. |
| 47h | Ext seek | Same as 42h (no data) | CF=0 | - |
| 48h | Get ext params | DS:SI=DAP (size=18h) | ES:DI=drive info | Size, geometry. |
| 4Eh | Set config | DS:SI=cfg data | CF=0 | Advanced. |

### INT 14h: Serial Port Services

RS-232 comms. Port in DX (0=COM1,1=COM2).

| AH | Function | Inputs | Outputs | Notes |
|----|----------|--------|---------|-------|
| 00h | Init port | DX=port, AL=params (bits 7-5: baud rate, 4-3: parity, 2: stop bits, 1-0: word length). Example: E3h = 9600,N,8,1 | None | Bits: AL lo=word len (3=8bit). |
| 01h | Transmit char | DX=port, AL=char | None | Waits if full. |
| 02h | Receive char | DX=port | AL=char, if none CF=1 | - |
| 03h | Get status | DX=port | AH=modem, AL=line (bits:0=RxRDY,1=TxRDY,2=break,3=overrun,4=parity err,5=framing,6=Tx empty,7=Rx full) | Modem: bits for DTR/DSR etc. |

### INT 15h: System Services

Misc; subfunctions in AH (or full AX for some, e.g., A20). Includes A20 gate control for memory access.

| AH | Function | Inputs | Outputs | Notes |
|----|----------|--------|---------|-------|
| 00h | Cassette (obsolete) | - | - | Rarely used. |
| 4Fh | Keyboard intercept | - | - | For custom keys. |
| 76h | Sys Req | - | - | - |
| 80h8Fh | OEM-specific | Varies | Varies | Vendor hooks. |
| 24h (AX=2400h) | Disable A20 | Set AX=2400h | CF=0 success, AH=status | For <1MB compatibility. |
| 24h (AX=2401h) | Enable A20 | Set AX=2401h | CF=0 success, AH=status | Required for >1MB access. |
| 24h (AX=2403h) | Query A20 status | Set AX=2403h | CF=0, AX=status (0=disabled, 1=enabled) | Check before enable. |
| 86h | Delay | CX:DX=s as 32-bit value (CX=high word, DX=low word) | None | Max ~71 minutes (32-bit s limit: FFFFFFFFh s  4295 seconds). |
| 87h | Block move (up to 1MB) | ES:SI=GDT pointer, CX=word count | CF=0 | For moving data between conventional and extended memory using a GDT structure. |
| 88h | Get ext mem | None | AX=KB (>1MB) | Up to 64MB typical. |
| 89h | Enter protected mode | - | - | BIOS-specific. |
| C0hFFh | System configuration & modern extensions | Varies | Varies | Includes PnP/PCI, pointing device, etc. |
| E801h | Get mem map | - | AX=extended memory 1-16MB (KB), BX=extended memory >16MB (64KB blocks), CX=configured memory 1-16MB (KB), DX=configured memory >16MB (64KB blocks) | Legacy mem query. |
| E820h | Memory map | EAX=0E820h, EDX='SMAP' (534D4150h), ES:DI=buf (20-byte entry), ECX=size (20), EBX=continuation (0 first call) | EAX='SMAP', EBX=continuation value (0 if last entry), ECX=bytes returned, CF=0 | Iter: Call repeatedly with EBX from previous call until EBX=0. EDX must be 'SMAP' (534D4150h) on each call. CF=1 or EAX'SMAP' indicates error/unsupported. Buf has base,len,type (1=usable RAM, 2=reserved, 3=ACPI reclaimable, 4=ACPI NVS, 5=bad memory). Best for modern. |

### INT 16h: Keyboard Services

Waits/polls keys. Returns AL=ASCII, AH=scan.

| AH | Function | Inputs | Outputs | Notes |
|----|----------|--------|---------|-------|
| 00h | Get key (wait) | None | AL=char, AH=scan | Blocks until key. |
| 01h | Check key | None | ZF=0 if key (AL=char, AH=scan), else ZF=1 | Non-blocking. |
| 02h | Get shift status | None | AL=flags (bit0=right shift,1=left,2=ctrl,3=alt,4=scroll,5=num,6=caps,7=insert) | - |
| 05h | Buffer key | AL=char, AH=scan | None | Simulate press. |
| 10h | Ext get key | None | AL=char, AH=scan (ext keys >80h) | Unicode/Ext support. |
| 11h | Ext check | None | As 01h | - |
| 12h | Ext shift | None | As 02h | - |

### INT 17h: Printer Services

Parallel port. DX=printer (0=LPT1).

| AH | Function | Inputs | Outputs | Notes |
|----|----------|--------|---------|-------|
| 00h | Print char | DX=port, AL=char | AH=status (bit0=timeout,13=err,4=selected,5=out empty,6=paper out,7=ack) | - |
| 01h | Init port | DX=port | AH=status | Reset. |
| 02h | Get status | DX=port | AH=status (as above) | - |
| 03h | - | - | - | Unused. |

### INT 18h: User Wait / ROM BASIC

| AH | Function | Inputs | Outputs | Notes |
|----|----------|--------|---------|-------|
| 00h | User wait | None | None | Infinite loop; for BASIC entry. |
| - | Boot BASIC | None | - | Jumps to ROM BASIC if present (e.g., IBM PC). |

### INT 19h: Bootstrap Loader

| AH | Function | Inputs | Outputs | Notes |
|----|----------|--------|---------|-------|
| - | Reboot | None | - | Warm boot; restarts POST/boot sequence. |

### INT 1Ah: Time-of-Day Services

RTC access (AT+).

| AH | Function | Inputs | Outputs | Notes |
|----|----------|--------|---------|-------|
| 00h | Get time | None | CX:DX = tick count (ticks since midnight at 18.2 Hz) | - |
| 01h | Set time | CX:DX = tick count to set (ticks since midnight at 18.2 Hz) | None | - |
| 02h | Get RTC time | None | CH=hr, CL=min, DH=sec (BCD) | 24hr format. |
| 03h | Set RTC time | As 02h | CF=0 | - |
| 04h | Get RTC date | None | CH=month, CL=day, DH=year (BCD, year=099) | - |
| 05h | Set RTC date | As 04h | CF=0 | - |
| 06h | Set alarm | As 02h | CF=0 | IRQ 4Fh trigger. |
| 07h | Reset alarm | None | None | - |

### Other Notable Interrupts

- **INT 05h**: Print screen (Shift-PrtSc).
- **INT 09h**: Hardware keyboard IRQ (low-level; avoid in software).
- **INT 1Bh**: Ctrl-Break handler (hook for abort).
- **INT 1Ch**: Timer tick (18.2 Hz; hook for delays).
- **INT 1Dh**: Video param table (seg addr in IVT).
- **INT 1Eh**: Disk param table (seg addr).
- **INT 1Fh**: Graphics char table (seg addr).

For full details (e.g., rare subfunctions), consult RBIL. Test on emulators like QEMU for consistency. These enable hardware access without drivers in early boot.

## BIOS Interrupt Compatibility Table

### By System Generation

| Interrupt/Function | IBM PC (1981) | IBM XT (1983) | IBM AT (1984) | PS/2 (1987) | Modern BIOS | Notes |
|-------------------|---------------|---------------|---------------|-------------|-------------|-------|
| **INT 10h (Video)** |
| AH=00h-0Fh | X | X | X | X | X | Core functions universal |
| AH=10h (Palette) | | | EGA | X | X | Requires EGA/VGA |
| AH=11h (Font) | | | EGA | X | X | Requires EGA/VGA |
| AH=12h (Alt func) | | | EGA | X | X | Requires EGA/VGA |
| AH=13h (Write str) | | | X | X | X | AT+ only |
| AH=4Fh (VBE) | | | | | 1990s+ | VESA BIOS Ext required |
| **INT 11h (Equipment)** | X | X | X | X | X | Universal |
| **INT 12h (Memory)** | X | X | X | X | X | Universal |
| **INT 13h (Disk)** |
| AH=00h-05h | X | X | X | X | X | Core CHS functions |
| AH=08h (Get params) | Partial | X | X | X | X | Limited on PC |
| AH=15h (Get type) | | | X | X | X | AT+ only |
| AH=41h-48h (Ext/LBA) | | | | | 1994+ | Phoenix EDD spec |
| **INT 14h (Serial)** | X | X | X | X | X | Universal |
| **INT 15h (System)** |
| AH=00h (Cassette) | X | X | | | | PC/XT only |
| AH=4Fh (Kbd intercept) | | | X | X | X | AT+ |
| AH=86h (Delay) | | | X | X | X | AT+ |
| AH=87h (Block move) | | | X | X | X | AT+ (286+) |
| AH=88h (Ext mem) | | | X | X | X | AT+ (286+) |
| AH=89h (Prot mode) | | | X | X | Rare | AT+, BIOS-specific |
| AX=2400h-2403h (A20) | | | | X | X | PS/2+ |
| AH=C0h (Sys config) | | | X | X | X | AT+ |
| AH=E801h (Mem map) | | | | | 1990s+ | Phoenix spec |
| AH=E820h (Mem map) | | | | | 1995+ | ACPI spec, best method |
| **INT 16h (Keyboard)** |
| AH=00h-02h | X | X | X | X | X | Universal |
| AH=05h (Buffer key) | | | X | X | X | AT+ |
| AH=10h-12h (Extended) | | | | X | X | PS/2+ (101-key kbd) |
| **INT 17h (Printer)** | X | X | X | X | X | Universal |
| **INT 1Ah (RTC)** |
| AH=00h-01h (Tick) | X | X | X | X | X | Universal |
| AH=02h-07h (RTC) | | | X | X | X | AT+ (MC146818 RTC) |

### Key Hardware Requirements

| Feature | Minimum Hardware | Year Introduced |
|---------|-----------------|-----------------|
| CHS disk access | Floppy/HDD controller | 1981 (PC) |
| Hard disk support | XT controller | 1983 |
| Extended memory access | 286+ CPU | 1984 (AT) |
| EGA/VGA functions | EGA/VGA card | 1984/1987 |
| 101-key keyboard | Enhanced keyboard | 1987 (PS/2) |
| A20 gate control | PS/2 keyboard controller | 1987 |
| LBA disk access | Phoenix BIOS extensions | 1994 |
| VESA VBE | VESA BIOS | 1989+ |
| E820 memory map | ACPI-aware BIOS | 1995+ |

### Quick Compatibility Guide

**For maximum compatibility (PC/XT/AT):**
- INT 10h: AH=00h-0Fh (avoid 10h-13h, 4Fh)
- INT 13h: AH=00h-05h (CHS only)
- INT 16h: AH=00h-02h only
- INT 1Ah: AH=00h-01h (tick count, not RTC)

**For AT-compatible systems (1984+):**
- Add: INT 10h AH=10h-13h (EGA/VGA)
- Add: INT 13h AH=08h, 15h (reliable drive params)
- Add: INT 15h AH=86h-88h (delay, extended memory)
- Add: INT 1Ah AH=02h-07h (RTC access)

**For modern bootloaders (1995+):**
- Use: INT 15h E820h for memory detection
- Use: INT 13h AH=41h-48h for LBA disk access
- Use: INT 10h AH=4Fh for VESA graphics
- Use: A20 gate (AX=2401h) before accessing >1MB