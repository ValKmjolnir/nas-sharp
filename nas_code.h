#ifndef __NAS_CODE_H__
#define __NAS_CODE_H__

struct bytecode
{
    unsigned char type;
    unsigned char reg1;
    unsigned char reg2;
    unsigned char reg3;
};

// 1*2*3-1/2*4+3;
// set reg1 1
// set reg2 2
// mult reg1 reg1 reg2
// set reg2 3
// mult reg1 reg1 reg2
// set reg2 1
// set reg3 2
// div reg2 reg2 reg3
// set reg3 4
// mult reg2 reg2 reg3
// minus reg1 reg1 reg2
// set reg2 3
// plus reg1 reg1 reg2

// pushvec reg1 reg2
bool reg[64];
std::vector<bytecode> exec_code;

void proc_gen(nas_ast& root)
{
    exec_code.clear();
    ;
    return;
}

#endif