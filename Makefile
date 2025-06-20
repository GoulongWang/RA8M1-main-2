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

#all: vec_scalar_gf16.elf mat_vec_gf16.elf

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
	
# 新增 mat_vec_mul_gf256.elf 規則
mat_vec_mul_gf256.elf: gf256/matrix-vector-multiplication/mat_vec_mul_gf256_mve.c.o randombytes.c.o gf256/matrix-vector-multiplication/mat_vec_mul_gf256_mve.S.o $(LDSCRIPT) $(LIBDEBS)
	$(LD) $(LDFLAGS) -o $@ gf256/matrix-vector-multiplication/mat_vec_mul_gf256_mve.c.o randombytes.c.o gf256/matrix-vector-multiplication/mat_vec_mul_gf256_mve.S.o -Wl,--start-group $(LDLIBS) -Wl,--end-group

# 新增 vec_scalar_mul_gf256.elf 規則
vec_scalar_mul_gf256.elf: gf256/vector-scalar-multiplication/vec_scalar_mul_gf256_mve.c.o randombytes.c.o gf256/vector-scalar-multiplication/vec_scalar_mul_gf256_mve.S.o $(LDSCRIPT) $(LIBDEBS)
	$(LD) $(LDFLAGS) -o $@ gf256/vector-scalar-multiplication/vec_scalar_mul_gf256_mve.c.o randombytes.c.o gf256/vector-scalar-multiplication/vec_scalar_mul_gf256_mve.S.o -Wl,--start-group $(LDLIBS) -Wl,--end-group

trimat_2trimat_madd_gf16.elf: gf16/trimat-2trimat-madd/gf16trimat_2trimat_madd.c.o randombytes.c.o gf16/trimat-2trimat-madd/gf16trimat_2trimat_madd.S.o $(LDSCRIPT) $(LIBDEBS)
	$(LD) $(LDFLAGS) -o $@ gf16/trimat-2trimat-madd/gf16trimat_2trimat_madd.c.o randombytes.c.o gf16/trimat-2trimat-madd/gf16trimat_2trimat_madd.S.o -Wl,--start-group $(LDLIBS) -Wl,--end-group

trimat_2trimat_madd_gf256.elf: gf256/trimat-2trimat-madd/gf256trimat_2trimat_madd.c.o randombytes.c.o gf256/trimat-2trimat-madd/gf256trimat_2trimat_madd.S.o $(LDSCRIPT) $(LIBDEBS)
	$(LD) $(LDFLAGS) -o $@ gf256/trimat-2trimat-madd/gf256trimat_2trimat_madd.c.o randombytes.c.o gf256/trimat-2trimat-madd/gf256trimat_2trimat_madd.S.o -Wl,--start-group $(LDLIBS) -Wl,--end-group

uov-publicmap.elf: gf16/uov-publicmap/ov_publicmap.c.o randombytes.c.o gf16/uov-publicmap/ov_publicmap.S.o $(LDSCRIPT) $(LIBDEBS)
	$(LD) $(LDFLAGS) -o $@ gf16/uov-publicmap/ov_publicmap.c.o randombytes.c.o gf16/uov-publicmap/ov_publicmap.S.o -Wl,--start-group $(LDLIBS) -Wl,--end-group

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
