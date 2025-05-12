# Cortex-M85 Project
### How to run
1. In the terminal, set the ARM GCC toolchain path by entering: 
```set ARM_GCC_TOOLCHAIN_PATH="/opt/homebrew/opt/arm-none-eabi-gcc/bin"```
2. In VScode, select the [GCC 14.2.1 arm-none-eabi] Kit from the status bar, then click `Build` to generate `bench_ra8m1.elf`
![status bar](/status_bar.png) (**If you modify the code, click `Build` again**)
3. In the terminal, execute the following commands:
```
cd build/Debug
ninja
printf "Sleep 2000\nLoadFile bench_ra8m1.elf\ngo\nSleep 3600000" | JLinkExe -if SWD -device R7FA8M1AH -speed auto
```
4. Open a new terminal and run:
```telnet 127.0.0.1 19021```

### Benchmark
| Function   | Avg cycles in  MVE | Avg cycles in C |
|------------|--------------------|-----------------|
| UOV-Is: gf16mat_prod 2048_96  | 1287021 | 2688904 |
| UOV-Is: gf16mat_prod 48_64    | 21075   |   50027 |
| UOV-Is: gf16mat_prod 32_X     | 3848    |    8710 |
| UOV-Ip: gf256mat_prod 1936_68 |  |  |
| UOV-Ip: gf256mat_prod 68_44   |  |  |
| UOV-Ip: gf256mat_prod 44_X    |  |  |

### Reproducing the Error
I'm testing **UOV-Ip: gf256mat_prod 1936_68** on Cortex-M85, but it produces the following error:
![error](/error.png)

### What I've tried
1. Reduce `N_A_VEC_BYTE` to `944`, `960`,`976`, `992`, `1008` after which they ran successfully. <br>

I made the following changes at these 2 files: <br>
In `main.c`:
```
#define N_A_VEC_BYTE 944
```

In `src/mat_vec_mul_gf256_mve.S`:
```
.global gf256mat_prod_1936_68
.type gf256mat_prod_1936_68, %function
gf256mat_prod_1936_68:
    gf256mat_prod 944, 68 // change this line
```

Result:
![](/testpass.png)

2. Reduce `N_A_VEC_BYTE` to `1376`, it fails
3. Haven't tried from `1008` ~ `1376`