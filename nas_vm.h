#ifndef __NAS_VM_H__
#define __NAS_VM_H__

void opr_nop();
void opr_nil();
void opr_pushn();
void opr_pushs();
void opr_pushv();
void opr_pushh();
void opr_pushf();
void opr_vapp();
void opr_happ();
void opr_para();
void opr_dynpara();
void opr_entry();
void opr_load();
void opr_call();
void opr_callb();
void opr_callv();
void opr_callh();
void opr_callf();
void opr_mcall();
void opr_mcallv();
void opr_mcallh();
void opr_mcallf();
void opr_neg();
void opr_not();
void opr_plus();
void opr_minus();
void opr_mult();
void opr_div();
void opr_lnk();
void opr_meq();
void opr_pluseq();
void opr_minuseq();
void opr_lnkeq();
void opr_multeq();
void opr_diveq();
void opr_eq();
void opr_neq();
void opr_less();
void opr_leq();
void opr_grt();
void opr_geq();
void opr_pop();
void opr_jmp();
void opr_jt();
void opr_jf();
void opr_ret();

void (*func)()[]=
{
    &opr_nop,
    &opr_nil,
    &opr_pushn,
    &opr_pushs,
    &opr_pushv,
    &opr_pushh,
    &opr_pushf,
    &opr_vapp,
    &opr_happ,
    &opr_para,
    &opr_dynpara,
    &opr_entry,
    &opr_load,
    &opr_call,
    &opr_callb,
    &opr_callv,
    &opr_callh,
    &opr_callf,
    &opr_mcall,
    &opr_mcallv,
    &opr_mcallh,
    &opr_mcallf,
    &opr_neg,
    &opr_not,
    &opr_plus,
    &opr_minus,
    &opr_mult,
    &opr_div,
    &opr_lnk,
    &opr_meq,
    &opr_pluseq,
    &opr_minuseq,
    &opr_lnkeq,
    &opr_multeq,
    &opr_diveq,
    &opr_eq,
    &opr_neq,
    &opr_less,
    &opr_leq,
    &opr_grt,
    &opr_geq,
    &opr_pop,
    &opr_jmp,
    &opr_jt,
    &opr_jf,
    &opr_ret,
};

#endif