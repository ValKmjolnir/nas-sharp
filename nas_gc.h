#ifndef __NAS_GC_H__
#define __NAS_GC_H__

#define MEMSIZE (65536)

enum vm_type
{
    vm_nil=0,
    vm_num,
    vm_str,
    vm_scop,
    vm_func,
    vm_vec,
    vm_hash,
};

struct nas_gc;
struct nas_vec;
struct nas_hash;
struct nas_func;
struct nas_scop;
struct nas_val;

// runtime value stack
nas_val** val_stack;
// stack top pointer
nas_val** stack_top;
// global
nas_val* global_scope;
// local
std::list<nas_val*> local_scope;
std::queue<nas_val*> free_space;
std::vector<nas_val*> memory;

void collect_vec(nas_val*);
void collect_hash(nas_val*);
void collect_scop(nas_val*);
void sweep();
void mark();
nas_val* gc_alloc(int);
void gc_init();
void gc_clean();

struct nas_vec
{
    std::vector<nas_val*> elems;

    void      add_elem(nas_val*);
    void      del_elem();
    nas_val*  get_val(int);
    nas_val** get_mem(int);
    void      print();
};

struct nas_hash
{
    std::unordered_map<std::string,nas_val*> elems;

    void      add_elem(std::string,nas_val*);
    void      del_elem(std::string);
    nas_val*  get_val(std::string);
    nas_val** get_mem(std::string);
    nas_val*  get_keys();
    void      print();
};

struct nas_func
{
    nas_val* scope;
    std::vector<int> para;
    int entry;
    int dynpara;
    bool is_builtin;

    nas_func();
    void set_scope(nas_val*);
};

struct nas_scop
{
    std::unordered_map<int,nas_val*> elems;

    void add_value(int,nas_val*);
    nas_val*  get_val(int);
    nas_val** get_mem(int);
    void set_closure(nas_scop*);
};

struct nas_val
{
    int type;
    union 
    {
        double       num;
        std::string* str;
        nas_vec*     vec;
        nas_hash*    hash;
        nas_func*    func;
        nas_scop*    cls;
    }ptr;
    bool mark;

    nas_val();
    nas_val(int);
    ~nas_val();
    void clear();
};

/*functions of nas_vec*/
void nas_vec::add_elem(nas_val* value_address)
{
    elems.push_back(value_address);
    return;
}
void nas_vec::del_elem()
{
    if(!elems.size())
        return;
    elems.pop_back();
    return;
}
nas_val* nas_vec::get_val(int index)
{
    int vec_size=elems.size();
    if(index<-vec_size || index>=vec_size)
    {
        std::cout<<">> [vm] nas_vec::get_val: index out of range: "<<index<<".\n";
        return nullptr;
    }
    int idx[2]={index+vec_size,index};
    return elems[idx[index>=0]];
}
nas_val** nas_vec::get_mem(int index)
{
    int vec_size=elems.size();
    if(index<-vec_size || index>=vec_size)
    {
        std::cout<<">> [vm] nas_vec::get_mem: index out of range: "<<index<<".\n";
        return nullptr;
    }
    int idx[2]={index+vec_size,index};
    return &elems[idx[index>=0]];
}
void nas_vec::print()
{
    int size=elems.size();
    std::cout<<"[";
    if(!size)
        std::cout<<"]";
    for(int i=0;i<size;++i)
    {
        nas_val* tmp=elems[i];
        switch(tmp->type)
        {
            case vm_nil:  std::cout<<"nil";            break;
            case vm_num:  std::cout<<tmp->ptr.num;     break;
            case vm_str:  std::cout<<*tmp->ptr.str;    break;
            case vm_vec:  tmp->ptr.vec->print();       break;
            case vm_hash: tmp->ptr.hash->print();      break;
            case vm_func: std::cout<<"func(...){...}"; break;
        }
        std::cout<<",]"[i==size-1];
    }
    return;
}

/*functions of nas_hash*/
void nas_hash::add_elem(std::string key,nas_val* value_address)
{
    elems[key]=value_address;
    return;
}
void nas_hash::del_elem(std::string key)
{
    if(elems.count(key))
        elems.erase(key);
    return;
}
nas_val* nas_hash::get_val(std::string key)
{
    if(elems.count(key))
        return elems[key];
    std::cout<<">> [vm] nas_hash::get_val: cannot find member named \""<<key<<"\".\n";
    return nullptr;
}
nas_val** nas_hash::get_mem(std::string key)
{
    if(elems.count(key))
        return &elems[key];
    std::cout<<">> [vm] nas_hash::get_mem: cannot find member named \""<<key<<"\".\n";
    return nullptr;
}
nas_val* nas_hash::get_keys()
{
    nas_val* ret_addr=gc_alloc(vm_vec);
    nas_vec& ref_vec=*ret_addr->ptr.vec;
    for(auto iter=elems.begin();iter!=elems.end();++iter)
    {
        nas_val* str_addr=gc_alloc(vm_str);
        *str_addr->ptr.str=(iter->first);
        ref_vec.add_elem(str_addr);
    }
    return ret_addr;
}
void nas_hash::print()
{
    std::cout<<'{';
    if(!elems.size())
        std::cout<<'}';
    for(auto i=elems.begin();i!=elems.end();++i)
    {
        std::cout<<i->first<<":";
        nas_val* tmp=i->second;
        switch(tmp->type)
        {
            case vm_nil:  std::cout<<"nil";            break;
            case vm_num:  std::cout<<tmp->ptr.num;     break;
            case vm_str:  std::cout<<*tmp->ptr.str;    break;
            case vm_vec:  tmp->ptr.vec->print();      break;
            case vm_hash: tmp->ptr.hash->print();     break;
            case vm_func: std::cout<<"func(...){...}"; break;
        }
        std::cout<<',';
    }
    std::cout<<'}';
    return;
}

/*functions of nas_func*/
nas_func::nas_func()
{
    is_builtin=false;
    scope=nullptr;
    scope=gc_alloc(vm_scop);
    dynpara=-1;
    return;
}
void nas_func::set_scope(nas_val* value_address)
{
    nas_val* new_scop=gc_alloc(vm_scop);
    std::unordered_map<int,nas_val*>& this_scop=new_scop->ptr.cls->elems;
    std::unordered_map<int,nas_val*>& src_scop=value_address->ptr.cls->elems;
    for(auto i=src_scop.begin();i!=src_scop.end();++i)
        this_scop[i->first]=i->second;
    scope=new_scop;
    return;
}

/*functions of nas_scop*/
void nas_scop::add_value(int key,nas_val* value_address)
{
    elems[key]=value_address;
    return;
}
nas_val* nas_scop::get_val(int key)
{
    if(elems.count(key))
        return (elems)[key];
    return nullptr;
}
nas_val** nas_scop::get_mem(int key)
{
    if(elems.count(key))
        return &(elems[key]);
    return nullptr;
}
void nas_scop::set_closure(nas_scop* tmp)
{
    for(auto i=tmp->elems.begin();i!=tmp->elems.end();++i)
        elems[i->first]=i->second;
    return;
}

/*functions of nas_val*/
nas_val::nas_val()
{
    mark=false;
    type=vm_nil;
    return;
}
nas_val::nas_val(int val_type)
{
    mark=false;
    type=val_type;
    switch(val_type)
    {
        case vm_nil:  break;
        case vm_num:  ptr.num=0;               break;
        case vm_str:  ptr.str=new std::string; break;
        case vm_vec:  ptr.vec=new nas_vec;     break;
        case vm_hash: ptr.hash=new nas_hash;   break;
        case vm_func: ptr.func=new nas_func;   break;
        case vm_scop: ptr.cls=new nas_scop;    break;
    }
    return;
}
nas_val::~nas_val()
{
    switch(type)
    {
        case vm_nil:  break;
        case vm_num:  break;
        case vm_str:  delete ptr.str;  break;
        case vm_vec:  delete ptr.vec;  break;
        case vm_hash: delete ptr.hash; break;
        case vm_func: delete ptr.func; break;
        case vm_scop: delete ptr.cls;  break;
    }
    return;
}
void nas_val::clear()
{
    switch(type)
    {
        case vm_nil:  break;
        case vm_num:  break;
        case vm_str:  delete ptr.str;  break;
        case vm_vec:  delete ptr.vec;  break;
        case vm_hash: delete ptr.hash; break;
        case vm_func: delete ptr.func; break;
        case vm_scop: delete ptr.cls;  break;
    }
    type=vm_nil;
    return;
}

void sweep()
{
    int size=memory.size();
    int begin=rand()%size;
    int end=(begin+size)>>1;
    for(int i=begin;i<end;++i)
    {
        if(!memory[i]->mark)
        {
            memory[i]->clear();
            free_space.push(memory[i]);
        }
        memory[i]->mark=false;
    }
    return;
}

void collect_vec(nas_val* addr)
{
    std::vector<nas_val*>& ref=addr->ptr.vec->elems;
    int size=ref.size();
    for(int i=0;i<size;++i)
        if(!ref[i]->mark)
        {
            ref[i]->mark=true;
            switch(ref[i]->type)
            {
                case vm_vec: collect_vec(ref[i]); break;
                case vm_hash:collect_hash(ref[i]);break;
                case vm_func:collect_scop(ref[i]->ptr.func->scope);break;
                case vm_scop:collect_scop(ref[i]);break;
            }
        }
    return;
}

void collect_hash(nas_val* addr)
{
    std::unordered_map<std::string,nas_val*>& ref=addr->ptr.hash->elems;
    for(auto i=ref.begin();i!=ref.end();++i)
        if(!i->second->mark)
        {
            i->second->mark=true;
            switch(i->second->type)
            {
                case vm_vec: collect_vec(i->second); break;
                case vm_hash:collect_hash(i->second);break;
                case vm_func:collect_scop(i->second->ptr.func->scope);break;
                case vm_scop:collect_scop(i->second);break;
            }
        }
    return;
}

void collect_scop(nas_val* addr)
{
    addr->mark=true;
    std::unordered_map<int,nas_val*>& ref=addr->ptr.cls->elems;
    for(auto i=ref.begin();i!=ref.end();++i)
        if(!i->second->mark)
        {
            i->second->mark=true;
            switch(i->second->type)
            {
                case vm_vec: collect_vec(i->second); break;
                case vm_hash:collect_hash(i->second);break;
                case vm_func:collect_scop(i->second->ptr.func->scope);break;
                case vm_scop:collect_scop(i->second);break;
            }
        }
    return;
}

void mark()
{
    for(nas_val** i=val_stack;i<=stack_top;++i)
        if(!(*i)->mark)
        {
            (*i)->mark=true;
            switch((*i)->type)
            {
                case vm_vec: collect_vec(*i); break;
                case vm_hash:collect_hash(*i);break;
                case vm_func:collect_scop((*i)->ptr.func->scope);break;
                case vm_scop:collect_scop(*i);break;
            }
        }
    for(auto i=local_scope.begin();i!=local_scope.end();++i)
        if(!(*i)->mark)
            collect_scop(*i);
    collect_scop(global_scope);
    return;
}

nas_val* gc_alloc(int type)
{
    if(free_space.empty())
    {
        mark();
        sweep();
    }
    if(free_space.empty())
        for(int i=0;i<MEMSIZE;++i)
        {
            nas_val* tmp=new nas_val;
            memory.push_back(tmp);
            free_space.push(tmp);
        }
    nas_val* new_addr=free_space.front();
    free_space.pop();
    new_addr->type=type;
    switch(type)
    {
        case vm_nil:  break;
        case vm_num:  new_addr->ptr.num=0;               break;
        case vm_str:  new_addr->ptr.str=new std::string; break;
        case vm_vec:  new_addr->ptr.vec=new nas_vec;     break;
        case vm_hash: new_addr->ptr.hash=new nas_hash;   break;
        case vm_func: new_addr->ptr.func=new nas_func;   break;
        case vm_scop: new_addr->ptr.cls=new nas_scop;    break;
    }
    return new_addr;
}

void gc_init()
{
    memory.resize(MEMSIZE);
    for(int i=0;i<MEMSIZE;++i)
    {
        memory[i]=new nas_val;
        free_space.push(memory[i]);
    }
    return;
}

void gc_clean()
{
    int size=memory.size();
    for(int i=0;i<size;++i)
        delete memory[i];
    memory.clear();
    while(!free_space.empty())
        free_space.pop();
    return;
}
#endif