#include "convolucion.h"

void convolucion2D_hw(T matriz[DIM][DIM], T kernel[KERNEL_DIM][KERNEL_DIM], T salida[DIM - KERNEL_DIM + 1][DIM - KERNEL_DIM + 1]) {
    for (int i = 0; i < DIM - KERNEL_DIM + 1; i++) {
        #pragma HLS PIPELINE II=1
        for (int j = 0; j < DIM - KERNEL_DIM + 1; j++) {
            T suma = 0;

            for (int m = 0; m < KERNEL_DIM; m++) {
                #pragma HLS UNROLL
                for (int n = 0; n < KERNEL_DIM; n++) {
                    suma += matriz[i + m][j + n] * kernel[m][n];
                }
            }

            salida[i][j] = suma;
        }
    }
}


/* Implementing AXI4-Stream */
void convolucion2D_hw_wrapped(hls::stream<AXI_VAL> &in_stream, hls::stream<AXI_VAL> &out_stream) {
	
	#pragma HLS INTERFACE s_axilite port=return bundle=CONTROL_BUS 	
    #pragma HLS INTERFACE axis port=in_stream						
    #pragma HLS INTERFACE axis port=out_stream					

    T matriz[DIM][DIM];
    T kernel[KERNEL_DIM][KERNEL_DIM];
    T salida[DIM - KERNEL_DIM + 1][DIM - KERNEL_DIM + 1];
    #pragma HLS ARRAY_PARTITION variable=salida complete dim=2

    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j += NUM_ELEMS_WORD) {
            #pragma HLS PIPELINE II=1
            WORD_MEM w = in_stream.read().data; 
            conv_t c; 
            for (int k = 0; k < NUM_ELEMS_WORD; k++) {
                c.in = w((k + 1) * 8 - 1, k * 8);
                matriz[i][j + k] = c.out;
            }
        }
    }

    for (int i = 0; i < KERNEL_DIM; i++) {
        for (int j = 0; j < KERNEL_DIM; j += NUM_ELEMS_WORD) {
            #pragma HLS PIPELINE II=1
            WORD_MEM w = in_stream.read().data;
            conv_t c;
            for (int k = 0; k < NUM_ELEMS_WORD; k++) {
                c.in = w((k + 1) * 8 - 1, k * 8);
                kernel[i][j + k] = c.out;
            }
        }
    }

    convolucion2D_hw(matriz, kernel, salida);

    for (int i = 0; i < DIM - KERNEL_DIM + 1; i++) {
        for (int j = 0; j < DIM - KERNEL_DIM + 1; j += NUM_ELEMS_WORD) {
            #pragma HLS PIPELINE II=1
            AXI_VAL e;
            conv_t c;
            WORD_MEM w;
            for (int k = 0; k < NUM_ELEMS_WORD; k++) {
                c.out = salida[i][j + k];
                w((k + 1) * 8 - 1, k * 8) = c.in;
            }
            
            e.data = w;
            e.strb = -1;
            e.keep = -1;
            e.user = 0;
            e.last = ((i == (DIM - KERNEL_DIM)) && (j == (DIM - KERNEL_DIM + 1 - NUM_ELEMS_WORD)));
            e.id = 0;
            e.dest = 0;
            out_stream.write(e);
        }
    }
}
