set SynModuleInfo {
  {SRCNAME entry_proc MODELNAME entry_proc RTLNAME image_accel_entry_proc}
  {SRCNAME {(anonymous namespace)read_input_Pipeline_read_loop} MODELNAME p_anonymous_namespace_read_input_Pipeline_read_loop RTLNAME image_accel_p_anonymous_namespace_read_input_Pipeline_read_loop
    SUBMODULES {
      {MODELNAME image_accel_flow_control_loop_pipe_sequential_init RTLNAME image_accel_flow_control_loop_pipe_sequential_init BINDTYPE interface TYPE internal_upc_flow_control INSTNAME image_accel_flow_control_loop_pipe_sequential_init_U}
    }
  }
  {SRCNAME {(anonymous namespace)read_input} MODELNAME p_anonymous_namespace_read_input RTLNAME image_accel_p_anonymous_namespace_read_input}
  {SRCNAME {(anonymous namespace)rgb_to_gray_Pipeline_process_loop} MODELNAME p_anonymous_namespace_rgb_to_gray_Pipeline_process_loop RTLNAME image_accel_p_anonymous_namespace_rgb_to_gray_Pipeline_process_loop
    SUBMODULES {
      {MODELNAME image_accel_mul_8ns_8ns_15_1_1 RTLNAME image_accel_mul_8ns_8ns_15_1_1 BINDTYPE op TYPE mul IMPL auto LATENCY 0 ALLOW_PRAGMA 1}
      {MODELNAME image_accel_mac_muladd_8ns_5ns_15ns_15_4_1 RTLNAME image_accel_mac_muladd_8ns_5ns_15ns_15_4_1 BINDTYPE op TYPE all IMPL dsp_slice LATENCY 3}
      {MODELNAME image_accel_mac_muladd_8ns_8ns_15ns_16_4_1 RTLNAME image_accel_mac_muladd_8ns_8ns_15ns_16_4_1 BINDTYPE op TYPE all IMPL dsp_slice LATENCY 3}
    }
  }
  {SRCNAME {(anonymous namespace)rgb_to_gray} MODELNAME p_anonymous_namespace_rgb_to_gray RTLNAME image_accel_p_anonymous_namespace_rgb_to_gray}
  {SRCNAME {(anonymous namespace)write_output_Pipeline_write_loop} MODELNAME p_anonymous_namespace_write_output_Pipeline_write_loop RTLNAME image_accel_p_anonymous_namespace_write_output_Pipeline_write_loop}
  {SRCNAME {(anonymous namespace)write_output} MODELNAME p_anonymous_namespace_write_output RTLNAME image_accel_p_anonymous_namespace_write_output}
  {SRCNAME image_accel MODELNAME image_accel RTLNAME image_accel IS_TOP 1
    SUBMODULES {
      {MODELNAME image_accel_fifo_w64_d4_S RTLNAME image_accel_fifo_w64_d4_S BINDTYPE storage TYPE fifo IMPL srl ALLOW_PRAGMA 1 INSTNAME output_gray_c_U}
      {MODELNAME image_accel_fifo_w24_d64_A RTLNAME image_accel_fifo_w24_d64_A BINDTYPE storage TYPE fifo IMPL memory ALLOW_PRAGMA 1 INSTNAME rgb_stream_U}
      {MODELNAME image_accel_fifo_w32_d2_S RTLNAME image_accel_fifo_w32_d2_S BINDTYPE storage TYPE fifo IMPL srl ALLOW_PRAGMA 1 INSTNAME num_pixels_c1_U}
      {MODELNAME image_accel_fifo_w8_d64_S RTLNAME image_accel_fifo_w8_d64_S BINDTYPE storage TYPE fifo IMPL srl ALLOW_PRAGMA 1 INSTNAME gray_stream_U}
      {MODELNAME image_accel_fifo_w32_d2_S RTLNAME image_accel_fifo_w32_d2_S BINDTYPE storage TYPE fifo IMPL srl ALLOW_PRAGMA 1 INSTNAME num_pixels_c_U}
      {MODELNAME image_accel_start_for_p_anonymous_namespace_write_output_U0 RTLNAME image_accel_start_for_p_anonymous_namespace_write_output_U0 BINDTYPE storage TYPE fifo IMPL srl ALLOW_PRAGMA 1 INSTNAME start_for_p_anonymous_namespace_write_output_U0_U}
      {MODELNAME image_accel_start_for_p_anonymous_namespace_rgb_to_gray_U0 RTLNAME image_accel_start_for_p_anonymous_namespace_rgb_to_gray_U0 BINDTYPE storage TYPE fifo IMPL srl ALLOW_PRAGMA 1 INSTNAME start_for_p_anonymous_namespace_rgb_to_gray_U0_U}
      {MODELNAME image_accel_gmem0_m_axi RTLNAME image_accel_gmem0_m_axi BINDTYPE interface TYPE adapter IMPL m_axi}
      {MODELNAME image_accel_gmem1_m_axi RTLNAME image_accel_gmem1_m_axi BINDTYPE interface TYPE adapter IMPL m_axi}
      {MODELNAME image_accel_control_s_axi RTLNAME image_accel_control_s_axi BINDTYPE interface TYPE interface_s_axilite}
    }
  }
}
