############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
## Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
############################################################
open_project SSEE_Lab1
set_top convolucion2D_hw_wrapped
add_files ../../../home/pxdrom/Documents/uclm/sistemas_empotrados/SSEE_Lab1/convolucion.h
add_files ../../../home/pxdrom/Documents/uclm/sistemas_empotrados/SSEE_Lab1/convolucion_accel.cpp
add_files -tb ../../../home/pxdrom/Documents/uclm/sistemas_empotrados/SSEE_Lab1/convolution_test.cpp
open_solution "solution1" -flow_target vivado
set_part {xa7a12tcpg238-1I}
create_clock -period 10 -name default
config_cosim -tool xsim
#source "./SSEE_Lab1/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -format ip_catalog
