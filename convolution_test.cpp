#include <iostream>
#include "convolucion.h"

void convert2array(WORD_MEM w, T out[NUM_ELEMS_WORD]);
void convert2word(WORD_MEM &w, T in[NUM_ELEMS_WORD]);

void convolucion2D_sw(const T matriz[DIM][DIM], const T kernel[KERNEL_DIM][KERNEL_DIM], T salida[DIM - KERNEL_DIM + 1][DIM - KERNEL_DIM + 1]) {
    for (int i = 0; i < DIM - KERNEL_DIM + 1; i++) {
        for (int j = 0; j < DIM - KERNEL_DIM + 1; j++) {
            int suma = 0;
            for (int m = 0; m < KERNEL_DIM; m++) {
                for (int n = 0; n < KERNEL_DIM; n++) {
                    suma += matriz[i + m][j + n] * kernel[m][n];
                }
            }
            salida[i][j] = suma;
        }
    }
}

int main() {
	
    T matriz[DIM][DIM];
    T kernel[KERNEL_DIM][KERNEL_DIM];
    T salida_sw[DIM - KERNEL_DIM + 1][DIM - KERNEL_DIM + 1];
    T salida_hw[DIM - KERNEL_DIM + 1][DIM - KERNEL_DIM + 1];

    for (int i = 0; i < DIM; i++)
        for (int j = 0; j < DIM; j++)
            matriz[i][j] = i + j;

    for (int i = 0; i < KERNEL_DIM; i++)
        for (int j = 0; j < KERNEL_DIM; j++)
        	if (i == 1 && j == 1) {
        	    kernel[i][j] = 5;
        	} else {
        	    kernel[i][j] = -1;
        	}

    hls::stream<AXI_VAL> inp_stream("input_stream");
    hls::stream<AXI_VAL> out_stream("output_stream");

    AXI_VAL e;

    for (int i = 0; i < DIM; i++)
        for (int j = 0; j < DIM; j += NUM_ELEMS_WORD) {
            convert2word(e.data, &matriz[i][j]);
            inp_stream.write(e);
        }

    
    for (int i = 0; i < KERNEL_DIM; i++)
        for (int j = 0; j < KERNEL_DIM; j += NUM_ELEMS_WORD) {
            convert2word(e.data, &kernel[i][j]); 
            inp_stream.write(e);
        }

    
    convolucion2D_hw_wrapped(inp_stream, out_stream);

    for (int i = 0; i < DIM - KERNEL_DIM + 1; i++)
        for (int j = 0; j < DIM - KERNEL_DIM + 1; j += NUM_ELEMS_WORD) {
            e = out_stream.read(); 
            convert2array(e.data, &salida_hw[i][j]);
        }

    convolucion2D_sw(matriz, kernel, salida_sw);

    int errores = 0;
    for (int i = 0; i < DIM - KERNEL_DIM + 1; i++) {
        for (int j = 0; j < DIM - KERNEL_DIM + 1; j++) {
            if (salida_sw[i][j] != salida_hw[i][j]) {
                errores++;
                std::cout << "Diferencia en (" << i << ", " << j << "): "
                          << "SW=" << (int)salida_sw[i][j] << " HW=" << (int)salida_hw[i][j] << std::endl;
            }
        }
    }

    if (errores == 0) {
        std::cout << "Prueba superada: No se encontraron errores." << std::endl;
    } else {
        std::cout << "Se encontraron " << errores << " errores." << std::endl;
    }

    return 0;
}

void convert2array(WORD_MEM w, T out[NUM_ELEMS_WORD]) {
    conv_t c;
    for (int k = 0; k < NUM_ELEMS_WORD; k++) {
        c.in = w((k + 1) * 8 - 1, k * 8);
        out[k] = c.out;
    }
}

void convert2word(WORD_MEM &w, T in[NUM_ELEMS_WORD]) {
    conv_t c;
    for (int k = 0; k < NUM_ELEMS_WORD; k++) {
        c.out = in[k];
        w((k + 1) * 8 - 1, k * 8) = c.in;
    }
}
