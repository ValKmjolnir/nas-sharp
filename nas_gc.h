#ifndef __NAS_GC_H__
#define __NAS_GC_H__

#define MEMSIZE (65536)

enum vm_type
{
    vm_nil=0,
    vm_num,
    vm_str,
    vm_func,
    vm_vec,
    vm_hash,
};

struct nas_gc;
struct nas_vec;
struct nas_hash;
struct nas_func;
struct nas_val;

struct nas_vec
{
    std::vector<nas_val*> elems;

    nas_val*  get_val(int);
    nas_val** get_mem(int);
    void      print();
};

struct nas_hash
{
    std::unordered_map<std::string,nas_val*> elems;

    nas_val*  get_val(std::string);
    nas_val** get_mem(std::string);
    void      print();
};

struct nas_func
{
    int entry;
    int dynpara;
    bool is_builtin;
    std::vector<int> para;
    std::vector<nas_val*> scope;

    nas_func();
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
    }ptr;
    bool mark;

    nas_val();
    nas_val(int);
    ~nas_val();
    void clear();
};

/*functions of nas_vec*/
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
    dynpara=-1;
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
        case vm_num:  ptr.num=0;               break;
        case vm_str:  ptr.str=new std::string; break;
        case vm_vec:  ptr.vec=new nas_vec;     break;
        case vm_hash: ptr.hash=new nas_hash;   break;
        case vm_func: ptr.func=new nas_func;   break;
    }
    return;
}
nas_val::~nas_val()
{
    switch(type)
    {
        case vm_str:  delete ptr.str;  break;
        case vm_vec:  delete ptr.vec;  break;
        case vm_hash: delete ptr.hash; break;
        case vm_func: delete ptr.func; break;
    }
    type=vm_nil;
    return;
}
void nas_val::clear()
{
    switch(type)
    {
        case vm_str:  delete ptr.str;  break;
        case vm_vec:  delete ptr.vec;  break;
        case vm_hash: delete ptr.hash; break;
        case vm_func: delete ptr.func; break;
    }
    type=vm_nil;
    return;
}

nas_val* val_stack[65536<<4];
nas_val** stack_top;
std::queue<nas_val*> free_space;
std::vector<nas_val*> memory;
std::vector<nas_val*> global_scope;
std::list<std::vector<nas_val*> > local_scope;

void sweep()
{
    int size=memory.size();
    for(int i=0;i<size;++i)
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

void mark()
{
    std::queue<nas_val*> bfs;
    for(auto i=global_scope.begin();i!=global_scope.end();++i)
        bfs.push(*i);
    for(auto i=local_scope.begin();i!=local_scope.end();++i)
        for(auto j=i->begin();j!=i->end();++j)
            bfs.push(*j);
    for(auto i=val_stack;i<=stack_top;++i)
        bfs.push(*i);
    while(!bfs.empty())
    {
        nas_val* t=bfs.front();
        bfs.pop();
        if(!t || t->mark)
            continue;
        t->mark=true;
        if(t->type==vm_vec)
            for(auto i=t->ptr.vec->elems.begin();i!=t->ptr.vec->elems.end();++i)
                bfs.push(*i);
        else if(t->type==vm_func)
            for(auto i=t->ptr.func->scope.begin();i!=t->ptr.func->scope.end();++i)
                bfs.push(*i);
        else if(t->type==vm_hash)
            for(auto i=t->ptr.hash->elems.begin();i!=t->ptr.hash->elems.end();++i)
                bfs.push(i->second);
    }
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
        case vm_num:  new_addr->ptr.num=0;               break;
        case vm_str:  new_addr->ptr.str=new std::string; break;
        case vm_vec:  new_addr->ptr.vec=new nas_vec;     break;
        case vm_hash: new_addr->ptr.hash=new nas_hash;   break;
        case vm_func: new_addr->ptr.func=new nas_func;   break;
    }
    return new_addr;
}

void gc_init()
{
    for(int i=0;i<MEMSIZE;++i)
    {
        nas_val* tmp=new nas_val;
        memory.push_back(tmp);
        free_space.push(tmp);
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
    global_scope.clear();
    local_scope.clear();
    return;
}
#endif