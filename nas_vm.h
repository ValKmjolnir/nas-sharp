#ifndef __NAS_VM_H__
#define __NAS_VM_H__

bool main_loop_continue_mark;
nas_val* global_scope;
std::stack<nas_val**> mem_stack;
std::stack<nas_val*> local_scope;
std::stack<unsigned int> return_address;
nas_vm vm;

std::vector<double> runtime_number_table;
std::vector<std::string> runtime_string_table;
nas_val** val_stack;
nas_val** stack_top;
int pc;

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
    *(++stack_top)=vm.gc_alloc(vm_nil);
    return;
}
void opr_pushn()
{
    *(++stack_top)=vm.gc_alloc(vm_num);
    (*stack_top)->set_num(runtime_number_table[exec_code[pc].num]);
    return;
}
void opr_pushs()
{
    *(++stack_top)=vm.gc_alloc(vm_str);
    (*stack_top)->set_str(runtime_string_table[exec_code[pc].num]);
    return;
}
void opr_pushv()
{
    *(++stack_top)=vm.gc_alloc(vm_vec);
    return;
}
void opr_pushh()
{
    *(++stack_top)=vm.gc_alloc(vm_hash);
    return;
}
void opr_pushf()
{
    *(++stack_top)=vm.gc_alloc(vm_func);
    if(local_scope.top()) (*stack_top)->get_func().set_scope(local_scope.top());
    else (*stack_top)->get_func().set_new_closure();
    return;
}
void opr_vapp()
{
    nas_val* tmp=*stack_top--;
    (*stack_top)->get_vec().add_elem(tmp);
    return;
}
void opr_happ()
{
    nas_val* tmp=*stack_top--;
    (*stack_top)->get_hash().add_elem(runtime_string_table[exec_code[pc].num],tmp);
    return;
}
void opr_para()
{
    (*stack_top)->get_func().add_para(exec_code[pc].num);
    return;
}
void opr_dynpara()
{
    (*stack_top)->get_func().add_para(exec_code[pc].num,true);
    return;
}
void opr_entry()
{
    (*stack_top)->get_func().set_entry(exec_code[pc].num);
    return;
}
void opr_load()
{
    if(local_scope.top())
        local_scope.top()->get_scop().add_value(exec_code[pc].num,*stack_top--);
    else
        global_scope->get_scop().add_value(exec_code[pc].num,*stack_top--);
    return;
}
void opr_call()
{
    nas_val* tmp=nullptr;
    if(local_scope.top()) tmp=local_scope.top()->get_scop().get_val(exec_code[pc].num);
    if(!tmp) tmp=global_scope->get_scop().get_val(exec_code[pc].num);
    if(!tmp) runtime_error("call","cannot find value named \""+runtime_string_table[exec_code[pc].num]+"\"");
    *(++stack_top)=tmp;
    vm.add_ref(tmp);
    return;
}
void opr_callv()
{
    nas_val* tmp=*stack_top--;
    nas_val* res=nullptr;
    if((*stack_top)->get_type()!=vm_vec && (*stack_top)->get_type()!=vm_hash)
    {
        runtime_error("callv","must call a vector or hash");
        return;
    }
    if((*stack_top)->get_type()==vm_vec)
    {
        if(tmp->get_type()!=vm_num)
        {
            runtime_error("callv[vec]","index must be a number");
            return;
        }
        res=(*stack_top)->get_vec().get_val(tmp->get_num());
    }
    else
    {
        if(tmp->get_type()!=vm_str)
        {
            runtime_error("callv[hash]","key must be a string");
            return;
        }
        res=(*stack_top)->get_hash().get_val(tmp->get_str());
    }
    if(!res)
    {
        main_loop_continue_mark=false;
        return;
    }
    vm.add_ref(res);
    vm.del_ref(tmp);
    vm.del_ref(*stack_top);
    *stack_top=res;
    return;
}
void opr_callh()
{
    if((*stack_top)->get_type()!=vm_hash)
    {
        runtime_error("callh","must call a hash");
        return;
    }
    nas_val* tmp=(*stack_top)->get_hash().get_val(runtime_string_table[exec_code[pc].num]);
    if(!tmp)
    {
        main_loop_continue_mark=false;
        return;
    }
    vm.add_ref(tmp);
    vm.del_ref(*stack_top);
    *stack_top=tmp;
    return;
}
void opr_callf()
{
    nas_val* vec=*stack_top--;
    nas_val* func=*stack_top;
    if(func->get_type()!=vm_func)
    {
        runtime_error("callf","must call a function");
        return;
    }
    return_address.push(pc);
    pc=func->get_func().get_entry()-1;
    local_scope.push(func->get_func().get_scope());
    local_scope.top()->get_scop().add_scope();

    // load parameter unfinished
    vm.del_ref(vec);
    return;
}
void opr_mcall()
{
    nas_val** tmp=nullptr;
    if(local_scope.top()) tmp=local_scope.top()->get_scop().get_mem(exec_code[pc].num);
    if(!tmp) tmp=global_scope->get_scop().get_mem(exec_code[pc].num);
    if(!tmp) runtime_error("mcall","cannot find value named \""+runtime_string_table[exec_code[pc].num]+"\"");
    mem_stack.push(tmp);
    return;
}
void opr_mcallv()
{
    nas_val* tmp=*stack_top--;
    nas_val** mem=mem_stack.top();
    nas_val** res=nullptr;
    if((*mem)->get_type()!=vm_vec && (*mem)->get_type()!=vm_hash)
    {
        runtime_error("mcallv","must call a vector or hash");
        return;
    }
    if((*mem)->get_type()==vm_vec)
    {
        if(tmp->get_type()!=vm_num)
        {
            runtime_error("mcallv[vec]","index must be a number");
            return;
        }
        res=(*mem)->get_vec().get_mem(tmp->get_num());
    }
    else
    {
        if(tmp->get_type()!=vm_str)
        {
            runtime_error("mcallv[hash]","key must be a string");
            return;
        }
        res=(*mem)->get_hash().get_mem(tmp->get_str());
    }
    if(!res)
    {
        main_loop_continue_mark=false;
        return;
    }
    vm.del_ref(tmp);
    mem_stack.top()=res;
    return;
}
void opr_mcallh()
{
    nas_val** mem=mem_stack.top();
    if((*mem)->get_type()!=vm_hash)
    {
        runtime_error("mcallh","must call a hash");
        return;
    }
    nas_val** res=(*stack_top)->get_hash().get_mem(runtime_string_table[exec_code[pc].num]);
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
    double num=std::nan("");
    if(tmp->get_type()==vm_num)
        num=-tmp->get_num();
    vm.del_ref(tmp);
    *stack_top=vm.gc_alloc(vm_num);
    (*stack_top)->set_num(num);
    return;
}
void opr_not()
{
    nas_val* tmp=*stack_top;
    double num=0;
    switch(tmp->get_type())
    {
        case vm_nil:num=0;break;
        case vm_num:num=(!tmp->get_num());break;
        case vm_str:num=(!tmp->get_str().length());break;
        default:    num=0;break;
    }
    vm.del_ref(tmp);
    *stack_top=vm.gc_alloc(vm_num);
    (*stack_top)->set_num(num);
    return;
}
void opr_plus()
{
    return;
}
void opr_minus()
{
    return;
}
void opr_mult()
{
    return;
}
void opr_div()
{
    return;
}
void opr_lnk()
{
    return;
}
void opr_meq()
{
    return;
}
void opr_pluseq()
{
    return;
}
void opr_minuseq()
{
    return;
}
void opr_lnkeq()
{
    return;
}
void opr_multeq()
{
    return;
}
void opr_diveq()
{
    return;
}
void opr_eq()
{
    return;
}
void opr_neq()
{
    return;
}
void opr_less()
{
    return;
}
void opr_leq()
{
    return;
}
void opr_grt()
{
    return;
}
void opr_geq()
{
    return;
}
void opr_pop()
{
    vm.del_ref(*stack_top--);
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
    if(tmp->get_type()==vm_num && tmp->get_num()) pc=exec_code[pc].num-1;
    else if(tmp->get_type()==vm_str && tmp->get_str().length()) pc=exec_code[pc].num-1;
    vm.del_ref(tmp);
    return;
}
void opr_jf()
{
    nas_val* tmp=*stack_top--;
    if(tmp->get_type()==vm_str && !tmp->get_str().length()) pc=exec_code[pc].num-1;
    else if(tmp->get_type()!=vm_num) pc=exec_code[pc].num-1;
    vm.del_ref(tmp);
    return;
}
void opr_ret()
{
    pc=return_address.top();
    return_address.pop();
    local_scope.top()->get_scop().del_scope();
    local_scope.pop();
    nas_val* tmp=*stack_top--;
    vm.del_ref(*stack_top);
    *stack_top=tmp;
    return;
}

void init_vm()
{
    main_loop_continue_mark=true;
    global_scope=vm.gc_alloc(vm_scop);
    local_scope.push(nullptr);

    for(int i=0;builtin_func[i].func_name;++i)
    {
        nas_val* new_builtin=vm.gc_alloc(vm_func);
        new_builtin->get_func().is_builtin=true;
        global_scope->get_scop().add_value(string_table[builtin_func[i].func_name],new_builtin);
    }

    val_stack=new nas_val*[16384];
    stack_top=val_stack;
    *stack_top=vm.gc_alloc(vm_nil); // set nil in the beginning space to avoid errors

    runtime_number_table.resize(number_table.size());
    for(auto iter=number_table.begin();iter!=number_table.end();++iter)
        runtime_number_table[iter->second]=iter->first;
    runtime_string_table.resize(string_table.size());
    for(auto iter=string_table.begin();iter!=string_table.end();++iter)
        runtime_string_table[iter->second]=iter->first;
    return;
}
void clear_vm()
{
    delete []val_stack;
    while(!mem_stack.empty()) mem_stack.pop();
    while(!local_scope.empty()) local_scope.pop();
    while(!return_address.empty()) return_address.pop();
    vm.clear();
    runtime_number_table.clear();
    runtime_string_table.clear();
    return;
}
void run_vm()
{
    init_vm();
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
        &opr_entry,
        &opr_load,
        &opr_call,
        &opr_callv,
        &opr_callh,
        &opr_callf,
        &opr_mcall,
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
    clock_t begin_time=clock();
    for(pc=0;main_loop_continue_mark;++pc)
        (*func[exec_code[pc].op])();
    float total_run_time=((double)(clock()-begin_time))/CLOCKS_PER_SEC;
    std::cout<<">> [vm] process exited after "<<total_run_time<<"s.\n";
    clear_vm();
    return;
}

#endif