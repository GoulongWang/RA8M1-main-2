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

### Reproducing the Error
1. If I set the following definitions, it produces nothing as shown in the picture below:
```
#define N_A_VEC_BYTE 2048
#define N_A_WIDTH    96   
```

In `src/mat_vec_mul_gf16_mve.S`
```
.global gf16mat_prod_2048_96
.type gf16mat_prod_2048_96, %function
gf16mat_prod_2048_96:
    gf16mat_prod 2048, 96
```
![nothing](/nothing.png)

2. When I reduce `N_A_VEC_BYTE` to 512
```
#define N_A_VEC_BYTE 512
#define N_A_WIDTH    96   
```

In `src/mat_vec_mul_gf16_mve.S`
```
.global gf16mat_prod_2048_96
.type gf16mat_prod_2048_96, %function
gf16mat_prod_2048_96:
    gf16mat_prod 512, 96
```
It produces `TEST PASS.`

![testpass](/testpass.png)

### What I've Tried
- Moving `matA` to SRAM => it didn't work.