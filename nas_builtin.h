#ifndef __NAS_BUILTIN_H__
#define __NAS_BUILTIN_H__

nas_val* nas_builtin_print(nas_val*,nas_vm&);
nas_val* nas_builtin_type(nas_val*,nas_vm&);

struct
{
    nas_val* (*func_ptr)(nas_val*,nas_vm&);
    const char* func_name;
}builtin_func[]=
{
    {&nas_builtin_print,"print"},
    {&nas_builtin_type,"type"},
    {NULL,NULL}
};

nas_val* nas_builtin_print(nas_val* elem,nas_vm& vm)
{
    nas_vec& vec=elem->get_vec();
    int size=vec.size();
    for(int i=0;i<size;++i)
    {
        nas_val* tmp=vec.get_val(i);
        switch(tmp->get_type())
        {
            case vm_nil:  std::cout<<"nil";          break;
            case vm_num:  std::cout<<tmp->get_num(); break;
            case vm_str:  std::cout<<tmp->get_str(); break;
            case vm_vec:  tmp->get_vec().print();    break;
            case vm_hash: tmp->get_hash().print();   break;
            case vm_func: std::cout<<"func(...)";    break;
        }
    }
    return vm.gc_alloc(vm_nil);
}

nas_val* nas_builtin_type(nas_val* elem,nas_vm& vm)
{
    nas_val* s=vm.gc_alloc(vm_str);
    switch(elem->get_vec().get_val(0)->get_type())
    {
        case vm_nil:  s->set_str("nil");      break;
        case vm_num:  s->set_str("number");   break;
        case vm_str:  s->set_str("string");   break;
        case vm_vec:  s->set_str("vector");   break;
        case vm_hash: s->set_str("hash");     break;
        case vm_func: s->set_str("function"); break;
        case vm_scop: s->set_str("scope");    break;
    }
    return s;
}
#endif