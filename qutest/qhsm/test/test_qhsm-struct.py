# test-script for QUTest unit testing harness
# see https://www.state-machine.com/qtools/qutest.html

# preamble...
def on_reset():
    expect_run()
    glb_filter(GRP_SM, GRP_U1)
    current_obj(OBJ_SM, "the_sm")

# tests...
#==========================================================
test("QHsmTst init")
init()
expect("===RTC===> St-Init  Obj=the_sm,State=QP::QHsm::top->QHsmTst::s2")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s2")
expect("===RTC===> St-Init  Obj=the_sm,State=QHsmTst::s2->QHsmTst::s211")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s21")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s211")
expect("@timestamp Init===> Obj=the_sm,State=QHsmTst::s211")
expect("@timestamp Trg-Done QS_RX_EVENT")

#----------------------------------------------------------
test("QHsmTst isIn", NORESET)
command(0) # get state handler
expect("@timestamp CMD QHsmTst::s211")
expect("@timestamp Trg-Done QS_RX_COMMAND")

command(1, 1) # is in state s1
expect("@timestamp CMD 0 1") # NO
expect("@timestamp Trg-Done QS_RX_COMMAND")

command(1, 2) # is in state s2
expect("@timestamp CMD 1 2") # YES
expect("@timestamp Trg-Done QS_RX_COMMAND")

command(1, 21) # is in state s21
expect("@timestamp CMD 1 21") # YES
expect("@timestamp Trg-Done QS_RX_COMMAND")

command(1, 211) # is in state s211
expect("@timestamp CMD 1 211") # YES
expect("@timestamp Trg-Done QS_RX_COMMAND")

#----------------------------------------------------------
test("QHsmTst dispatch", NORESET)
dispatch("A_SIG")
expect("@timestamp Disp===> Obj=the_sm,Sig=A_SIG,State=QHsmTst::s211")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s211")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s21")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s21")
expect("===RTC===> St-Init  Obj=the_sm,State=QHsmTst::s21->QHsmTst::s211")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s211")
expect("@timestamp ===>Tran Obj=the_sm,Sig=A_SIG,State=QHsmTst::s21->QHsmTst::s211")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("B_SIG")
expect("@timestamp Disp===> Obj=the_sm,Sig=B_SIG,State=QHsmTst::s211")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s211")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s211")
expect("@timestamp ===>Tran Obj=the_sm,Sig=B_SIG,State=QHsmTst::s21->QHsmTst::s211")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("C_SIG")
expect("@timestamp Disp===> Obj=the_sm,Sig=C_SIG,State=QHsmTst::s211")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s211")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s212")
expect("@timestamp ===>Tran Obj=the_sm,Sig=C_SIG,State=QHsmTst::s211->QHsmTst::s212")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("D_SIG")
expect("@timestamp Disp===> Obj=the_sm,Sig=D_SIG,State=QHsmTst::s212")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s212")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s21")
expect("===RTC===> St-Init  Obj=the_sm,State=QHsmTst::s2->QHsmTst::s211")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s21")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s211")
expect("@timestamp ===>Tran Obj=the_sm,Sig=D_SIG,State=QHsmTst::s21->QHsmTst::s211")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("E_SIG")
expect("@timestamp Disp===> Obj=the_sm,Sig=E_SIG,State=QHsmTst::s211")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s211")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s21")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s22")
expect("@timestamp ===>Tran Obj=the_sm,Sig=E_SIG,State=QHsmTst::s2->QHsmTst::s22")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("F_SIG")
expect("@timestamp Disp===> Obj=the_sm,Sig=F_SIG,State=QHsmTst::s22")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s22")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s2")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s1")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s11")
expect("===RTC===> St-Init  Obj=the_sm,State=QHsmTst::s11->QHsmTst::s111")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s111")
expect("@timestamp ===>Tran Obj=the_sm,Sig=F_SIG,State=QHsmTst::s2->QHsmTst::s111")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("H_SIG")
expect("@timestamp Disp===> Obj=the_sm,Sig=H_SIG,State=QHsmTst::s111")
expect("@timestamp =>Ignore Obj=the_sm,Sig=H_SIG,State=QHsmTst::s111")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("C_SIG")
expect("@timestamp Disp===> Obj=the_sm,Sig=C_SIG,State=QHsmTst::s111")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s111")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s112")
expect("@timestamp ===>Tran Obj=the_sm,Sig=C_SIG,State=QHsmTst::s111->QHsmTst::s112")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("H_SIG")
expect("@timestamp Disp===> Obj=the_sm,Sig=H_SIG,State=QHsmTst::s112")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s112")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s11")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s1")
expect("===RTC===> St-Init  Obj=the_sm,State=QHsmTst::s->QHsmTst::s1")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s1")
expect("===RTC===> St-Init  Obj=the_sm,State=QHsmTst::s1->QHsmTst::s111")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s11")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s111")
expect("@timestamp ===>Tran Obj=the_sm,Sig=H_SIG,State=QHsmTst::s112->QHsmTst::s111")
expect("@timestamp Trg-Done QS_RX_EVENT")

#==========================================================
test("QHsmTst guards")
init()
expect("===RTC===> St-Init  Obj=the_sm,State=QP::QHsm::top->QHsmTst::s2")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s2")
expect("===RTC===> St-Init  Obj=the_sm,State=QHsmTst::s2->QHsmTst::s211")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s21")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s211")
expect("@timestamp Init===> Obj=the_sm,State=QHsmTst::s211")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("D_SIG")
expect("@timestamp Disp===> Obj=the_sm,Sig=D_SIG,State=QHsmTst::s211")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s211")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s21")
expect("===RTC===> St-Init  Obj=the_sm,State=QHsmTst::s2->QHsmTst::s211")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s21")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s211")
expect("@timestamp ===>Tran Obj=the_sm,Sig=D_SIG,State=QHsmTst::s21->QHsmTst::s211")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("D_SIG")
expect("@timestamp Disp===> Obj=the_sm,Sig=D_SIG,State=QHsmTst::s211")
expect("===RTC===> St-Unhnd Obj=the_sm,Sig=D_SIG,State=QHsmTst::s21")
expect("@timestamp =>Ignore Obj=the_sm,Sig=D_SIG,State=QHsmTst::s211")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("I_SIG")
expect("@timestamp Disp===> Obj=the_sm,Sig=I_SIG,State=QHsmTst::s211")
expect("@timestamp =>Intern Obj=the_sm,Sig=I_SIG,State=QHsmTst::s2")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("I_SIG")
expect("@timestamp Disp===> Obj=the_sm,Sig=I_SIG,State=QHsmTst::s211")
expect("===RTC===> St-Unhnd Obj=the_sm,Sig=I_SIG,State=QHsmTst::s2")
expect("@timestamp =>Intern Obj=the_sm,Sig=I_SIG,State=QHsmTst::s")
expect("@timestamp Trg-Done QS_RX_EVENT")

#==========================================================
test("QHsmTst history")
init()
expect("===RTC===> St-Init  Obj=the_sm,State=QP::QHsm::top->QHsmTst::s2")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s2")
expect("===RTC===> St-Init  Obj=the_sm,State=QHsmTst::s2->QHsmTst::s211")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s21")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s211")
expect("@timestamp Init===> Obj=the_sm,State=QHsmTst::s211")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("G_SIG")
expect("@timestamp Disp===> Obj=the_sm,Sig=G_SIG,State=QHsmTst::s211")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s211")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s21")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s2")
expect("===RTC===> St-Hist  Obj=the_sm,State=QHsmTst::s21->QHsmTst::s111")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s1")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s11")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s111")
expect("@timestamp ===>Tran Obj=the_sm,Sig=G_SIG,State=QHsmTst::s21->QHsmTst::s111")
expect("@timestamp Trg-Done QS_RX_EVENT")
command(1, 111) # is in state s111
expect("@timestamp CMD 1 111")
expect("@timestamp Trg-Done QS_RX_COMMAND")

dispatch("C_SIG")
expect("@timestamp Disp===> Obj=the_sm,Sig=C_SIG,State=QHsmTst::s111")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s111")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s112")
expect("@timestamp ===>Tran Obj=the_sm,Sig=C_SIG,State=QHsmTst::s111->QHsmTst::s112")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("C_SIG")
expect("@timestamp Disp===> Obj=the_sm,Sig=C_SIG,State=QHsmTst::s112")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s112")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s11")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s1")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::t")
expect("@timestamp ===>Tran Obj=the_sm,Sig=C_SIG,State=QHsmTst::s->QHsmTst::t")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("C_SIG")
expect("@timestamp Disp===> Obj=the_sm,Sig=C_SIG,State=QHsmTst::t")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::t")
expect("===RTC===> St-Hist  Obj=the_sm,State=QHsmTst::t->QHsmTst::s1")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s1")
expect("===RTC===> St-Init  Obj=the_sm,State=QHsmTst::s1->QHsmTst::s111")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s11")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s111")
expect("@timestamp ===>Tran Obj=the_sm,Sig=C_SIG,State=QHsmTst::t->QHsmTst::s111")
expect("@timestamp Trg-Done QS_RX_EVENT")

dispatch("G_SIG")
expect("@timestamp Disp===> Obj=the_sm,Sig=G_SIG,State=QHsmTst::s111")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s111")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s11")
expect("===RTC===> St-Exit  Obj=the_sm,State=QHsmTst::s1")
expect("===RTC===> St-Hist  Obj=the_sm,State=QHsmTst::s11->QHsmTst::s211")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s2")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s21")
expect("===RTC===> St-Entry Obj=the_sm,State=QHsmTst::s211")
expect("@timestamp ===>Tran Obj=the_sm,Sig=G_SIG,State=QHsmTst::s11->QHsmTst::s211")
expect("@timestamp Trg-Done QS_RX_EVENT")
