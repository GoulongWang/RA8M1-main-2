# === 處理 openssl
OPENSSL_DIR := $(shell brew --prefix openssl@3)
# 把 include 目錄加到編譯期
CFLAGS  += -I$(OPENSSL_DIR)/include

# 把 lib 目錄加到連結期
LDFLAGS += -L$(OPENSSL_DIR)/lib

# 把需要的 libssl / libcrypto 加到連結清單
LDLIBS  += -lssl -lcrypto
# ===

CFLAGS += \
	-O3 \
	-Wall -Wextra -Wshadow \
	-MMD \
	-fno-common \
	-ffunction-sections \
	-fdata-sections \
	$(CPPFLAGS)

LDFLAGS += \
	-Wl,--gc-sections \
	-L.

LDLIBS += -lhal
LIBDEBS=libhal.a

OBJS= gf16/vector-scalar-multiplication/vec_scalar_mul_gf16_mve.c.o randombytes.c.o gf16/vector-scalar-multiplication/vec_scalar_mul_gf16_mve.S.o gf16/vector-scalar-multiplication/mat_vec_mul_gf16_mve.S.o

objs = $(addsuffix .o,$(1))
PLATFORM ?= mps3-an547
include platform/$(PLATFORM).mk

run-vec-scalar-gf16.elf: vec_scalar_gf16.elf
	qemu-system-arm -M mps3-an547 -nographic -semihosting -kernel $<

run-mat-vec-gf16.elf: mat_vec_gf16.elf
	qemu-system-arm -M mps3-an547 -nographic -semihosting -kernel $<

run-mayo-mat.elf: mayo_mat.elf
	qemu-system-arm -M mps3-an547 -nographic -semihosting -kernel $<

run-mat-vec-gf256.elf: mat_vec_mul_gf256.elf
	qemu-system-arm -M mps3-an547 -nographic -semihosting -kernel $<

run-vec-scalar-gf256.elf: vec_scalar_mul_gf256.elf
	qemu-system-arm -M mps3-an547 -nographic -semihosting -kernel $<

run-trimat-2trimat-madd-gf16.elf: trimat_2trimat_madd_gf16.elf
	qemu-system-arm -M mps3-an547 -nographic -semihosting -kernel $<

run-trimat-2trimat-madd-gf256.elf: trimat_2trimat_madd_gf256.elf
	qemu-system-arm -M mps3-an547 -nographic -semihosting -kernel $<

run-uov-publicmap.elf: uov-publicmap.elf
	qemu-system-arm -M mps3-an547 -nographic -semihosting -kernel $<

run-uov-Is.elf: uov-Is.elf
	qemu-system-arm -M mps3-an547 -nographic -semihosting -kernel $<

%.elf: %.c.o $(OBJS) $(LDSCRIPT) $(LIBDEBS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) -Wl,--start-group $(LDLIBS) -Wl,--end-group

# 新增 vec_scalar_gf16.elf 規則
vec_scalar_gf16.elf: gf16/vector-scalar-multiplication/vec_scalar_mul_gf16_mve.c.o randombytes.c.o gf16/vector-scalar-multiplication/vec_scalar_mul_gf16_mve.S.o $(LDSCRIPT) $(LIBDEBS)
	$(LD) $(LDFLAGS) -o $@ gf16/vector-scalar-multiplication/vec_scalar_mul_gf16_mve.c.o randombytes.c.o gf16/vector-scalar-multiplication/vec_scalar_mul_gf16_mve.S.o -Wl,--start-group $(LDLIBS) -Wl,--end-group

# 新增 mat_vec_gf16.elf 的專屬規則
mat_vec_gf16.elf: gf16/matrix-vector-multiplication/mve_matrix_mul_gf16_main.c.o randombytes.c.o gf16/matrix-vector-multiplication/mat_vec_mul_gf16_mve.S.o $(LDSCRIPT) $(LIBDEBS)
	$(LD) $(LDFLAGS) -o $@ gf16/matrix-vector-multiplication/mve_matrix_mul_gf16_main.c.o randombytes.c.o gf16/matrix-vector-multiplication/mat_vec_mul_gf16_mve.S.o -Wl,--start-group $(LDLIBS) -Wl,--end-group

# 新增 mayo_mat.elf 規則
mayo_mat.elf: gf16/MAYO-matrix-multiplication/mayo_gf16_mat_vec_mul_mve.c.o randombytes.c.o gf16/MAYO-matrix-multiplication/mul_add_mat_x_m_mat.S.o $(LDSCRIPT) $(LIBDEBS)
	$(LD) $(LDFLAGS) -o $@ gf16/MAYO-matrix-multiplication/mayo_gf16_mat_vec_mul_mve.c.o randombytes.c.o gf16/MAYO-matrix-multiplication/mul_add_mat_x_m_mat.S.o -Wl,--start-group $(LDLIBS) -Wl,--end-group

GF256_MAT_VEC_MUL_DIR  := gf256/matrix-vector-multiplication/
GF256_MAT_VEC_MUL_SRCS := $(wildcard $(GF256_VEC_SCALAR_MUL_DIR)/*.c $(GF256_VEC_SCALAR_MUL_DIR)/*.S)
GF256_MAT_VEC_MUL_OBJS := $(addsuffix .o,$(GF256_VEC_SCALAR_MUL_SRCS))
mat_vec_mul_gf256.elf: randombytes.c.o $(GF256_MAT_VEC_MUL_OBJS) $(LDSCRIPT) $(LIBDEBS)
	$(LD) $(LDFLAGS) -o $@ $(GF256_MAT_VEC_MUL_OBJS) randombytes.c.o -Wl,--start-group $(LDLIBS) -Wl,--end-group

GF256_VEC_SCALAR_MUL_DIR  := gf256/vector-scalar-multiplication/
GF256_VEC_SCALAR_MUL_SRCS := $(wildcard $(GF256_VEC_SCALAR_MUL_DIR)/*.c $(GF256_VEC_SCALAR_MUL_DIR)/*.S)
GF256_VEC_SCALAR_MUL_OBJS := $(addsuffix .o,$(GF256_VEC_SCALAR_MUL_SRCS))
vec_scalar_mul_gf256.elf: randombytes.c.o $(GF256_VEC_SCALAR_MUL_OBJS) $(LDSCRIPT) $(LIBDEBS)
	$(LD) $(LDFLAGS) -o $@ randombytes.c.o $(GF256_VEC_SCALAR_MUL_OBJS) -Wl,--start-group $(LDLIBS) -Wl,--end-group

GF16_TRIMAT2TRIMAT_MADD_DIR  := gf16/trimat-2trimat-madd/
GF16_TRIMAT2TRIMAT_MADD_SRCS := $(wildcard $(GF16_TRIMAT2TRIMAT_MADD_DIR)/*.c $(GF16_TRIMAT2TRIMAT_MADD_DIR)/*.S)
GF16_TRIMAT2TRIMAT_MADD_OBJS := $(addsuffix .o,$(GF16_TRIMAT2TRIMAT_MADD_SRCS))
trimat_2trimat_madd_gf16.elf: randombytes.c.o $(GF16_TRIMAT2TRIMAT_MADD_OBJS) $(LDSCRIPT) $(LIBDEBS)
	$(LD) $(LDFLAGS) -o $@ randombytes.c.o $(GF16_TRIMAT2TRIMAT_MADD_OBJS) -Wl,--start-group $(LDLIBS) -Wl,--end-group

GF256_TRIMAT2TRIMAT_MADD_DIR  := gf256/trimat-2trimat-madd/
GF256_TRIMAT2TRIMAT_MADD_SRCS := $(wildcard $(GF256_TRIMAT2TRIMAT_MADD_DIR)/*.c $(GF256_TRIMAT2TRIMAT_MADD_DIR)/*.S)
GF256_TRIMAT2TRIMAT_MADD_OBJS := $(addsuffix .o,$(GF256_TRIMAT2TRIMAT_MADD_SRCS))
trimat_2trimat_madd_gf256.elf: randombytes.c.o $(GF256_TRIMAT2TRIMAT_MADD_OBJS) $(LDSCRIPT) $(LIBDEBS)
	$(LD) $(LDFLAGS) -o $@ randombytes.c.o $(GF256_TRIMAT2TRIMAT_MADD_OBJS) -Wl,--start-group $(LDLIBS) -Wl,--end-group

UOV_PUBLICMAP_DIR  := gf16/uov-publicmap
UOV_PUBLICMAP_SRCS := $(wildcard $(UOV_PUBLICMAP_DIR)/*.c $(UOV_PUBLICMAP_DIR)/*.S)
UOV_PUBLICMAP_OBJS := $(addsuffix .o,$(UOV_PUBLICMAP_SRCS))
uov-publicmap.elf: $(UOV_PUBLICMAP_OBJS) randombytes.c.o $(LDSCRIPT) $(LIBDEBS)
	$(LD) $(LDFLAGS) -o $@ $(UOV_PUBLICMAP_OBJS) randombytes.c.o -Wl,--start-group $(LDLIBS) -Wl,--end-group

# uov-Is.elf: gf16/UOV-Is/sign_api-test.c.o gf16/UOV-Is/sign.c.o randombytes.c.o $(LDSCRIPT) $(LIBDEBS)
# 	$(LD) $(LDFLAGS) -o $@ gf16/UOV-Is/sign_api-test.c.o randombytes.c.o gf16/UOV-Is/sign.c.o -Wl,--start-group $(LDLIBS) -Wl,--end-group
UOV_IS_DIR := gf16/UOV-Is
#UOV_IS_SRCS := sign_api-test.c sign.c utils_randombytes.c ov.c ov_keypair.c utils_randombytes.c utils_hash.c blas_matrix.c
#UOV_IS_OBJS := $(addprefix $(UOV_IS_DIR)/,$(patsubst %.c,%.c.o,$(UOV_IS_SRCS)))

UOV_IS_SRCS := $(wildcard $(UOV_IS_DIR)/*.c)
UOV_IS_OBJS := $(patsubst %.c,%.c.o,$(UOV_IS_SRCS))

uov-Is.elf: $(UOV_IS_OBJS) randombytes.c.o $(LDSCRIPT) $(LIBDEBS)
	$(LD) $(LDFLAGS) -o $@ $(UOV_IS_OBJS) -Wl,--start-group $(LDLIBS) -Wl,--end-group

# Dependencies for macro file
vec_scalar_mul_gf16_mve.S.o:  vec_scalar_mul_gf16_mve.i
mat_vec_mul_gf16_mve.S.o:     mat_vec_mul_gf16_mve.i vec_scalar_mul_gf16_mve.i
mul_add_mat_x_m_mat.S.o:      mul_add_mat_x_m_mat.i vec_scalar_mul_gf16_mve.i
mat_vec_mul_gf256_mve.S.o: 	  mat_vec_mul_gf256_mve.i
vec_scalar_mul_gf256_mve.S.o: vec_scalar_mul_gf256_mve.i
gf16trimat_2trimat_madd.S:    gf16trimat_2trimat_madd.i
gf256trimat_2trimat_madd.S:    gf256trimat_2trimat_madd.i
ov_publicmap.S.o: ov_publicmap.i

%.a:
	@echo "  AR      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(AR) rcs $@ $(filter %.o,$^)

%.c.o: %.c
	@echo "  CC      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<

%.c.S: %.c
	@echo "  CC      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -S -o $@ $(CFLAGS) $<

%.S.o: %.S
	@echo "  AS      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<

%.s.o: %.s
	@echo "  AS      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<

.PHONY: clean
clean:
	rm -rf *.o *.d platform/*.o platform/*.d *.elf *.a

-include $(OBJS:.o=.d)
