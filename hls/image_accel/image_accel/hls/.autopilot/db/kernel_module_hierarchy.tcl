set ModuleHierarchy {[{
"Name" : "image_accel","ID" : "0","Type" : "dataflow",
"SubInsts" : [
	{"Name" : "entry_proc_U0","ID" : "1","Type" : "sequential"},
	{"Name" : "p_anonymous_namespace_read_input_U0","ID" : "2","Type" : "sequential",
		"SubInsts" : [
		{"Name" : "grp_p_anonymous_namespace_read_input_Pipeline_read_loop_fu_70","ID" : "3","Type" : "sequential",
			"SubLoops" : [
			{"Name" : "read_loop","ID" : "4","Type" : "pipeline"},]},]},
	{"Name" : "p_anonymous_namespace_rgb_to_gray_U0","ID" : "5","Type" : "sequential",
		"SubInsts" : [
		{"Name" : "grp_p_anonymous_namespace_rgb_to_gray_Pipeline_process_loop_fu_50","ID" : "6","Type" : "sequential",
			"SubLoops" : [
			{"Name" : "process_loop","ID" : "7","Type" : "pipeline"},]},]},
	{"Name" : "p_anonymous_namespace_write_output_U0","ID" : "8","Type" : "sequential",
		"SubInsts" : [
		{"Name" : "grp_p_anonymous_namespace_write_output_Pipeline_write_loop_fu_58","ID" : "9","Type" : "sequential",
			"SubLoops" : [
			{"Name" : "write_loop","ID" : "10","Type" : "pipeline"},]},]},]
}]}