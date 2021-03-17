#ifndef __NAS_GC_H__
#define __NAS_GC_H__

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

struct nas_vec
{
    nas_gc& gc;
    std::vector<nas_val*> elems;

    nas_vec(nas_gc&);
    ~nas_vec();
    void      add_elem(nas_val*);
    nas_val*  del_elem();
    int       size();
    nas_val*  get_val(int);
    nas_val** get_mem(int);
    void      print();
};

struct nas_hash
{
    nas_gc& gc;
    std::unordered_map<std::string,nas_val*> elems;

    nas_hash(nas_gc&);
    ~nas_hash();
    void      add_elem(std::string,nas_val*);
    void      del_elem(std::string);
    int       size();
    nas_val*  get_val(std::string);
    nas_val** get_mem(std::string);
    bool      check_contain(std::string);
    nas_val*  get_keys();
    void      print();
};

struct nas_func
{
    nas_gc& gc;
    nas_val* scope;
    std::vector<int> para;
    int entry;
    int dynpara;
    bool is_builtin;

    nas_func(nas_gc&);
    ~nas_func();
    void set_entry(int);
    int  get_entry();
    void add_para(int,bool);
    std::vector<int>& get_para();
    int  get_dynamic_para();
    void set_scope(nas_val*);
    void set_new_closure();
    nas_val* get_scope();
};

struct nas_scop
{
    nas_gc& gc;
    std::unordered_map<int,nas_val*> elems;

    nas_scop(nas_gc&);
    ~nas_scop();
    void add_value(int,nas_val*);
    nas_val*  get_val(int);
    nas_val** get_mem(int);
    void set_closure(nas_scop&);
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
    int ref_cnt;
    
    nas_val();
    nas_val(int,nas_gc&);
    ~nas_val();
    void        clear();
    void        set_type(int,nas_gc&);
    void        set_num(double);
    void        set_str(std::string);
    int         get_type();
    double      get_num();
    std::string get_str();
    nas_vec&    get_vec();
    nas_hash&   get_hash();
    nas_func&   get_func();
    nas_scop&   get_scop();
};

class nas_gc
{
private:
    nas_val error_returned_value;
    std::queue<nas_val*> free_space;
    std::vector<nas_val*> memory;
public:
    ~nas_gc();
    void clear();
    nas_val* gc_alloc(int);
    void add_ref(nas_val*);
    void del_ref(nas_val*);
};

/*functions of nas_vec*/
nas_vec::nas_vec(nas_gc& ngc):gc(ngc)
{
    return;
}
nas_vec::~nas_vec()
{
    int size=elems.size();
    for(int i=0;i<size;++i)
        gc.del_ref(elems[i]);
    elems.clear();
    return;
}
void nas_vec::add_elem(nas_val* value_address)
{
    elems.push_back(value_address);
    return;
}
nas_val* nas_vec::del_elem()
{
    // pop back
    if(!elems.size())
        return NULL;
    nas_val* ret=elems.back();
    elems.pop_back();
    return ret;
}
int nas_vec::size()
{
    return elems.size();
}
nas_val* nas_vec::get_val(int index)
{
    int vec_size=elems.size();
    if(index<-vec_size || index>=vec_size)
    {
        std::cout<<">> [vm] nas_vec::get_val: index out of range: "<<index<<".\n";
        return NULL;
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
        return NULL;
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
        switch(tmp->get_type())
        {
            case vm_nil:  std::cout<<"nil";            break;
            case vm_num:  std::cout<<tmp->get_num();   break;
            case vm_str:  std::cout<<tmp->get_str();   break;
            case vm_vec:  tmp->get_vec().print();      break;
            case vm_hash: tmp->get_hash().print();     break;
            case vm_func: std::cout<<"func(...){...}"; break;
        }
        std::cout<<",]"[i==size-1];
    }
    return;
}

/*functions of nas_hash*/
nas_hash::nas_hash(nas_gc& ngc):gc(ngc)
{
    return;
}
nas_hash::~nas_hash()
{
    for(auto iter=elems.begin();iter!=elems.end();++iter)
        gc.del_ref(iter->second);
    elems.clear();
    return;
}
void nas_hash::add_elem(std::string key,nas_val* value_address)
{
    if(!elems.count(key))
        elems[key]=value_address;
    return;
}
void nas_hash::del_elem(std::string key)
{
    if(elems.count(key))
    {
        gc.del_ref(elems[key]);
        elems.erase(key);
    }
    return;
}
int nas_hash::size()
{
    return elems.size();
}
nas_val* nas_hash::get_val(std::string key)
{
    if(elems.count(key))
        return elems[key];
    std::cout<<">> [vm] nas_hash::get_val: cannot find member named \""<<key<<"\".\n";
    return NULL;
}
nas_val** nas_hash::get_mem(std::string key)
{
    if(elems.count(key))
        return &elems[key];
    std::cout<<">> [vm] nas_hash::get_mem: cannot find member named \""<<key<<"\".\n";
    return NULL;
}
bool nas_hash::check_contain(std::string key)
{
    return elems.count(key);
}
nas_val* nas_hash::get_keys()
{
    nas_val* ret_addr=gc.gc_alloc(vm_vec);
    nas_vec& ref_vec=ret_addr->get_vec();
    for(auto iter=elems.begin();iter!=elems.end();++iter)
    {
        nas_val* str_addr=gc.gc_alloc(vm_str);
        str_addr->set_str(iter->first);
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
        switch(tmp->get_type())
        {
            case vm_nil:  std::cout<<"nil";            break;
            case vm_num:  std::cout<<tmp->get_num();   break;
            case vm_str:  std::cout<<tmp->get_str();   break;
            case vm_vec:  tmp->get_vec().print();      break;
            case vm_hash: tmp->get_hash().print();     break;
            case vm_func: std::cout<<"func(...){...}"; break;
        }
        std::cout<<',';
    }
    std::cout<<'}';
    return;
}

/*functions of nas_func*/
nas_func::nas_func(nas_gc& ngc):gc(ngc)
{
    is_builtin=false;
    scope=NULL;
    dynpara=-1;
    return;
}
nas_func::~nas_func()
{
    if(scope)
        gc.del_ref(scope);
    return;
}
void nas_func::set_entry(int etr)
{
    entry=etr;
    return;
}
int nas_func::get_entry()
{
    return entry;
}
void nas_func::add_para(int name_index,bool is_dynamic=false)
{
    if(is_dynamic)
    {
        dynpara=name_index;
        return;
    }
    para.push_back(name_index);
    return;
}
std::vector<int>& nas_func::get_para()
{
    return para;
}
int nas_func::get_dynamic_para()
{
    return dynpara;
}
void nas_func::set_scope(nas_val* value_address)
{
    nas_val* new_closure=gc.gc_alloc(vm_scop);
    new_closure->get_scop().set_closure(value_address->get_scop());
    scope=new_closure;
    return;
}
void nas_func::set_new_closure()
{
    scope=gc.gc_alloc(vm_scop);
    return;
}
nas_val* nas_func::get_scope()
{
    return scope;
}

/*functions of nas_scop*/
nas_scop::nas_scop(nas_gc& ngc):gc(ngc)
{
    return;
}
nas_scop::~nas_scop()
{
    for(auto i=elems.begin();i!=elems.end();++i)
        gc.del_ref(i->second);
    return;
}
void nas_scop::add_value(int key,nas_val* value_address)
{
    if(elems.count(key))
    {
        // if this value already exists,delete the old value and update a new value
        nas_val* old_val_address=elems[key];
        gc.del_ref(old_val_address);
    }
    elems[key]=value_address;
    return;
}
nas_val* nas_scop::get_val(int key)
{
    if(elems.count(key))
        return (elems)[key];
    return NULL;
}
nas_val** nas_scop::get_mem(int key)
{
    if(elems.count(key))
        return &(elems[key]);
    return NULL;
}
void nas_scop::set_closure(nas_scop& tmp)
{
    for(auto i=elems.begin();i!=elems.end();++i)
        gc.del_ref(i->second);
    elems.clear();
    for(auto i=tmp.elems.begin();i!=tmp.elems.end();++i)
    {
        nas_val* value_addr=i->second;
        ++value_addr->ref_cnt;
        elems[i->first]=value_addr;
    }
    return;
}

/*functions of nas_val*/
nas_val::nas_val()
{
    mark=false;
    ref_cnt=1;
    type=vm_nil;
    return;
}
nas_val::nas_val(int nas_val_type,nas_gc& ngc)
{
    mark=false;
    ref_cnt=1;
    type=nas_val_type;
    switch(nas_val_type)
    {
        case vm_nil:  break;
        case vm_num:  ptr.num=0;                  break;
        case vm_str:  ptr.str=new std::string;    break;
        case vm_vec:  ptr.vec=new nas_vec(ngc);   break;
        case vm_hash: ptr.hash=new nas_hash(ngc); break;
        case vm_func: ptr.func=new nas_func(ngc); break;
        case vm_scop: ptr.cls=new nas_scop(ngc);  break;
    }
    return;
}
nas_val::~nas_val()
{
    // must set type and scalar_ptr to default first
    // this operation will avoid SIGTRAP caused by circular reference
    // circular reference will cause using destructor repeatedly
    int tmp_type=type;
    type=vm_nil;
    switch(tmp_type)
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
    int tmp_type=type;
    type=vm_nil;
    switch(tmp_type)
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
void nas_val::set_type(int nas_val_type,nas_gc& ngc)
{
    type=nas_val_type;
    switch(nas_val_type)
    {
        case vm_nil:  break;
        case vm_num:  ptr.num=0;                  break;
        case vm_str:  ptr.str=new std::string;    break;
        case vm_vec:  ptr.vec=new nas_vec(ngc);   break;
        case vm_hash: ptr.hash=new nas_hash(ngc); break;
        case vm_func: ptr.func=new nas_func(ngc); break;
        case vm_scop: ptr.cls=new nas_scop(ngc);  break;
    }
    return;
}
void nas_val::set_num(double num)
{
    ptr.num=num;
    return;
}
void nas_val::set_str(std::string str)
{
    *ptr.str=str;
    return;
}
int nas_val::get_type()
{
    return type;
}
double nas_val::get_num()
{
    return ptr.num;
}
std::string nas_val::get_str()
{
    return *ptr.str;
}
nas_vec& nas_val::get_vec()
{
    return *ptr.vec;
}
nas_hash& nas_val::get_hash()
{
    return *ptr.hash;
}
nas_func& nas_val::get_func()
{
    return *ptr.func;
}
nas_scop& nas_val::get_scop()
{
    return *ptr.cls;
}

/*functions of nas_gc*/
nas_gc::~nas_gc()
{
    int gc_mem_size=memory.size();
    for(int i=0;i<gc_mem_size;++i)
        memory[i]->clear();
    for(int i=0;i<gc_mem_size;++i)
        delete memory[i];
    while(!free_space.empty())
        free_space.pop();
    memory.clear();
    return;
}
void nas_gc::clear()
{
    int gc_mem_size=memory.size();
    for(int i=0;i<gc_mem_size;++i)
        memory[i]->clear();
    for(int i=0;i<gc_mem_size;++i)
        delete memory[i];
    while(!free_space.empty())
        free_space.pop();
    memory.clear();
    return;
}
nas_val* nas_gc::gc_alloc(int val_type)
{
    if(free_space.empty())
    {
        nas_val* new_unit=new nas_val(val_type,*this);
        memory.push_back(new_unit);
        return new_unit;
    }
    nas_val* ret=free_space.front();
    free_space.pop();
    ret->ref_cnt=1;
    ret->set_type(val_type,*this);
    return ret;
}
void nas_gc::add_ref(nas_val* value_address)
{
    ++value_address->ref_cnt;
    return;
}
void nas_gc::del_ref(nas_val* value_address)
{
    --value_address->ref_cnt;
    if(!value_address->ref_cnt)
    {
        value_address->clear();
        free_space.push(value_address);
    }
    return;
}

#endif