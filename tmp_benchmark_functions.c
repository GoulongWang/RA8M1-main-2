void benchmark_gf256mat_prod_1936_68(){
    printf("====== UOV-Ip: gf256mat_prod 1936_68 unit test ======\n");
    uint32_t sum_ref = 0, sum_mve = 0, sum_m4 = 0;
    uint32_t cycles;

    uint8_t matA[ N_A_WIDTH * N_A_VEC_BYTE];
    uint8_t vec_b[ N_A_VEC_BYTE ];
    uint8_t vec_c0[ N_A_VEC_BYTE ];
    uint8_t vec_c1[ N_A_VEC_BYTE ];
    uint8_t vec_c2[ N_A_VEC_BYTE ];
    
    int fail = 0;
    for (int l = 1; l <= TEST_RUN; l++) {
        randombytes(vec_b, sizeof vec_b);
        randombytes(matA, sizeof matA);

        memset(vec_c0, 0, sizeof(vec_c0));
        memset(vec_c1, 0, sizeof(vec_c1));
        memset(vec_c2, 0, sizeof(vec_c2));

        bench_cycles(gf256mat_prod_ref( vec_c0, matA, N_A_VEC_BYTE, N_A_WIDTH, vec_b), cycles);
        sum_ref += cycles;
        bench_cycles(gf256mat_prod_1936_68(vec_c1, matA, vec_b), cycles);
        sum_mve += cycles;
        bench_cycles(gf256mat_prod_m4f_1936_68_normal_normal((uint32_t *)vec_c2, (uint32_t *)matA, (uint8_t *)vec_b), cycles);
        sum_m4 += cycles;

        gf256v_add( vec_c0, vec_c1, N_A_VEC_BYTE );
        if ( !gf256v_is_zero( vec_c0, N_A_VEC_BYTE ) ) {
            gf256v_add( vec_c0, vec_c1, N_A_VEC_BYTE );
            printf("test %d:\n", l);
            printf("out_ref = ");
            for (int i = 0; i < N_A_VEC_BYTE; i++) {
                printf("%d ", vec_c0[i]);
            }
            printf("\n\n");
            printf("out_mve = ");
            for (int i = 0; i < N_A_VEC_BYTE; i++) {
                printf("%d ", vec_c1[i]);
            }
            printf("\n");
            fail = 1;
            break;
        }
    }

    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n");
    printf("Average ref cycles = %lu\n", sum_ref / TEST_RUN);
    printf("Average MVE cycles = %lu\n", sum_mve / TEST_RUN);
    printf("Average M4 cycles = %lu\n", sum_m4 / TEST_RUN);
}

void benchmark_gf256mat_prod_68_44(){
    printf("====== UOV-Ip: gf256mat_prod 68_44 unit test ======\n");
    uint32_t sum_ref = 0, sum_mve = 0, sum_m4 = 0;
    uint32_t cycles;

    uint8_t N_A_VEC_BYTE_ = 68;
    uint8_t N_A_WIDTH_ = 44;
    uint8_t matA[ N_A_WIDTH_ * N_A_VEC_BYTE_];
    uint8_t vec_b[ N_A_VEC_BYTE_ ];
    uint8_t vec_c0[ N_A_VEC_BYTE_ ];
    uint8_t vec_c1[ N_A_VEC_BYTE_ ];
    uint8_t vec_c2[ N_A_VEC_BYTE_ ];
    
    int fail = 0;
    for (int l = 1; l <= 1; l++) {
        randombytes(vec_b, sizeof vec_b);
        randombytes(matA, sizeof matA);

        memset(vec_c0, 0, sizeof(vec_c0));
        memset(vec_c1, 0, sizeof(vec_c1));
        memset(vec_c2, 0, sizeof(vec_c2));

        bench_cycles(gf256mat_prod_ref( vec_c0, matA, N_A_VEC_BYTE_, N_A_WIDTH_, vec_b), cycles);
        sum_ref += cycles;
        bench_cycles(gf256mat_prod_68_44(vec_c1, matA, vec_b), cycles);
        sum_mve += cycles;
        bench_cycles(gf256mat_prod_m4f_68_44_normal_normal((uint32_t *)vec_c2, (uint32_t *)matA, vec_b), cycles);
        sum_m4 += cycles;

        gf256v_add( vec_c0, vec_c1, N_A_VEC_BYTE_ );

        if ( !gf256v_is_zero( vec_c0, N_A_VEC_BYTE_ ) ) {
            gf256v_add( vec_c0, vec_c1, N_A_VEC_BYTE_ );
            printf("test %d:\n", l);
            printf("out_ref = ");
            for (int i = 0; i < N_A_VEC_BYTE_; i++) {
                printf("%d ", vec_c0[i]);
            }
            printf("\n\n");
            printf("out_mve = ");
            for (int i = 0; i < N_A_VEC_BYTE_; i++) {
                printf("%d ", vec_c1[i]);
            }
            printf("\n");
            fail = 1;
            break;
        }
    }

    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n");
    printf("Average ref cycles = %lu\n", sum_ref / TEST_RUN);
    printf("Average MVE cycles = %lu\n", sum_mve / TEST_RUN);
    printf("Average M4 cycles = %lu\n", sum_m4 / TEST_RUN);
}


void benchmark_gf256mat_prod_44_X(){
    printf("====== UOV-Ip: gf256mat_prod 44_X unit test ======\n");
    uint32_t sum_ref = 0, sum_mve = 0, sum_m4 = 0;
    uint32_t cycles;

    uint8_t N_A_VEC_BYTE_test = 44, N_A_WIDTH_test;
    uint8_t vec_b[ N_A_VEC_BYTE_test ];
    uint8_t vec_c0[ N_A_VEC_BYTE_test ];
    uint8_t vec_c1[ N_A_VEC_BYTE_test ];
    uint8_t vec_c2[ N_A_VEC_BYTE_test ];
    
    int fail = 0;
    for (int l = 1; l <= TEST_RUN; l++) {
        randombytes(vec_b, sizeof vec_b);
        randombytes(&N_A_WIDTH_test, sizeof(N_A_WIDTH_test));
        N_A_WIDTH_test = N_A_WIDTH_test % 44 + 1;
        uint8_t matA[ N_A_WIDTH_test * N_A_VEC_BYTE_test];
        randombytes(matA, sizeof matA);

        memset(vec_c0, 0, sizeof(vec_c0));
        memset(vec_c1, 0, sizeof(vec_c1));
        memset(vec_c2, 0, sizeof(vec_c2));

        bench_cycles(gf256mat_prod_ref( vec_c0, matA, N_A_VEC_BYTE_test, N_A_WIDTH_test, vec_b), cycles);
        sum_ref += cycles;
        bench_cycles(gf256mat_prod_44_X(vec_c1, matA, vec_b, N_A_WIDTH_test), cycles);
        sum_mve += cycles;
        bench_cycles(gf256mat_prod_m4f_44_X_normal_normal((uint32_t *)vec_c2, (uint32_t *)matA, vec_b, N_A_WIDTH_test), cycles);
        sum_m4 += cycles;

        gf256v_add( vec_c0, vec_c1, N_A_VEC_BYTE_test );

        if ( !gf256v_is_zero( vec_c0, N_A_VEC_BYTE_test ) ) {
            gf256v_add( vec_c0, vec_c1, N_A_VEC_BYTE_test );
            printf("test %d:\n", l);
            printf("out_ref = ");
            for (int i = 0; i < N_A_VEC_BYTE_test; i++) {
                printf("%d ", vec_c0[i]);
            }
            printf("\n\n");
            printf("out_mve = ");
            for (int i = 0; i < N_A_VEC_BYTE_test; i++) {
                printf("%d ", vec_c1[i]);
            }
            printf("\n");
            fail = 1;
            break;
        }
    }

    printf((fail) ? "TEST FAIL.!\n" : "TEST PASS.\n");
    printf("Average ref cycles = %lu\n", sum_ref / TEST_RUN);
    printf("Average MVE cycles = %lu\n", sum_mve / TEST_RUN);
    printf("Average M4 cycles = %lu\n", sum_m4 / TEST_RUN);
}