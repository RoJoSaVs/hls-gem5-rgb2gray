set moduleName p_anonymous_namespace_rgb_to_gray_Pipeline_process_loop
set isTopModule 0
set isCombinational 0
set isDatapathOnly 0
set isPipelined 1
set pipeline_type none
set FunctionProtocol ap_ctrl_hs
set isOneStateSeq 0
set ProfileFlag 0
set StallSigGenFlag 0
set isEnableWaveformDebug 1
set hasInterrupt 0
set DLRegFirstOffset 0
set DLRegItemOffset 0
set C_modelName {(anonymous namespace)rgb_to_gray_Pipeline_process_loop}
set C_modelType { void 0 }
set ap_memory_interface_dict [dict create]
set C_modelArgList {
	{ num_pixels_2 int 32 regular  }
	{ rgb_stream int 24 regular {fifo 0 volatile }  }
	{ gray_stream int 8 regular {fifo 1 volatile }  }
}
set hasAXIMCache 0
set hasAXIML2Cache 0
set AXIMCacheInstDict [dict create]
set C_modelArgMapList {[ 
	{ "Name" : "num_pixels_2", "interface" : "wire", "bitwidth" : 32, "direction" : "READONLY"} , 
 	{ "Name" : "rgb_stream", "interface" : "fifo", "bitwidth" : 24, "direction" : "READONLY"} , 
 	{ "Name" : "gray_stream", "interface" : "fifo", "bitwidth" : 8, "direction" : "WRITEONLY"} ]}
# RTL Port declarations: 
set portNum 17
set portList { 
	{ ap_clk sc_in sc_logic 1 clock -1 } 
	{ ap_rst sc_in sc_logic 1 reset -1 active_high_sync } 
	{ ap_start sc_in sc_logic 1 start -1 } 
	{ ap_done sc_out sc_logic 1 predone -1 } 
	{ ap_idle sc_out sc_logic 1 done -1 } 
	{ ap_ready sc_out sc_logic 1 ready -1 } 
	{ rgb_stream_dout sc_in sc_lv 24 signal 1 } 
	{ rgb_stream_num_data_valid sc_in sc_lv 7 signal 1 } 
	{ rgb_stream_fifo_cap sc_in sc_lv 7 signal 1 } 
	{ rgb_stream_empty_n sc_in sc_logic 1 signal 1 } 
	{ rgb_stream_read sc_out sc_logic 1 signal 1 } 
	{ gray_stream_din sc_out sc_lv 8 signal 2 } 
	{ gray_stream_num_data_valid sc_in sc_lv 7 signal 2 } 
	{ gray_stream_fifo_cap sc_in sc_lv 7 signal 2 } 
	{ gray_stream_full_n sc_in sc_logic 1 signal 2 } 
	{ gray_stream_write sc_out sc_logic 1 signal 2 } 
	{ num_pixels_2 sc_in sc_lv 32 signal 0 } 
}
set NewPortList {[ 
	{ "name": "ap_clk", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "clock", "bundle":{"name": "ap_clk", "role": "default" }} , 
 	{ "name": "ap_rst", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "reset", "bundle":{"name": "ap_rst", "role": "default" }} , 
 	{ "name": "ap_start", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "start", "bundle":{"name": "ap_start", "role": "default" }} , 
 	{ "name": "ap_done", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "predone", "bundle":{"name": "ap_done", "role": "default" }} , 
 	{ "name": "ap_idle", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "done", "bundle":{"name": "ap_idle", "role": "default" }} , 
 	{ "name": "ap_ready", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "ready", "bundle":{"name": "ap_ready", "role": "default" }} , 
 	{ "name": "rgb_stream_dout", "direction": "in", "datatype": "sc_lv", "bitwidth":24, "type": "signal", "bundle":{"name": "rgb_stream", "role": "dout" }} , 
 	{ "name": "rgb_stream_num_data_valid", "direction": "in", "datatype": "sc_lv", "bitwidth":7, "type": "signal", "bundle":{"name": "rgb_stream", "role": "num_data_valid" }} , 
 	{ "name": "rgb_stream_fifo_cap", "direction": "in", "datatype": "sc_lv", "bitwidth":7, "type": "signal", "bundle":{"name": "rgb_stream", "role": "fifo_cap" }} , 
 	{ "name": "rgb_stream_empty_n", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "rgb_stream", "role": "empty_n" }} , 
 	{ "name": "rgb_stream_read", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "rgb_stream", "role": "read" }} , 
 	{ "name": "gray_stream_din", "direction": "out", "datatype": "sc_lv", "bitwidth":8, "type": "signal", "bundle":{"name": "gray_stream", "role": "din" }} , 
 	{ "name": "gray_stream_num_data_valid", "direction": "in", "datatype": "sc_lv", "bitwidth":7, "type": "signal", "bundle":{"name": "gray_stream", "role": "num_data_valid" }} , 
 	{ "name": "gray_stream_fifo_cap", "direction": "in", "datatype": "sc_lv", "bitwidth":7, "type": "signal", "bundle":{"name": "gray_stream", "role": "fifo_cap" }} , 
 	{ "name": "gray_stream_full_n", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "gray_stream", "role": "full_n" }} , 
 	{ "name": "gray_stream_write", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "gray_stream", "role": "write" }} , 
 	{ "name": "num_pixels_2", "direction": "in", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "num_pixels_2", "role": "default" }}  ]}

set RtlHierarchyInfo {[
	{"ID" : "0", "Level" : "0", "Path" : "`AUTOTB_DUT_INST", "Parent" : "", "Child" : ["1", "2", "3", "4"],
		"CDFG" : "p_anonymous_namespace_rgb_to_gray_Pipeline_process_loop",
		"Protocol" : "ap_ctrl_hs",
		"ControlExist" : "1", "ap_start" : "1", "ap_ready" : "1", "ap_done" : "1", "ap_continue" : "0", "ap_idle" : "1", "real_start" : "0",
		"Pipeline" : "None", "UnalignedPipeline" : "0", "RewindPipeline" : "0", "ProcessNetwork" : "0",
		"II" : "0",
		"VariableLatency" : "1", "ExactLatency" : "-1", "EstimateLatencyMin" : "7", "EstimateLatencyMax" : "2073606",
		"Combinational" : "0",
		"Datapath" : "0",
		"ClockEnable" : "0",
		"HasSubDataflow" : "0",
		"InDataflowNetwork" : "0",
		"HasNonBlockingOperation" : "0",
		"IsBlackBox" : "0",
		"Port" : [
			{"Name" : "num_pixels_2", "Type" : "None", "Direction" : "I"},
			{"Name" : "rgb_stream", "Type" : "Fifo", "Direction" : "I",
				"BlockSignal" : [
					{"Name" : "rgb_stream_blk_n", "Type" : "RtlSignal"}]},
			{"Name" : "gray_stream", "Type" : "Fifo", "Direction" : "O",
				"BlockSignal" : [
					{"Name" : "gray_stream_blk_n", "Type" : "RtlSignal"}]}],
		"Loop" : [
			{"Name" : "process_loop", "PipelineType" : "UPC",
				"LoopDec" : {"FSMBitwidth" : "1", "FirstState" : "ap_ST_fsm_pp0_stage0", "FirstStateIter" : "ap_enable_reg_pp0_iter0", "FirstStateBlock" : "ap_block_pp0_stage0_subdone", "LastState" : "ap_ST_fsm_pp0_stage0", "LastStateIter" : "ap_enable_reg_pp0_iter5", "LastStateBlock" : "ap_block_pp0_stage0_subdone", "QuitState" : "ap_ST_fsm_pp0_stage0", "QuitStateIter" : "ap_enable_reg_pp0_iter5", "QuitStateBlock" : "ap_block_pp0_stage0_subdone", "OneDepthLoop" : "0", "has_ap_ctrl" : "1", "has_continue" : "0"}}]},
	{"ID" : "1", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.mul_8ns_8ns_15_1_1_U12", "Parent" : "0"},
	{"ID" : "2", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.mac_muladd_8ns_5ns_15ns_15_4_1_U13", "Parent" : "0"},
	{"ID" : "3", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.mac_muladd_8ns_8ns_15ns_16_4_1_U14", "Parent" : "0"},
	{"ID" : "4", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.flow_control_loop_pipe_sequential_init_U", "Parent" : "0"}]}


set ArgLastReadFirstWriteLatency {
	p_anonymous_namespace_rgb_to_gray_Pipeline_process_loop {
		num_pixels_2 {Type I LastRead 0 FirstWrite -1}
		rgb_stream {Type I LastRead 1 FirstWrite -1}
		gray_stream {Type O LastRead -1 FirstWrite 5}}}

set hasDtUnsupportedChannel 0

set PerformanceInfo {[
	{"Name" : "Latency", "Min" : "7", "Max" : "2073606"}
	, {"Name" : "Interval", "Min" : "7", "Max" : "2073606"}
]}

set PipelineEnableSignalInfo {[
	{"Pipeline" : "0", "EnableSignal" : "ap_enable_pp0"}
]}

set Spec2ImplPortList { 
	num_pixels_2 { ap_none {  { num_pixels_2 in_data 0 32 } } }
	rgb_stream { ap_fifo {  { rgb_stream_dout fifo_data_in 0 24 }  { rgb_stream_num_data_valid fifo_status_num_data_valid 0 7 }  { rgb_stream_fifo_cap fifo_update 0 7 }  { rgb_stream_empty_n fifo_status 0 1 }  { rgb_stream_read fifo_port_we 1 1 } } }
	gray_stream { ap_fifo {  { gray_stream_din fifo_data_in 1 8 }  { gray_stream_num_data_valid fifo_status_num_data_valid 0 7 }  { gray_stream_fifo_cap fifo_update 0 7 }  { gray_stream_full_n fifo_status 0 1 }  { gray_stream_write fifo_port_we 1 1 } } }
}
