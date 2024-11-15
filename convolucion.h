#ifndef __CONVOLUCION_HW_H__
#define __CONVOLUCION_HW_H__

#include <assert.h>
#include <ap_axi_sdata.h>
#include <hls_stream.h>

#define DIM 128      // Tamaño de la matriz de entrada (128x128)
#define KERNEL_DIM 3 // Tamaño del kernel (3x3)
#define WORD_SIZE 32 // Ancho de palabra de 32 bits

typedef unsigned char T; // Tipo de dato (8 bits sin signo)
typedef ap_axiu<WORD_SIZE, 4, 5, 5> AXI_VAL; // Estructura para la interfaz AXI4-Stream

#define NUM_ELEMS_WORD ((WORD_SIZE / 8) / sizeof(T))

typedef ap_uint<WORD_SIZE> WORD_MEM; /*Palabra de 32 bits donde se alamcenan los datos*/

/*Definicion para convertir los datos de entre enteros normales y enteros sin signo de 8 bits sin problema*/
typedef union {
    int in;
    T out;
} conv_t;

/*Definicion de la funcion que hace la multiplicacion mediante hardware*/
void convolucion2D_hw(T matriz[DIM][DIM], T kernel[KERNEL_DIM][KERNEL_DIM], T salida[DIM - KERNEL_DIM + 1][DIM - KERNEL_DIM + 1]);
/*Definicion de la funcion que se encarga de la transmision de datos usando AXI4-Stream*/
void convolucion2D_hw_wrapped(hls::stream<AXI_VAL> &in_stream, hls::stream<AXI_VAL> &out_stream);

#endif
