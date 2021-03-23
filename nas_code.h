#ifndef __NAS_CODE_H__
#define __NAS_CODE_H__

enum opcode_type
{
    // loop end mark
    op_nop=0,
    // push value
    op_nil,
    op_pushn,
    op_pushs,
    op_pushv,
    op_pushh,
    op_pushf,
    op_vapp,
    op_happ,
    op_para,
    op_dynpara,
    // load
    op_loadg,
    op_loadl,
    // call
    op_callg,
    op_calll,
    op_callv,
    op_callh,
    op_callf,
    // mem_call
    op_mcallg,
    op_mcalll,
    op_mcallv,
    op_mcallh,
    // operator
    op_neg,
    op_not,
    op_plus,
    op_minus,
    op_mult,
    op_div,
    op_lnk,
    // assignment
    op_meq,
    op_pluseq,
    op_minuseq,
    op_lnkeq,
    op_multeq,
    op_diveq,
    // compare operator
    op_eq,
    op_neq,
    op_less,
    op_leq,
    op_grt,
    op_geq,
    // stack operator
    op_pop,
    // branch
    op_jmp,
    op_jt,
    op_jf,
    // function return
    op_ret
};

struct
{
    char type;
    const char* content;
}code_table[]=
{
    {op_nop,     "nop   "},
    {op_nil,     "nil   "},
    {op_pushn,   "pushn "},
    {op_pushs,   "pushs "},
    {op_pushv,   "pushv "},
    {op_pushh,   "pushh "},
    {op_pushf,   "pushf "},
    {op_vapp,    "vapp  "},
    {op_happ,    "happ  "},
    {op_para,    "para  "},
    {op_dynpara, "dynp  "},
    {op_loadg,   "loadg "},
    {op_loadl,   "loadl "},
    {op_callg,   "callg "},
    {op_calll,   "calll "},
    {op_callv,   "callv "},
    {op_callh,   "callh "},
    {op_callf,   "callf "},
    {op_mcallg,  "mcallg"},
    {op_mcalll,  "mcalll"},
    {op_mcallv,  "mcallv"},
    {op_mcallh,  "mcallh"},
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
    {op_jt,      "jt    "},
    {op_jf,      "jf    "},
    {op_ret,     "ret   "},
    {-1,         NULL    }
};

struct bytecode
{
    unsigned char op;
    unsigned int num;
};

std::vector<bytecode>
    exec_code;
std::unordered_map<std::string,unsigned int>
    string_table;
std::unordered_map<double,unsigned int>
    number_table;
std::list<std::vector<int> >
    continue_ptr;
std::list<std::vector<int> >
    break_ptr;
std::vector<std::string>
    global_symbol;
std::list<std::vector<std::string> >
    local_symbol;

void code_print()
{
    int size=exec_code.size();
    for(int i=0;i<size;++i)
    {
        printf("0x%.8x ",i);
        for(int j=0;code_table[j].content;++j)
            if(exec_code[i].op==code_table[j].type)
            {
                printf("%s ",code_table[j].content);
                break;
            }
        printf("0x%.8x\n",exec_code[i].num);
    }
    return;
}
void regist_str(std::string);
void regist_num(double);
void global_addsym(std::string);
void local_addsym(std::string);
bool in_global(std::string);
bool in_local(std::string);
void emit(unsigned char,unsigned int);
void regist_builtin();
void proc_gen(nas_ast&);
void blk_gen(nas_ast&);
void expr_gen(nas_ast&);
void calc_gen(nas_ast&);
void num_gen(nas_ast&);
void str_gen(nas_ast&);
void vec_gen(nas_ast&);
void hash_gen(nas_ast&);
void func_gen(nas_ast&);
void call_gen(nas_ast&);
void mcall_gen(nas_ast&);
void def_gen(nas_ast&);
void if_gen(nas_ast&);
void while_gen(nas_ast&);
void for_gen(nas_ast&);
void ret_gen(nas_ast&);
void and_gen(nas_ast&);
void or_gen(nas_ast&);

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
void global_addsym(std::string sym)
{
    regist_str(sym);
    for(int i=0;i<global_symbol.size();++i)
        if(global_symbol[i]==sym)
            return;
    global_symbol.push_back(sym);
    return;
}
void local_addsym(std::string sym)
{
    regist_str(sym);
    for(int i=0;i<local_symbol.front().size();++i)
        if(local_symbol.front()[i]==sym)
            return;
    local_symbol.front().push_back(sym);
    return;
}
bool in_global(std::string sym)
{
    for(int i=0;i<global_symbol.size();++i)
        if(global_symbol[i]==sym)
            return true;
    return false;
}
bool in_local(std::string sym)
{
    for(auto i=local_symbol.begin();i!=local_symbol.end();++i)
        for(int j=0;j<i->size();++j)
            if((*i)[j]==sym)
                return true;
    return false;
}
void emit(unsigned char op,unsigned int num=0)
{
    bytecode tmp;
    tmp.op=op;
    tmp.num=num;
    exec_code.push_back(tmp);
    return;
}
void regist_builtin()
{
    for(int i=0;builtin_func[i].func_name;++i)
        global_addsym(builtin_func[i].func_name);
    return;
}
void proc_gen(nas_ast& root)
{
    global_symbol.clear();
    local_symbol.clear();
    regist_builtin();
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
        case ast_definition:
            def_gen(node);
            break;
        case ast_nil:
        case ast_num:
        case ast_str:
        case ast_func:break;
        case ast_list:
        case ast_hash:
        case ast_call:
        case ast_plus:
        case ast_minus:
        case ast_mult:
        case ast_div:
        case ast_link:
        case ast_and:
        case ast_or:
        case ast_not:
        case ast_cmpeq:
        case ast_neq:
        case ast_less:
        case ast_leq:
        case ast_grt:
        case ast_geq:
        case ast_eq:
        case ast_pluseq:
        case ast_minuseq:
        case ast_linkeq:
        case ast_multeq:
        case ast_diveq:
            calc_gen(node);
            emit(op_pop);
            break;
        case ast_conditional:
            if_gen(node);
            break;
        case ast_while:
            while_gen(node);
            break;
        case ast_for:
            for_gen(node);
            break;
        case ast_continue:
            continue_ptr.front().push_back(exec_code.size());
            emit(op_jmp);
            break;
        case ast_break:
            break_ptr.front().push_back(exec_code.size());
            emit(op_jmp);
            break;
        case ast_ret:
            ret_gen(node);
            break;
    }
    return;
}
void calc_gen(nas_ast& node)
{
    switch(node.get_type())
    {
        case ast_nil:  emit(op_nil);   break;
        case ast_id:
        case ast_call: call_gen(node); break;
        case ast_num:  num_gen(node);  break;
        case ast_str:  str_gen(node);  break;
        case ast_list: vec_gen(node);  break;
        case ast_hash: hash_gen(node); break;
        case ast_func: func_gen(node); break;
        case ast_eq:
        case ast_pluseq:
        case ast_minuseq:
        case ast_linkeq:
        case ast_multeq:
        case ast_diveq:
            mcall_gen(node.get_children()[0]);
            calc_gen(node.get_children()[1]);
            emit(node.get_type()-ast_eq+op_meq);
            break;
        case ast_not:
            calc_gen(node.get_children()[0]);
            emit(op_not);
            break;
        case ast_minus:
            if(node.get_children().size()==1)
            {
                calc_gen(node.get_children()[0]);
                emit(op_neg);
            }
            else
            {
                calc_gen(node.get_children()[0]);
                calc_gen(node.get_children()[1]);
                emit(op_minus);
            }
            break;
        case ast_plus:
        case ast_mult:
        case ast_div:
        case ast_link:
            calc_gen(node.get_children()[0]);
            calc_gen(node.get_children()[1]);
            switch(node.get_type())
            {
                case ast_plus:emit(op_plus);break;
                case ast_mult:emit(op_mult);break;
                case ast_div: emit(op_div); break;
                case ast_link:emit(op_lnk); break;
            }
            break;
        case ast_cmpeq:
        case ast_neq:
        case ast_less:
        case ast_leq:
        case ast_grt:
        case ast_geq:
            calc_gen(node.get_children()[0]);
            calc_gen(node.get_children()[1]);
            emit(node.get_type()-ast_cmpeq+op_eq);
            break;
        case ast_and: and_gen(node); break;
        case ast_or:  or_gen(node);  break;
    }
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
    int pushf_label=exec_code.size();
    emit(op_pushf,0);
    std::vector<nas_ast>& parameter=node.get_children()[0].get_children();
    std::vector<std::string> new_local;
    local_symbol.push_front(new_local);
    for(int i=0;i<parameter.size();++i)
    {
        local_addsym(parameter[i].get_str());
        emit(
            parameter[i].get_type()==ast_para?op_para:op_dynpara,
            string_table[parameter[i].get_str()]
        );
    }
    exec_code[pushf_label].num=exec_code.size()+1;
    emit(op_jmp);
    int jmp_label=exec_code.size()-1;
    blk_gen(node.get_children()[1]);
    if(exec_code.back().op!=op_ret)
    {
        emit(op_nil);
        emit(op_ret);
    }
    local_symbol.pop_front();
    exec_code[jmp_label].num=exec_code.size();
    return;
}
void call_gen(nas_ast& node)
{
    if(node.get_type()==ast_id)
    {
        if(in_local(node.get_str()))
            emit(op_calll,string_table[node.get_str()]);
        else if(in_global(node.get_str()))
            emit(op_callg,string_table[node.get_str()]);
        else
            die("cannot find symbol named \""+node.get_str()+"\".",node.get_line());
        return;
    }
    std::vector<nas_ast>& calls=node.get_children();
    switch(calls[0].get_type())
    {
        case ast_id:  call_gen(calls[0]);break;
        case ast_str: str_gen(calls[0]); break;
        case ast_list:vec_gen(calls[0]); break;
        case ast_hash:hash_gen(calls[0]);break;
        case ast_func:func_gen(calls[0]);break;
    }
    for(int i=1;i<calls.size();++i)
    {
        switch(calls[i].get_type())
        {
            case ast_callv:
                calc_gen(calls[i].get_children()[0]);
                emit(op_callv);
                break;
            case ast_callh:
                regist_str(calls[i].get_str());
                emit(op_callh,string_table[calls[i].get_str()]);
                break;
            case ast_callf:
                vec_gen(calls[i]);
                emit(op_callf);
                break;
        }
    }
    return;
}
void mcall_gen(nas_ast& node)
{
    if(node.get_type()==ast_id)
    {
        if(in_local(node.get_str()))
            emit(op_mcalll,string_table[node.get_str()]);
        else if(in_global(node.get_str()))
            emit(op_mcallg,string_table[node.get_str()]);
        else
            die("cannot find symbol named \""+node.get_str()+"\".",node.get_line());
        return;
    }
    std::vector<nas_ast>& calls=node.get_children();
    mcall_gen(calls[0]);
    for(int i=1;i<calls.size();++i)
    {
        switch(calls[i].get_type())
        {
            case ast_callv:
                calc_gen(calls[i].get_children()[0]);
                emit(op_mcallv);
                break;
            case ast_callh:
                regist_str(calls[i].get_str());
                emit(op_mcallh,string_table[calls[i].get_str()]);
                break;
        }
    }
    return;
}
void def_gen(nas_ast& node)
{
    if(local_symbol.empty())
        global_addsym(node.get_str());
    else
        local_addsym(node.get_str());
    calc_gen(node.get_children()[0]);
    emit(local_symbol.empty()?op_loadg:op_loadl,string_table[node.get_str()]);
    return;
}
void if_gen(nas_ast& node)
{
    nas_ast& if_node=node.get_children()[0];
    calc_gen(if_node.get_children()[0]);
    emit(op_jf);
    int condition_jmplabel=exec_code.size()-1;
    blk_gen(if_node.get_children()[1]);
    exec_code[condition_jmplabel].num=exec_code.size();
    if(node.get_children().size()==2)
    {
        emit(op_jmp);
        exec_code[condition_jmplabel].num=exec_code.size();
        int jmp_out_label=exec_code.size()-1;
        nas_ast& else_node=node.get_children()[1];
        blk_gen(else_node.get_children()[0]);
        exec_code[jmp_out_label].num=exec_code.size();
    }
    return;
}
void while_gen(nas_ast& node)
{
    if(node.get_children()[1].get_children()[0].get_type()==ast_null)
        return;
    std::vector<int> new_labels;
    continue_ptr.push_front(new_labels);
    break_ptr.push_front(new_labels);

    int jmp_label=exec_code.size();
    calc_gen(node.get_children()[0]);
    int jf_label=exec_code.size();
    emit(op_jf);
    blk_gen(node.get_children()[1]);
    emit(op_jmp,jmp_label);
    exec_code[jf_label].num=exec_code.size();

    for(int i=0;i<continue_ptr.front().size();++i)
        exec_code[continue_ptr.front()[i]].num=jmp_label;
    for(int i=0;i<break_ptr.front().size();++i)
        exec_code[break_ptr.front()[i]].num=exec_code.size();
    continue_ptr.pop_front();
    break_ptr.pop_front();
    return;
}
void for_gen(nas_ast& node)
{
    nas_ast& before_loop=node.get_children()[0];
    nas_ast& condition=node.get_children()[1];
    nas_ast& after_loop=node.get_children()[2];

    if(before_loop.get_type()==ast_definition)
        def_gen(before_loop);
    else if(before_loop.get_type()!=ast_null)
    {
        calc_gen(before_loop);
        emit(op_pop);
    }
    if(node.get_children()[3].get_children()[0].get_type()==ast_null)
        return;
    
    std::vector<int> new_labels;
    continue_ptr.push_front(new_labels);
    break_ptr.push_front(new_labels);

    int jmp_label=exec_code.size();
    if(condition.get_type()==ast_null)
    {
        regist_num(1);
        emit(op_pushn,number_table[1]);
    }
    else
        calc_gen(condition);
    int jmp_false=exec_code.size();
    emit(op_jf);
    blk_gen(node.get_children()[3]);
    int continue_label=exec_code.size();
    if(after_loop.get_type()!=ast_null)
    {
        calc_gen(after_loop);
        emit(op_pop);
    }
    emit(op_jmp,jmp_label);

    exec_code[jmp_false].num=exec_code.size();

    for(int i=0;i<continue_ptr.front().size();++i)
        exec_code[continue_ptr.front()[i]].num=continue_label;
    for(int i=0;i<break_ptr.front().size();++i)
        exec_code[break_ptr.front()[i]].num=exec_code.size();
    continue_ptr.pop_front();
    break_ptr.pop_front();
    return;
}
void ret_gen(nas_ast& node)
{
    if(!node.get_children().size())
        emit(op_nil);
    else
        calc_gen(node.get_children()[0]);
    emit(op_ret);
    return;
}
void and_gen(nas_ast& node)
{
    calc_gen(node.get_children()[0]);
    int jf_label1=exec_code.size();
    emit(op_jf);
    calc_gen(node.get_children()[1]);
    int jf_label2=exec_code.size();
    emit(op_jf);
    regist_num(1);
    emit(op_pushn,number_table[1]);
    int jmp_label=exec_code.size();
    emit(op_jmp);
    exec_code[jf_label1].num=exec_code[jf_label2].num=exec_code.size();
    emit(op_nil);
    exec_code[jmp_label].num=exec_code.size();
    return;
}
void or_gen(nas_ast& node)
{
    calc_gen(node.get_children()[0]);
    int jt_label1=exec_code.size();
    emit(op_jt);
    calc_gen(node.get_children()[1]);
    int jt_label2=exec_code.size();
    emit(op_jt);
    emit(op_nil);
    int jmp_label=exec_code.size();
    emit(op_jmp);
    exec_code[jt_label1].num=exec_code[jt_label2].num=exec_code.size();
    regist_num(1);
    emit(op_pushn,number_table[1]);
    exec_code[jmp_label].num=exec_code.size();
    return;
}
#endif