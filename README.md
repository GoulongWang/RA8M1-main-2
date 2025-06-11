# Cortex-M85 Project
### Install
- JLink [v7.96h](https://www.segger.com/downloads/jlink/)
- An extension on VSCode called "Cortex Debug"
### How to run
1. In the terminal, set the ARM GCC toolchain path by entering: 
```set ARM_GCC_TOOLCHAIN_PATH="/opt/homebrew/opt/arm-none-eabi-gcc/bin"```
2. In VScode, select the [GCC 14.2.1 arm-none-eabi] Kit from the status bar, then click `Build` to generate `bench_ra8m1.elf`
 (**If you modify the code, click `Build` again**)
3. In the terminal, execute the following commands:
```
cd build/Debug
ninja
printf "Sleep 2000\nLoadFile bench_ra8m1.elf\ngo\nSleep 3600000" | JLinkExe -if SWD -device R7FA8M1AH -speed auto
```
4. Open a new terminal and run:
```telnet 127.0.0.1 19021```

### Benchmark
| Function   | Avg cycles in  MVE | Avg cycles in C | Avg cycles in M4 |
|------------|--------------------|-----------------|------------------|
| UOV-Is: gf16mat_prod 2048_96  | 1287021 | 2688904 |          1018395 |
| UOV-Is: gf16mat_prod 48_64    | 21075   |   48617 |            16098 |
| UOV-Is: gf16mat_prod 32_X     | 3955    |    9033 |             2925 |
| UOV-Is: gf16trimat_2trimat_madd 96_48_64_32| 103863762 |790925336|114613576|
| UOV-Ip: gf256mat_prod 1936_68 | 1188551 | 3088048 |          1397865 |
| UOV-Ip: gf256mat_prod 68_44   | 31962   |  127600 |            33342 |
| UOV-Ip: gf256mat_prod 44_X    | 10511   |   31539 |            11261 |
| UOV-Ip: gf256trimat_2trimat_madd 68_68_44_44 | | | |