#ifndef __NAS_BUILTIN_H__
#define __NAS_BUILTIN_H__

nas_val* nas_builtin_print(nas_val*);
nas_val* nas_builtin_type(nas_val*);
nas_val* nas_builtin_time(nas_val*);
nas_val* nas_builtin_rand(nas_val*);
nas_val* nas_builtin_append(nas_val*);
nas_val* nas_builtin_exp(nas_val*);

struct
{
    nas_val* (*func_ptr)(nas_val*);
    const char* func_name;
}builtin_func[]=
{
    {&nas_builtin_print,  "print" },
    {&nas_builtin_type,   "type"  },
    {&nas_builtin_time,   "time"  },
    {&nas_builtin_rand,   "rand"  },
    {&nas_builtin_append, "append"},
    {&nas_builtin_exp,    "exp"   },
    {NULL,                NULL    }
};

nas_val* nas_builtin_print(nas_val* elem)
{
    std::vector<nas_val*>& vec=elem->ptr.vec->elems;
    int size=vec.size();
    for(int i=0;i<size;++i)
    {
        nas_val* tmp=vec[i];
        switch(tmp->type)
        {
            case vm_nil:  std::cout<<"nil";         break;
            case vm_num:  std::cout<<tmp->ptr.num;  break;
            case vm_str:  std::cout<<*tmp->ptr.str; break;
            case vm_vec:  tmp->ptr.vec->print();    break;
            case vm_hash: tmp->ptr.hash->print();   break;
            case vm_func: std::cout<<"func(...)";   break;
        }
    }
    return gc_alloc(vm_nil);
}

nas_val* nas_builtin_type(nas_val* elem)
{
    if(elem->ptr.vec->elems.size()!=1)
    {
        std::cout<<">> [vm] builtin_type: must use 1 argument.\n";
        return nullptr;
    }
    nas_val* s=gc_alloc(vm_str);
    switch(elem->ptr.vec->get_val(0)->type)
    {
        case vm_nil:  *s->ptr.str="nil";      break;
        case vm_num:  *s->ptr.str="number";   break;
        case vm_str:  *s->ptr.str="string";   break;
        case vm_vec:  *s->ptr.str="vector";   break;
        case vm_hash: *s->ptr.str="hash";     break;
        case vm_func: *s->ptr.str="function"; break;
        case vm_scop: *s->ptr.str="scope";    break;
    }
    return s;
}

nas_val* nas_builtin_time(nas_val* elem)
{
    if(elem->ptr.vec->elems.size()!=1)
    {
        std::cout<<">> [vm] builtin_time: must use 1 argument.\n";
        return nullptr;
    }
    time_t begin=(time_t)elem->ptr.vec->elems[0]->ptr.num;
    nas_val* ret=gc_alloc(vm_num);
    ret->ptr.num=(double)time(&begin);
    return ret;
}

nas_val* nas_builtin_rand(nas_val* elem)
{
    if(elem->ptr.vec->elems.size()>1)
    {
        std::cout<<">> [vm] builtin_rand: too much argument(s).\n";
        return nullptr;
    }
    if(elem->ptr.vec->elems.size()==1)
    {
        srand((unsigned int)elem->ptr.vec->elems[0]->ptr.num);
        return gc_alloc(vm_nil);
    }
    double num=0;
    for(int i=0;i<5;++i)
        num=(num+rand())*(1.0/(RAND_MAX+1.0));
    nas_val* ret_addr=gc_alloc(vm_num);
    ret_addr->ptr.num=num;
    return ret_addr;
}

nas_val* nas_builtin_append(nas_val* elem)
{
    std::vector<nas_val*>& vec=elem->ptr.vec->elems;
    if(vec.size()<2)
    {
        std::cout<<">> [vm] builtin_append: lack argument(s).\n";
        return nullptr;
    }
    std::vector<nas_val*>& ref=vec[0]->ptr.vec->elems;
    for(int i=1;i<vec.size();++i)
        ref.push_back(vec[i]);
    return gc_alloc(vm_nil);
}

nas_val* nas_builtin_exp(nas_val* elem)
{
    std::vector<nas_val*>& vec=elem->ptr.vec->elems;
    if(vec.size()!=1)
    {
        std::cout<<">> [vm] builtin_exp: must use one argument.\n";
        return nullptr;
    }
    nas_val* res=gc_alloc(vm_num);
    res->ptr.num=exp(vec[0]->ptr.num);
    return res;
}
#endif