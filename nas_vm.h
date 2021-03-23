#ifndef __NAS_VM_H__
#define __NAS_VM_H__

// loop mark
bool main_loop_continue_mark;
// constant pool
std::vector<nas_val*>    rt_num_table;
std::vector<std::string> rt_str_table;
// program counter
int pc;
// memory
std::stack<nas_val**> mem_stack;
// function call
std::stack<unsigned int> return_address;

void runtime_error(std::string opname,std::string info)
{
    main_loop_continue_mark=false;
    std::cout<<">> [vm] "<<opname<<": "<<info<<".\n";
    return;
}
void opr_nop()
{
    main_loop_continue_mark=false;
    return;
}
void opr_nil()
{
    *(++stack_top)=*val_stack;
    return;
}
void opr_pushn()
{
    *(++stack_top)=rt_num_table[exec_code[pc].num];
    return;
}
void opr_pushs()
{
    *(++stack_top)=gc_alloc(vm_str);
    *(*stack_top)->ptr.str=rt_str_table[exec_code[pc].num];
    return;
}
void opr_pushv()
{
    *(++stack_top)=gc_alloc(vm_vec);
    return;
}
void opr_pushh()
{
    *(++stack_top)=gc_alloc(vm_hash);
    return;
}
void opr_pushf()
{
    *(++stack_top)=gc_alloc(vm_func);
    if(!local_scope.empty())
        (*stack_top)->ptr.func->set_scope(local_scope.front());
    (*stack_top)->ptr.func->entry=exec_code[pc].num;
    return;
}
void opr_vapp()
{
    nas_val* tmp=*stack_top--;
    (*stack_top)->ptr.vec->add_elem(tmp);
    return;
}
void opr_happ()
{
    nas_val* tmp=*stack_top--;
    (*stack_top)->ptr.hash->add_elem(rt_str_table[exec_code[pc].num],tmp);
    return;
}
void opr_para()
{
    (*stack_top)->ptr.func->para.push_back(exec_code[pc].num);
    return;
}
void opr_dynpara()
{
    (*stack_top)->ptr.func->dynpara=exec_code[pc].num;
    return;
}
void opr_loadg()
{
    global_scope->ptr.cls->add_value(exec_code[pc].num,*stack_top--);
    return;
}
void opr_loadl()
{
    local_scope.front()->ptr.cls->add_value(exec_code[pc].num,*stack_top--);
    return;
}
void opr_callg()
{
    *(++stack_top)=global_scope->ptr.cls->get_val(exec_code[pc].num);
    return;
}
void opr_calll()
{
    *(++stack_top)=local_scope.front()->ptr.cls->get_val(exec_code[pc].num);
    return;
}
void opr_callv()
{
    nas_val* tmp=*stack_top--;
    nas_val* res=nullptr;
    if((*stack_top)->type!=vm_vec && (*stack_top)->type!=vm_hash && (*stack_top)->type!=vm_str)
    {
        runtime_error("callv","must call a vector,string or hash");
        return;
    }
    if((*stack_top)->type==vm_vec || (*stack_top)->type==vm_str)
    {
        if(tmp->type!=vm_num)
        {
            runtime_error("callv[vec/str]","index must be a number");
            return;
        }
        if((*stack_top)->type==vm_vec)
            res=(*stack_top)->ptr.vec->get_val(tmp->ptr.num);
        else
        {
            int index=tmp->ptr.num;
            int len=(*stack_top)->ptr.str->length();
            if(index<0 || index>=len)
            {
                runtime_error("callv[str]","index out of range");
                return;
            }
            res=gc_alloc(vm_str);
            *res->ptr.str=(*(*stack_top)->ptr.str)[index];
            *stack_top=res;
            return;
        }
    }
    else
    {
        if(tmp->type!=vm_str)
        {
            runtime_error("callv[hash]","key must be a string");
            return;
        }
        res=(*stack_top)->ptr.hash->get_val(*tmp->ptr.str);
    }
    if(!res)
    {
        main_loop_continue_mark=false;
        return;
    }
    *stack_top=res;
    return;
}
void opr_callh()
{
    if((*stack_top)->type!=vm_hash)
    {
        runtime_error("callh","must call a hash");
        return;
    }
    nas_val* tmp=(*stack_top)->ptr.hash->get_val(rt_str_table[exec_code[pc].num]);
    if(!tmp)
    {
        main_loop_continue_mark=false;
        return;
    }
    *stack_top=tmp;
    return;
}
void opr_callf()
{
    nas_val* vec=*stack_top;
    nas_val* func=*(stack_top-1);
    // reserve arg_vec and func on the stack to avoid being collected
    if(func->type!=vm_func)
    {
        runtime_error("callf","must call a function");
        return;
    }
    if(func->ptr.func->is_builtin)
    {
        nas_val* ret=builtin_func[func->ptr.func->entry].func_ptr(vec);
        main_loop_continue_mark=(ret!=0);
        *(--stack_top)=ret;
        return;
    }
    return_address.push(pc);
    pc=func->ptr.func->entry-1;

    local_scope.push_front(gc_alloc(vm_scop));
    local_scope.front()->ptr.cls->set_closure(func->ptr.func->scope->ptr.cls);

    // load parameter unfinished
    std::vector<nas_val*>& paras=vec->ptr.vec->elems;
    std::vector<int>& para_index=func->ptr.func->para;
    int dynpara=func->ptr.func->dynpara;

    int arg_size=paras.size();
    int argindex_size=para_index.size();
    if(arg_size<argindex_size)
    {
        runtime_error("callf","lack argument(s)");
        return;
    }
    for(int i=0;i<argindex_size;++i)
        local_scope.front()->ptr.cls->add_value(para_index[i],paras[i]);
    if(dynpara>=0)
    {
        nas_val* dyn=gc_alloc(vm_vec);
        for(int i=argindex_size;i<arg_size;++i)
            dyn->ptr.vec->add_elem(paras[i]);
        local_scope.front()->ptr.cls->add_value(dynpara,dyn);
    }
    return;
}
void opr_mcallg()
{
    mem_stack.push(global_scope->ptr.cls->get_mem(exec_code[pc].num));
    return;
}
void opr_mcalll()
{
    mem_stack.push(local_scope.front()->ptr.cls->get_mem(exec_code[pc].num));
    return;
}
void opr_mcallv()
{
    nas_val*  tmp=*stack_top--;
    nas_val** mem=mem_stack.top();
    nas_val** res=nullptr;
    if((*mem)->type!=vm_vec && (*mem)->type!=vm_hash)
    {
        runtime_error("mcallv","must call a vector or hash");
        return;
    }
    if((*mem)->type==vm_vec)
    {
        if(tmp->type!=vm_num)
        {
            runtime_error("mcallv[vec]","index must be a number");
            return;
        }
        res=(*mem)->ptr.vec->get_mem(tmp->ptr.num);
    }
    else
    {
        if(tmp->type!=vm_str)
        {
            runtime_error("mcallv[hash]","key must be a string");
            return;
        }
        res=(*mem)->ptr.hash->get_mem(*tmp->ptr.str);
    }
    if(!res)
    {
        main_loop_continue_mark=false;
        return;
    }
    mem_stack.top()=res;
    return;
}
void opr_mcallh()
{
    nas_val** mem=mem_stack.top();
    if((*mem)->type!=vm_hash)
    {
        runtime_error("mcallh","must call a hash");
        return;
    }
    nas_val** res=(*mem)->ptr.hash->get_mem(rt_str_table[exec_code[pc].num]);
    if(!res)
    {
        main_loop_continue_mark=false;
        return;
    }
    mem_stack.top()=res;
    return;
}
void opr_neg()
{
    nas_val* tmp=*stack_top;
    double num=0;
    if(tmp->type==vm_num)
        num=-tmp->ptr.num;
    *stack_top=gc_alloc(vm_num);
    (*stack_top)->ptr.num=num;
    return;
}
void opr_not()
{
    nas_val* tmp=*stack_top;
    double num=0;
    if(tmp->type==vm_num)
        num=!tmp->ptr.num;
    else if(tmp->type==vm_str)
        num=!tmp->ptr.str->length();
    *stack_top=gc_alloc(vm_num);
    (*stack_top)->ptr.num=num;
    return;
}
void opr_plus()
{
    nas_val* res=gc_alloc(vm_num);
    nas_val* num2=*stack_top--;
    nas_val* num1=*stack_top;
    res->ptr.num=num1->ptr.num+num2->ptr.num;
    *stack_top=res;
    return;
}
void opr_minus()
{
    nas_val* res=gc_alloc(vm_num);
    nas_val* num2=*stack_top--;
    nas_val* num1=*stack_top;
    res->ptr.num=num1->ptr.num-num2->ptr.num;
    *stack_top=res;
    return;
}
void opr_mult()
{
    nas_val* res=gc_alloc(vm_num);
    nas_val* num2=*stack_top--;
    nas_val* num1=*stack_top;
    res->ptr.num=num1->ptr.num*num2->ptr.num;
    *stack_top=res;
    return;
}
void opr_div()
{
    nas_val* res=gc_alloc(vm_num);
    nas_val* num2=*stack_top--;
    nas_val* num1=*stack_top;
    res->ptr.num=num1->ptr.num/num2->ptr.num;
    *stack_top=res;
    return;
}
void opr_lnk()
{
    nas_val* res=gc_alloc(vm_str);
    nas_val* num2=*stack_top--;
    nas_val* num1=*stack_top;
    if(num1->type!=vm_str || num2->type!=vm_str)
    {
        runtime_error("lnk","only strinsg can take part in this calculation");
        return;
    }
    *res->ptr.str=(*num1->ptr.str)+(*num2->ptr.str);
    *stack_top=res;
    return;
}
void opr_meq()
{
    *mem_stack.top()=*stack_top;
    mem_stack.pop();
    return;
}
void opr_pluseq()
{
    nas_val** mem=mem_stack.top();
    mem_stack.pop();
    nas_val* res=gc_alloc(vm_num);
    nas_val* num2=*stack_top;
    nas_val* num1=*mem;
    res->ptr.num=num1->ptr.num+num2->ptr.num;
    *mem=res;
    return;
}
void opr_minuseq()
{
    nas_val** mem=mem_stack.top();
    mem_stack.pop();
    nas_val* res=gc_alloc(vm_num);
    nas_val* num2=*stack_top;
    nas_val* num1=*mem;
    res->ptr.num=num1->ptr.num-num2->ptr.num;
    *mem=res;
    return;
}
void opr_lnkeq()
{
    nas_val** mem=mem_stack.top();
    mem_stack.pop();
    nas_val* res=gc_alloc(vm_str);
    nas_val* num2=*stack_top;
    nas_val* num1=*mem;
    if(num1->type!=vm_str || num2->type!=vm_str)
    {
        runtime_error("lnkeq","only strinsg can take part in this calculation");
        return;
    }
    *res->ptr.str=(*num1->ptr.str)+(*num2->ptr.str);
    *mem=res;
    return;
}
void opr_multeq()
{
    nas_val** mem=mem_stack.top();
    mem_stack.pop();
    nas_val* res=gc_alloc(vm_num);
    nas_val* num2=*stack_top;
    nas_val* num1=*mem;
    res->ptr.num=num1->ptr.num*num2->ptr.num;
    *mem=res;
    return;
}
void opr_diveq()
{
    nas_val** mem=mem_stack.top();
    mem_stack.pop();
    nas_val* res=gc_alloc(vm_num);
    nas_val* num2=*stack_top;
    nas_val* num1=*mem;
    res->ptr.num=num1->ptr.num/num2->ptr.num;
    *mem=res;
    return;
}
void opr_eq()
{
    nas_val* res=gc_alloc(vm_num);
    nas_val* num2=*stack_top--;
    nas_val* num1=*stack_top;
    if(num1->type!=num2->type)
        res->ptr.num=(0);
    else if(num1->type==vm_num && num2->type==vm_num)
        res->ptr.num=(num1->ptr.num==num2->ptr.num);
    else if(num1->type==vm_str && num2->type==vm_str)
        res->ptr.num=(num1->ptr.str==num2->ptr.str);
    else
        res->ptr.num=(num1==num2);
    *stack_top=res;
    return;
}
void opr_neq()
{
    nas_val* res=gc_alloc(vm_num);
    nas_val* num2=*stack_top--;
    nas_val* num1=*stack_top;
    if(num1->type!=num2->type)
        res->ptr.num=1;
    else if(num1->type==vm_num && num2->type==vm_num)
        res->ptr.num=(num1->ptr.num!=num2->ptr.num);
    else if(num1->type==vm_str && num2->type==vm_str)
        res->ptr.num=(num1->ptr.str!=num2->ptr.str);
    else
        res->ptr.num=(num1!=num2);
    *stack_top=res;
    return;
}
void opr_less()
{
    nas_val* res=gc_alloc(vm_num);
    nas_val* num2=*stack_top--;
    nas_val* num1=*stack_top;
    if(num1->type==vm_num && num2->type==vm_num)
        res->ptr.num=(num1->ptr.num<num2->ptr.num);
    else if(num1->type==vm_str && num2->type==vm_str)
        res->ptr.num=(num1->ptr.str<num2->ptr.str);
    else
        runtime_error("less","only number and string can take part in this calculation");
    *stack_top=res;
    return;
}
void opr_leq()
{
    nas_val* res=gc_alloc(vm_num);
    nas_val* num2=*stack_top--;
    nas_val* num1=*stack_top;
    if(num1->type==vm_num && num2->type==vm_num)
        res->ptr.num=(num1->ptr.num<=num2->ptr.num);
    else if(num1->type==vm_str && num2->type==vm_str)
        res->ptr.num=(num1->ptr.str<=num2->ptr.str);
    else
        runtime_error("leq","only number and string can take part in this calculation");
    *stack_top=res;
    return;
}
void opr_grt()
{
    nas_val* res=gc_alloc(vm_num);
    nas_val* num2=*stack_top--;
    nas_val* num1=*stack_top;
    if(num1->type==vm_num && num2->type==vm_num)
        res->ptr.num=(num1->ptr.num>num2->ptr.num);
    else if(num1->type==vm_str && num2->type==vm_str)
        res->ptr.num=(num1->ptr.str>num2->ptr.str);
    else
        runtime_error("grt","only number and string can take part in this calculation");
    *stack_top=res;
    return;
}
void opr_geq()
{
    nas_val* res=gc_alloc(vm_num);
    nas_val* num2=*stack_top--;
    nas_val* num1=*stack_top;
    if(num1->type==vm_num && num2->type==vm_num)
        res->ptr.num=(num1->ptr.num>=num2->ptr.num);
    else if(num1->type==vm_str && num2->type==vm_str)
        res->ptr.num=(num1->ptr.str>=num2->ptr.str);
    else
        runtime_error("geq","only number and string can take part in this calculation");
    *stack_top=res;
    return;
}
void opr_pop()
{
    --stack_top;
    return;
}
void opr_jmp()
{
    pc=exec_code[pc].num-1;
    return;
}
void opr_jt()
{
    nas_val* tmp=*stack_top--;
    if(tmp->type==vm_num && tmp->ptr.num)
        pc=exec_code[pc].num-1;
    return;
}
void opr_jf()
{
    nas_val* tmp=*stack_top--;
    if(tmp->type==vm_num && !tmp->ptr.num)
        pc=exec_code[pc].num-1;
    else if(tmp->type==vm_nil)
        pc=exec_code[pc].num-1;
    return;
}
void opr_ret()
{
    pc=return_address.top();
    return_address.pop();
    local_scope.pop_front();
    nas_val* tmp=*stack_top--;
    *(--stack_top)=tmp;
    return;
}

void init_vm()
{
    gc_init();
    main_loop_continue_mark=true;

    val_stack=new nas_val*[16384];
    stack_top=val_stack;
    *stack_top=gc_alloc(vm_nil); // set nil in the beginning space to avoid errors

    global_scope=gc_alloc(vm_scop);

    for(int i=0;builtin_func[i].func_name;++i)
    {
        nas_val* new_builtin=gc_alloc(vm_func);
        new_builtin->ptr.func->is_builtin=true;
        new_builtin->ptr.func->entry=i;
        global_scope->ptr.cls->add_value(string_table[builtin_func[i].func_name],new_builtin);
    }

    rt_num_table.resize(number_table.size());
    for(auto iter=number_table.begin();iter!=number_table.end();++iter)
    {
        nas_val* num=new nas_val(vm_num);
        num->ptr.num=iter->first;
        rt_num_table[iter->second]=num;
    }
    number_table.clear();
    rt_str_table.resize(string_table.size());
    for(auto iter=string_table.begin();iter!=string_table.end();++iter)
        rt_str_table[iter->second]=iter->first;
    string_table.clear();
    return;
}

void clear_vm()
{
    delete []val_stack;
    while(!mem_stack.empty())
        mem_stack.pop();
    local_scope.clear();
    while(!return_address.empty())
        return_address.pop();
    gc_clean();

    for(auto i=rt_num_table.begin();i!=rt_num_table.end();++i)
        delete *i;
    rt_num_table.clear();
    rt_str_table.clear();
    return;
}

void run_vm()
{
    static void (*func[])()=
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
        &opr_loadg,
        &opr_loadl,
        &opr_callg,
        &opr_calll,
        &opr_callv,
        &opr_callh,
        &opr_callf,
        &opr_mcallg,
        &opr_mcalll,
        &opr_mcallv,
        &opr_mcallh,
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

    init_vm();
    clock_t begin_time=clock();
    for(pc=0;main_loop_continue_mark;++pc)
        (*func[exec_code[pc].op])();
    float total_run_time=((double)(clock()-begin_time))/CLOCKS_PER_SEC;
    std::cout<<">> [vm] process exited after "<<total_run_time<<"s.\n";
    clear_vm();
    return;
}

#endif