// megafunction wizard: %ROM: 1-PORT%VBB%
// GENERATION: STANDARD
// VERSION: WM1.0
// MODULE: altsyncram 

// ============================================================
// File Name: Five_Centimeters_Per_Second_ROM.v
// Megafunction Name(s):
// 			altsyncram
//
// Simulation Library Files(s):
// 			altera_mf
// ============================================================
// ************************************************************
// THIS IS A WIZARD-GENERATED FILE. DO NOT EDIT THIS FILE!
//
// 17.0.0 Build 595 04/25/2017 SJ Lite Edition
// ************************************************************

//Copyright (C) 2017  Intel Corporation. All rights reserved.
//Your use of Intel Corporation's design tools, logic functions 
//and other software and tools, and its AMPP partner logic 
//functions, and any output files from any of the foregoing 
//(including device programming or simulation files), and any 
//associated documentation or information are expressly subject 
//to the terms and conditions of the Intel Program License 
//Subscription Agreement, the Intel Quartus Prime License Agreement,
//the Intel MegaCore Function License Agreement, or other 
//applicable license agreement, including, without limitation, 
//that your use is for the sole purpose of programming logic 
//devices manufactured by Intel and sold by Intel or its 
//authorized distributors.  Please refer to the applicable 
//agreement for further details.

module Five_Centimeters_Per_Second_ROM (
	address,
	clock,
	rden,
	q);

	input	[17:0]  address;
	input	  clock;
	input	  rden;
	output	[15:0]  q;
`ifndef ALTERA_RESERVED_QIS
// synopsys translate_off
`endif
	tri1	  clock;
	tri1	  rden;
`ifndef ALTERA_RESERVED_QIS
// synopsys translate_on
`endif

endmodule

// ============================================================
// CNX file retrieval info
// ============================================================
// Retrieval info: PRIVATE: ADDRESSSTALL_A NUMERIC "0"
// Retrieval info: PRIVATE: AclrAddr NUMERIC "0"
// Retrieval info: PRIVATE: AclrByte NUMERIC "0"
// Retrieval info: PRIVATE: AclrOutput NUMERIC "0"
// Retrieval info: PRIVATE: BYTE_ENABLE NUMERIC "0"
// Retrieval info: PRIVATE: BYTE_SIZE NUMERIC "8"
// Retrieval info: PRIVATE: BlankMemory NUMERIC "0"
// Retrieval info: PRIVATE: CLOCK_ENABLE_INPUT_A NUMERIC "0"
// Retrieval info: PRIVATE: CLOCK_ENABLE_OUTPUT_A NUMERIC "0"
// Retrieval info: PRIVATE: Clken NUMERIC "0"
// Retrieval info: PRIVATE: IMPLEMENT_IN_LES NUMERIC "0"
// Retrieval info: PRIVATE: INIT_FILE_LAYOUT STRING "PORT_A"
// Retrieval info: PRIVATE: INIT_TO_SIM_X NUMERIC "0"
// Retrieval info: PRIVATE: INTENDED_DEVICE_FAMILY STRING "Cyclone IV E"
// Retrieval info: PRIVATE: JTAG_ENABLED NUMERIC "0"
// Retrieval info: PRIVATE: JTAG_ID STRING "NONE"
// Retrieval info: PRIVATE: MAXIMUM_DEPTH NUMERIC "0"
// Retrieval info: PRIVATE: MIFfilename STRING "5_Centimeters_Per_Second_Clip_16bit_PCM_13_Seconds_HEX_ROM.mif"
// Retrieval info: PRIVATE: NUMWORDS_A NUMERIC "214198"
// Retrieval info: PRIVATE: RAM_BLOCK_TYPE NUMERIC "2"
// Retrieval info: PRIVATE: RegAddr NUMERIC "1"
// Retrieval info: PRIVATE: RegOutput NUMERIC "0"
// Retrieval info: PRIVATE: SYNTH_WRAPPER_GEN_POSTFIX STRING "0"
// Retrieval info: PRIVATE: SingleClock NUMERIC "1"
// Retrieval info: PRIVATE: UseDQRAM NUMERIC "0"
// Retrieval info: PRIVATE: WidthAddr NUMERIC "18"
// Retrieval info: PRIVATE: WidthData NUMERIC "16"
// Retrieval info: PRIVATE: rden NUMERIC "1"
// Retrieval info: LIBRARY: altera_mf altera_mf.altera_mf_components.all
// Retrieval info: CONSTANT: ADDRESS_ACLR_A STRING "NONE"
// Retrieval info: CONSTANT: CLOCK_ENABLE_INPUT_A STRING "BYPASS"
// Retrieval info: CONSTANT: CLOCK_ENABLE_OUTPUT_A STRING "BYPASS"
// Retrieval info: CONSTANT: INIT_FILE STRING "5_Centimeters_Per_Second_Clip_16bit_PCM_13_Seconds_HEX_ROM.mif"
// Retrieval info: CONSTANT: INTENDED_DEVICE_FAMILY STRING "Cyclone IV E"
// Retrieval info: CONSTANT: LPM_HINT STRING "ENABLE_RUNTIME_MOD=NO"
// Retrieval info: CONSTANT: LPM_TYPE STRING "altsyncram"
// Retrieval info: CONSTANT: NUMWORDS_A NUMERIC "214198"
// Retrieval info: CONSTANT: OPERATION_MODE STRING "ROM"
// Retrieval info: CONSTANT: OUTDATA_ACLR_A STRING "NONE"
// Retrieval info: CONSTANT: OUTDATA_REG_A STRING "UNREGISTERED"
// Retrieval info: CONSTANT: RAM_BLOCK_TYPE STRING "M9K"
// Retrieval info: CONSTANT: WIDTHAD_A NUMERIC "18"
// Retrieval info: CONSTANT: WIDTH_A NUMERIC "16"
// Retrieval info: CONSTANT: WIDTH_BYTEENA_A NUMERIC "1"
// Retrieval info: USED_PORT: address 0 0 18 0 INPUT NODEFVAL "address[17..0]"
// Retrieval info: USED_PORT: clock 0 0 0 0 INPUT VCC "clock"
// Retrieval info: USED_PORT: q 0 0 16 0 OUTPUT NODEFVAL "q[15..0]"
// Retrieval info: USED_PORT: rden 0 0 0 0 INPUT VCC "rden"
// Retrieval info: CONNECT: @address_a 0 0 18 0 address 0 0 18 0
// Retrieval info: CONNECT: @clock0 0 0 0 0 clock 0 0 0 0
// Retrieval info: CONNECT: @rden_a 0 0 0 0 rden 0 0 0 0
// Retrieval info: CONNECT: q 0 0 16 0 @q_a 0 0 16 0
// Retrieval info: GEN_FILE: TYPE_NORMAL Five_Centimeters_Per_Second_ROM.v TRUE
// Retrieval info: GEN_FILE: TYPE_NORMAL Five_Centimeters_Per_Second_ROM.inc FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL Five_Centimeters_Per_Second_ROM.cmp FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL Five_Centimeters_Per_Second_ROM.bsf FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL Five_Centimeters_Per_Second_ROM_inst.v TRUE
// Retrieval info: GEN_FILE: TYPE_NORMAL Five_Centimeters_Per_Second_ROM_bb.v TRUE
// Retrieval info: LIB_FILE: altera_mf
