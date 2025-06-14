# test-script for QUTest unit testing harness
# see https://www.state-machine.com/qtools/qutest.html

note("QMsm functional and structural tests...")

#=============================================================================
# preamble...

#=============================================================================
# tests...

#-----------------------------------------------------------------------------
test("TUN_QP_qep_msm_01")
note("QP version")
#
glb_filter(GRP_UA)
command("QP_VERSION_CMD") # get QP_versionStr
expect("@timestamp COMMAND QP_VERSION_CMD QP/C++ ?.?.?")
expect("@timestamp Trg-Done QS_RX_COMMAND")

#-----------------------------------------------------------------------------
test("TUN_QP_qep_msm_02", NORESET)
note("init")
#
glb_filter(GRP_UA)
init()
expect("@timestamp BSP_DISPLAY top-INIT;")
expect("@timestamp BSP_DISPLAY s-ENTRY;")
expect("@timestamp BSP_DISPLAY s2-ENTRY;")
expect("@timestamp BSP_DISPLAY s2-INIT;")
expect("@timestamp BSP_DISPLAY s21-ENTRY;")
expect("@timestamp BSP_DISPLAY s211-ENTRY;")
expect("@timestamp Trg-Done QS_RX_EVENT")

#-----------------------------------------------------------------------------
test("TUN_QP_qep_msm_03", NORESET)
note("isIn")
command("QMSM_GET_STATE_HANDLER_CMD") # get state handler
expect("@timestamp COMMAND QMSM_GET_STATE_HANDLER_CMD TstSM::s211")
expect("@timestamp Trg-Done QS_RX_COMMAND")
#
command("TSTSM_IS_IN_CMD", 1) # is in state s1
expect("@timestamp COMMAND TSTSM_IS_IN_CMD 0 1") # NO
expect("@timestamp Trg-Done QS_RX_COMMAND")
#
command("TSTSM_IS_IN_CMD", 2) # is in state s2
expect("@timestamp COMMAND TSTSM_IS_IN_CMD 1 2") # YES
expect("@timestamp Trg-Done QS_RX_COMMAND")
#
command("TSTSM_IS_IN_CMD", 21) # is in state s21
expect("@timestamp COMMAND TSTSM_IS_IN_CMD 1 21") # YES
expect("@timestamp Trg-Done QS_RX_COMMAND")
#
command("TSTSM_IS_IN_CMD", 211) # is in state s211
expect("@timestamp COMMAND TSTSM_IS_IN_CMD 1 211") # YES
expect("@timestamp Trg-Done QS_RX_COMMAND")

#-----------------------------------------------------------------------------
test("TUN_QP_qep_msm_04", NORESET)
note("dispatch")
dispatch("A_SIG")
expect("@timestamp BSP_DISPLAY s21-A;")
expect("@timestamp BSP_DISPLAY s211-EXIT;")
expect("@timestamp BSP_DISPLAY s21-EXIT;")
expect("@timestamp BSP_DISPLAY s21-ENTRY;")
expect("@timestamp BSP_DISPLAY s21-INIT;")
expect("@timestamp BSP_DISPLAY s211-ENTRY;")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("D_SIG")
expect("@timestamp BSP_DISPLAY s21-D;")
expect("@timestamp BSP_DISPLAY s211-EXIT;")
expect("@timestamp BSP_DISPLAY s21-EXIT;")
expect("@timestamp BSP_DISPLAY s2-INIT;")
expect("@timestamp BSP_DISPLAY s21-ENTRY;")
expect("@timestamp BSP_DISPLAY s211-ENTRY;")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("B_SIG")
expect("@timestamp BSP_DISPLAY s2-B;")
expect("@timestamp BSP_DISPLAY s211-EXIT;")
expect("@timestamp BSP_DISPLAY s21-EXIT;")
expect("@timestamp BSP_DISPLAY s22-ENTRY;")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("F_SIG")
expect("@timestamp BSP_DISPLAY s2-F;")
expect("@timestamp BSP_DISPLAY s22-EXIT;")
expect("@timestamp BSP_DISPLAY s2-EXIT;")
expect("@timestamp BSP_DISPLAY s1-ENTRY;")
expect("@timestamp BSP_DISPLAY s11-ENTRY;")
expect("@timestamp BSP_DISPLAY s11-INIT;")
expect("@timestamp BSP_DISPLAY s111-ENTRY;")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("H_SIG")
note("ignored")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("C_SIG")
expect("@timestamp BSP_DISPLAY s111-C;")
expect("@timestamp BSP_DISPLAY s111-EXIT;")
expect("@timestamp BSP_DISPLAY s112-ENTRY;")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("H_SIG")
expect("@timestamp BSP_DISPLAY s112-H;")
expect("@timestamp BSP_DISPLAY s112-EXIT;")
expect("@timestamp BSP_DISPLAY s11-EXIT;")
expect("@timestamp BSP_DISPLAY s1-EXIT;")
expect("@timestamp BSP_DISPLAY s-INIT;")
expect("@timestamp BSP_DISPLAY s1-ENTRY;")
expect("@timestamp BSP_DISPLAY s1-INIT;")
expect("@timestamp BSP_DISPLAY s11-ENTRY;")
expect("@timestamp BSP_DISPLAY s111-ENTRY;")
expect("@timestamp Trg-Done QS_RX_EVENT")

#-----------------------------------------------------------------------------
test("TUN_QP_qep_msm_05")
note("guards...")
init()
expect("@timestamp Trg-Done QS_RX_EVENT")
#
glb_filter(GRP_UA)
dispatch("D_SIG")
expect("@timestamp BSP_DISPLAY s21-D;")
expect("@timestamp BSP_DISPLAY s211-EXIT;")
expect("@timestamp BSP_DISPLAY s21-EXIT;")
expect("@timestamp BSP_DISPLAY s2-INIT;")
expect("@timestamp BSP_DISPLAY s21-ENTRY;")
expect("@timestamp BSP_DISPLAY s211-ENTRY;")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("D_SIG")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("I_SIG")
expect("@timestamp BSP_DISPLAY s2-I;")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("I_SIG")
expect("@timestamp BSP_DISPLAY s-I;")
expect("@timestamp Trg-Done QS_RX_EVENT")

#-----------------------------------------------------------------------------
test("TUN_QP_qep_msm_06")
note("history...")
init()
expect("@timestamp Trg-Done QS_RX_EVENT")
#
note("shallow history")
glb_filter(GRP_UA)
dispatch("G_SIG")
expect("@timestamp BSP_DISPLAY s21-G->H*-s1;")
expect("@timestamp BSP_DISPLAY s211-EXIT;")
expect("@timestamp BSP_DISPLAY s21-EXIT;")
expect("@timestamp BSP_DISPLAY s2-EXIT;")
expect("@timestamp BSP_DISPLAY s1-ENTRY;")
expect("@timestamp BSP_DISPLAY s11-ENTRY;")
expect("@timestamp BSP_DISPLAY s11-INIT;")
expect("@timestamp BSP_DISPLAY s111-ENTRY;")
expect("@timestamp Trg-Done QS_RX_EVENT")
command("TSTSM_IS_IN_CMD", 111) # is in state s111
expect("@timestamp COMMAND TSTSM_IS_IN_CMD 1 111")
expect("@timestamp Trg-Done QS_RX_COMMAND")

dispatch("C_SIG")
expect("@timestamp BSP_DISPLAY s111-C;")
expect("@timestamp BSP_DISPLAY s111-EXIT;")
expect("@timestamp BSP_DISPLAY s112-ENTRY;")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("C_SIG")
expect("@timestamp BSP_DISPLAY s-C;")
expect("@timestamp BSP_DISPLAY s112-EXIT;")
expect("@timestamp BSP_DISPLAY s11-EXIT;")
expect("@timestamp BSP_DISPLAY s1-EXIT;")
expect("@timestamp BSP_DISPLAY s-EXIT;")
expect("@timestamp BSP_DISPLAY t-ENTRY;")
expect("@timestamp Trg-Done QS_RX_EVENT")
command("TSTSM_IS_IN_CMD", 0) # is in state s
expect("@timestamp COMMAND TSTSM_IS_IN_CMD 0 0") # NO
expect("@timestamp Trg-Done QS_RX_COMMAND")

note("shallow history")
dispatch("C_SIG")
expect("@timestamp BSP_DISPLAY t-C->H-s;")
expect("@timestamp BSP_DISPLAY t-EXIT;")
expect("@timestamp BSP_DISPLAY s-ENTRY;")
expect("@timestamp BSP_DISPLAY s1-ENTRY;")
expect("@timestamp BSP_DISPLAY s1-INIT;")
expect("@timestamp BSP_DISPLAY s11-ENTRY;")
expect("@timestamp BSP_DISPLAY s111-ENTRY;")
expect("@timestamp Trg-Done QS_RX_EVENT")

note("deep history")
dispatch("G_SIG")
expect("@timestamp BSP_DISPLAY s11-G->H*-s2;")
expect("@timestamp BSP_DISPLAY s111-EXIT;")
expect("@timestamp BSP_DISPLAY s11-EXIT;")
expect("@timestamp BSP_DISPLAY s1-EXIT;")
expect("@timestamp BSP_DISPLAY s2-ENTRY;")
expect("@timestamp BSP_DISPLAY s21-ENTRY;")
expect("@timestamp BSP_DISPLAY s211-ENTRY;")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("C_SIG")
expect("@timestamp BSP_DISPLAY s211-C;")
expect("@timestamp BSP_DISPLAY s211-EXIT;")
expect("@timestamp BSP_DISPLAY s212-ENTRY;")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("C_SIG")
expect("@timestamp BSP_DISPLAY s-C;")
expect("@timestamp BSP_DISPLAY s212-EXIT;")
expect("@timestamp BSP_DISPLAY s21-EXIT;")
expect("@timestamp BSP_DISPLAY s2-EXIT;")
expect("@timestamp BSP_DISPLAY s-EXIT;")
expect("@timestamp BSP_DISPLAY t-ENTRY;")
expect("@timestamp Trg-Done QS_RX_EVENT")

note("deep history")
dispatch("E_SIG")
expect("@timestamp BSP_DISPLAY t-E->H*-s2;")
expect("@timestamp BSP_DISPLAY t-EXIT;")
expect("@timestamp BSP_DISPLAY s-ENTRY;")
expect("@timestamp BSP_DISPLAY s2-ENTRY;")
expect("@timestamp BSP_DISPLAY s21-ENTRY;")
expect("@timestamp BSP_DISPLAY s212-ENTRY;")
expect("@timestamp Trg-Done QS_RX_EVENT")

#-----------------------------------------------------------------------------
test("TUN_QP_qep_msm_07")
note("init")
#
glb_filter(GRP_SM, GRP_U0)
init()
expect("===RTC===> St-Init  Obj=the_TstSM,State=NULL->TstSM::s2")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s2")
expect("===RTC===> St-Init  Obj=the_TstSM,State=TstSM::s2->TstSM::s211")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s21")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s211")
expect("@timestamp Init===> Obj=the_TstSM,State=TstSM::s211")
expect("@timestamp Trg-Done QS_RX_EVENT")

#-----------------------------------------------------------------------------
test("TUN_QP_qep_msm_08", NORESET)
note("isIn")
#
command("QMSM_GET_STATE_HANDLER_CMD") # get state handler
expect("@timestamp COMMAND QMSM_GET_STATE_HANDLER_CMD TstSM::s211")
expect("@timestamp Trg-Done QS_RX_COMMAND")
#
command("TSTSM_IS_IN_CMD", 1) # is in state s1
expect("@timestamp COMMAND TSTSM_IS_IN_CMD 0 1") # NO
expect("@timestamp Trg-Done QS_RX_COMMAND")
#
command("TSTSM_IS_IN_CMD", 2) # is in state s2
expect("@timestamp COMMAND TSTSM_IS_IN_CMD 1 2") # YES
expect("@timestamp Trg-Done QS_RX_COMMAND")
#
command("TSTSM_IS_IN_CMD", 21) # is in state s21
expect("@timestamp COMMAND TSTSM_IS_IN_CMD 1 21") # YES
expect("@timestamp Trg-Done QS_RX_COMMAND")
#
command("TSTSM_IS_IN_CMD", 211) # is in state s211
expect("@timestamp COMMAND TSTSM_IS_IN_CMD 1 211") # YES
expect("@timestamp Trg-Done QS_RX_COMMAND")

#-----------------------------------------------------------------------------
test("TUN_QP_qep_msm_09", NORESET)
note("dispatch")
dispatch("A_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=A_SIG,State=TstSM::s211")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s211")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s21")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s21")
expect("===RTC===> St-Init  Obj=the_TstSM,State=TstSM::s21->TstSM::s211")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s211")
expect("@timestamp ===>Tran Obj=the_TstSM,Sig=A_SIG,State=TstSM::s21->TstSM::s211")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("D_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=D_SIG,State=TstSM::s211")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s211")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s21")
expect("===RTC===> St-Init  Obj=the_TstSM,State=TstSM::s2->TstSM::s211")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s21")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s211")
expect("@timestamp ===>Tran Obj=the_TstSM,Sig=D_SIG,State=TstSM::s21->TstSM::s211")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("B_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=B_SIG,State=TstSM::s211")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s211")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s21")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s22")
expect("@timestamp ===>Tran Obj=the_TstSM,Sig=B_SIG,State=TstSM::s2->TstSM::s22")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("F_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=F_SIG,State=TstSM::s22")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s22")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s2")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s1")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s11")
expect("===RTC===> St-Init  Obj=the_TstSM,State=TstSM::s11->TstSM::s111")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s111")
expect("@timestamp ===>Tran Obj=the_TstSM,Sig=F_SIG,State=TstSM::s2->TstSM::s111")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("H_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=H_SIG,State=TstSM::s111")
expect("@timestamp =>Ignore Obj=the_TstSM,Sig=H_SIG,State=TstSM::s111")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("C_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=C_SIG,State=TstSM::s111")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s111")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s112")
expect("@timestamp ===>Tran Obj=the_TstSM,Sig=C_SIG,State=TstSM::s111->TstSM::s112")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("H_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=H_SIG,State=TstSM::s112")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s112")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s11")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s1")
expect("===RTC===> St-Init  Obj=the_TstSM,State=TstSM::s->TstSM::s1")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s1")
expect("===RTC===> St-Init  Obj=the_TstSM,State=TstSM::s1->TstSM::s111")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s11")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s111")
expect("@timestamp ===>Tran Obj=the_TstSM,Sig=H_SIG,State=TstSM::s112->TstSM::s111")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("E_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=E_SIG,State=TstSM::s111")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s111")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s11")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s1")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s2")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s21")
expect("===RTC===> St-Init  Obj=the_TstSM,State=TstSM::s21->TstSM::s211")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s211")
expect("@timestamp ===>Tran Obj=the_TstSM,Sig=E_SIG,State=TstSM::s->TstSM::s211")
expect("@timestamp Trg-Done QS_RX_EVENT")

#-----------------------------------------------------------------------------
test("TUN_QP_qep_msm_10")
note("guards...")
init()
expect("@timestamp Trg-Done QS_RX_EVENT")
#
glb_filter(GRP_SM, GRP_U0)
dispatch("D_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=D_SIG,State=TstSM::s211")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s211")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s21")
expect("===RTC===> St-Init  Obj=the_TstSM,State=TstSM::s2->TstSM::s211")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s21")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s211")
expect("@timestamp ===>Tran Obj=the_TstSM,Sig=D_SIG,State=TstSM::s21->TstSM::s211")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("D_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=D_SIG,State=TstSM::s211")
expect("===RTC===> St-Unhnd Obj=the_TstSM,Sig=D_SIG,State=TstSM::s21")
expect("@timestamp =>Ignore Obj=the_TstSM,Sig=D_SIG,State=TstSM::s211")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("I_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=I_SIG,State=TstSM::s211")
expect("@timestamp =>Intern Obj=the_TstSM,Sig=I_SIG,State=TstSM::s2")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("I_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=I_SIG,State=TstSM::s211")
expect("===RTC===> St-Unhnd Obj=the_TstSM,Sig=I_SIG,State=TstSM::s2")
expect("@timestamp =>Intern Obj=the_TstSM,Sig=I_SIG,State=TstSM::s")
expect("@timestamp Trg-Done QS_RX_EVENT")

#-----------------------------------------------------------------------------
test("TUN_QP_qep_msm_11")
note("history...")
#
glb_filter(GRP_SM, GRP_U0)
init()
expect("===RTC===> St-Init  Obj=the_TstSM,State=NULL->TstSM::s2")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s2")
expect("===RTC===> St-Init  Obj=the_TstSM,State=TstSM::s2->TstSM::s211")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s21")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s211")
expect("@timestamp Init===> Obj=the_TstSM,State=TstSM::s211")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
note("shallow history")
dispatch("G_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=G_SIG,State=TstSM::s211")
expect("===RTC===> St-Hist  Obj=the_TstSM,State=TstSM::s21->TstSM::s11")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s211")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s21")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s2")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s1")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s11")
expect("===RTC===> St-Init  Obj=the_TstSM,State=TstSM::s11->TstSM::s111")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s111")
expect("@timestamp ===>Tran Obj=the_TstSM,Sig=G_SIG,State=TstSM::s21->TstSM::s111")
expect("@timestamp Trg-Done QS_RX_EVENT")
command("TSTSM_IS_IN_CMD", 111) # is in state s111
expect("@timestamp COMMAND TSTSM_IS_IN_CMD 1 111")
expect("@timestamp Trg-Done QS_RX_COMMAND")
#
dispatch("C_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=C_SIG,State=TstSM::s111")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s111")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s112")
expect("@timestamp ===>Tran Obj=the_TstSM,Sig=C_SIG,State=TstSM::s111->TstSM::s112")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("C_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=C_SIG,State=TstSM::s112")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s112")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s11")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s1")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::t")
expect("@timestamp ===>Tran Obj=the_TstSM,Sig=C_SIG,State=TstSM::s->TstSM::t")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
note("shallow history")
dispatch("C_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=C_SIG,State=TstSM::t")
expect("===RTC===> St-Hist  Obj=the_TstSM,State=TstSM::t->TstSM::s1")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::t")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s1")
expect("===RTC===> St-Init  Obj=the_TstSM,State=TstSM::s1->TstSM::s111")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s11")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s111")
expect("@timestamp ===>Tran Obj=the_TstSM,Sig=C_SIG,State=TstSM::t->TstSM::s111")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
note("deep history")
dispatch("G_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=G_SIG,State=TstSM::s111")
expect("===RTC===> St-Hist  Obj=the_TstSM,State=TstSM::s11->TstSM::s211")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s111")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s11")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s1")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s2")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s21")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s211")
expect("@timestamp ===>Tran Obj=the_TstSM,Sig=G_SIG,State=TstSM::s11->TstSM::s211")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("C_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=C_SIG,State=TstSM::s211")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s211")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s212")
expect("@timestamp ===>Tran Obj=the_TstSM,Sig=C_SIG,State=TstSM::s211->TstSM::s212")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
dispatch("C_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=C_SIG,State=TstSM::s212")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s212")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s21")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s2")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::s")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::t")
expect("@timestamp ===>Tran Obj=the_TstSM,Sig=C_SIG,State=TstSM::s->TstSM::t")
expect("@timestamp Trg-Done QS_RX_EVENT")
#
note("deep history")
dispatch("E_SIG")
expect("@timestamp Disp===> Obj=the_TstSM,Sig=E_SIG,State=TstSM::t")
expect("===RTC===> St-Hist  Obj=the_TstSM,State=TstSM::t->TstSM::s212")
expect("===RTC===> St-Exit  Obj=the_TstSM,State=TstSM::t")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s2")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s21")
expect("===RTC===> St-Entry Obj=the_TstSM,State=TstSM::s212")
expect("@timestamp ===>Tran Obj=the_TstSM,Sig=E_SIG,State=TstSM::t->TstSM::s212")
expect("@timestamp Trg-Done QS_RX_EVENT")
