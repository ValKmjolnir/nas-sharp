#ifndef __NAS_CODE_H__
#define __NAS_CODE_H__

enum opcode_type
{
    op_nop,
    op_pushn,
    op_pushs,
    op_pushv,
    op_pushh,
    op_pushf,
    op_vapp,
    op_happ,

    op_call,
    op_callv,
    op_callh,
    op_callf,

    op_mcall,
    op_mcallv,
    op_mcallh,
    op_mcallf,

    op_neg,
    op_not,
    op_plus,
    op_minus,
    op_mult,
    op_div,
    op_lnk,

    op_meq,
    op_pluseq,
    op_minuseq,
    op_multeq,
    op_diveq,
    op_lnkeq,

    op_eq,
    op_neq,
    op_less,
    op_leq,
    op_grt,
    op_geq,

    op_pop,

    op_jmp,
    op_jt,
    op_jf,
};

struct
{
    char type;
    const char* content;
}code_table[]=
{
    {op_nop,     "nop   "},
    {op_pushn,   "pushn "},
    {op_pushs,   "pushs "},
    {op_pushv,   "pushv "},
    {op_pushh,   "pushh "},
    {op_pushf,   "pushf "},
    {op_vapp,    "vapp  "},
    {op_happ,    "happ  "},
    {op_call,    "call  "},
    {op_callv,   "callv "},
    {op_callh,   "callh "},
    {op_callf,   "callf "},
    {op_mcall,   "mcall "},
    {op_mcallv,  "mcallv"},
    {op_mcallh,  "mcallh"},
    {op_mcallf,  "mcallf"},
    {op_neg,     "neg   "},
    {op_not,     "not   "},
    {op_plus,    "plus  "},
    {op_minus,   "minus "},
    {op_mult,    "mult  "},
    {op_div,     "div   "},
    {op_lnk,     "lnk   "},
    {op_meq,     "meq   "},
    {op_pluseq,  "pluseq"},
    {op_minuseq, "subeq "},
    {op_multeq,  "multeq"},
    {op_diveq,   "diveq "},
    {op_lnkeq,   "lnkeq "},
    {op_eq,      "eq    "},
    {op_neq,     "neq   "},
    {op_less,    "less  "},
    {op_leq,     "leq   "},
    {op_grt,     "grt   "},
    {op_geq,     "geq   "},
    {op_pop,     "pop   "},
    {op_jmp,     "jmp   "},
    {op_jt,      "jtrue "},
    {op_jf,      "jfalse"},
    {-1,         NULL    }
};

struct bytecode
{
    unsigned char op;
    unsigned int num;
};

std::vector<bytecode> exec_code;
std::map<std::string,unsigned int> string_table;
std::map<double,unsigned int> number_table;

void code_print()
{
    int size=exec_code.size();
    for(int i=0;i<size;++i)
    {
        std::string tmp;
        for(int j=0;code_table[j].content;++j)
            if(exec_code[i].op==code_table[j].type)
            {
                tmp=code_table[j].content;
                break;
            }
        printf("0x%.8x %s 0x%.8x\n",i,tmp.data(),exec_code[i].num);
    }
    return;
}
void regist_str(std::string);
void regist_num(double);
void emit(unsigned char,unsigned int);
void proc_gen(nas_ast&);
void blk_gen(nas_ast&);
void expr_gen(nas_ast&);
void calc_gen(nas_ast&);
void id_gen(nas_ast&);
void num_gen(nas_ast&);
void str_gen(nas_ast&);
void vec_gen(nas_ast&);
void hash_gen(nas_ast&);
void func_gen(nas_ast&);
void if_gen(nas_ast&);
void while_gen(nas_ast&);
void for_gen(nas_ast&);

void regist_str(std::string str)
{
    if(string_table.find(str)==string_table.end())
    {
        int size=string_table.size();
        string_table[str]=size;
    }
    return;
}
void regist_num(double num)
{
    if(number_table.find(num)==number_table.end())
    {
        int size=number_table.size();
        number_table[num]=size;
    }
    return;
}
void emit(unsigned char op,unsigned int num=0)
{
    bytecode tmp;
    tmp.op=op;
    tmp.num=num;
    exec_code.push_back(tmp);
    return;
}
void proc_gen(nas_ast& root)
{
    exec_code.clear();
    std::vector<nas_ast>& exprs=root.get_children();
    int size=exprs.size();
    for(int i=0;i<size;++i)
        expr_gen(exprs[i]);
    emit(op_nop);
    return;
}
void blk_gen(nas_ast& node)
{
    std::vector<nas_ast>& exprs=node.get_children();
    int size=exprs.size();
    for(int i=0;i<size;++i)
        expr_gen(exprs[i]);
    return;
}
void expr_gen(nas_ast& node)
{
    switch(node.get_type())
    {
        case ast_definition:break;
        case ast_call:
        case ast_plus:case ast_minus:case ast_mult:case ast_div:case ast_link:
        case ast_not:case ast_cmpeq:case ast_neq:case ast_less:case ast_leq:case ast_grt:case ast_geq:
        case ast_eq:case ast_pluseq:case ast_minuseq:case ast_multeq:case ast_diveq:case ast_linkeq:
            calc_gen(node);
            emit(op_pop);
            break;
        case ast_conditional:break;
        case ast_while:break;
        case ast_for:break;
        case ast_continue:break;
        case ast_break:break;
    }
    return;
}
void calc_gen(nas_ast& node)
{
    return;
}
void id_gen(nas_ast& node)
{

    return;
}
void num_gen(nas_ast& node)
{
    regist_num(node.get_num());
    emit(op_pushn,number_table[node.get_num()]);
    return;
}
void str_gen(nas_ast& node)
{
    regist_str(node.get_str());
    emit(op_pushs,string_table[node.get_str()]);
    return;
}
void vec_gen(nas_ast& node)
{
    emit(op_pushv,0);
    std::vector<nas_ast>& members=node.get_children();
    for(int i=0;i<members.size();++i)
    {
        calc_gen(members[i]);
        emit(op_vapp);
    }
    return;
}
void hash_gen(nas_ast& node)
{
    emit(op_pushh,0);
    std::vector<nas_ast>& members=node.get_children();
    for(int i=0;i<members.size();++i)
    {
        std::string name=members[i].get_str();
        regist_str(name);
        calc_gen(members[i].get_children()[0]);
        emit(op_happ,string_table[name]);
    }
    return;
}
void func_gen(nas_ast& node)
{
    emit(op_pushf,0);
    //
    return;
}
void if_gen(nas_ast& node)
{

    return;
}
void while_gen(nas_ast& node)
{

    return;
}
void for_gen(nas_ast& node)
{
    return;
}

#endif