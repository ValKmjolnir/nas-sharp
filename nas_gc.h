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

class nas_vm;
class nas_vec;
class nas_hash;
class nas_func;
class nas_scop;
class nas_val;

class nas_vec
{
private:
    nas_vm& vm;
    std::vector<nas_val*> elems;
public:
    nas_vec(nas_vm&);
    ~nas_vec();
    void      add_elem(nas_val*);
    nas_val*  del_elem();
    int       size();
    nas_val*  get_val(int);
    nas_val** get_mem(int);
    void      print();
};

class nas_hash
{
private:
    nas_vm& vm;
    std::unordered_map<std::string,nas_val*> elems;
public:
    nas_hash(nas_vm&);
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

class nas_func
{
private:
    nas_vm& vm;
    nas_val* scope;
    std::vector<int> para;
    int entry;
    int dynpara;
public:
    bool is_builtin;
    nas_func(nas_vm&);
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

class nas_scop
{
private:
    nas_vm& vm;
    std::unordered_map<int,nas_val*> elems;
public:
    nas_scop(nas_vm&);
    ~nas_scop();
    void add_value(int,nas_val*);
    nas_val*  get_val(int);
    nas_val** get_mem(int);
    void set_closure(nas_scop&);
};

class nas_val
{
protected:
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
public:
    int ref_cnt;
    nas_val();
    nas_val(int,nas_vm&);
    ~nas_val();
    void        clear();
    void        set_type(int,nas_vm&);
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

class nas_vm
{
private:
    nas_val error_returned_value;
    std::queue<nas_val*> free_space;
    std::vector<nas_val*> memory;
public:
    ~nas_vm();
    void clear();
    nas_val* gc_alloc(int);
    void add_ref(nas_val*);
    void del_ref(nas_val*);
};

/*functions of nas_vec*/
nas_vec::nas_vec(nas_vm& nvm):vm(nvm)
{
    return;
}
nas_vec::~nas_vec()
{
    int size=elems.size();
    for(int i=0;i<size;++i)
        vm.del_ref(elems[i]);
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
nas_hash::nas_hash(nas_vm& nvm):vm(nvm)
{
    return;
}
nas_hash::~nas_hash()
{
    for(auto iter=elems.begin();iter!=elems.end();++iter)
        vm.del_ref(iter->second);
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
        vm.del_ref(elems[key]);
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
    nas_val* ret_addr=vm.gc_alloc(vm_vec);
    nas_vec& ref_vec=ret_addr->get_vec();
    for(auto iter=elems.begin();iter!=elems.end();++iter)
    {
        nas_val* str_addr=vm.gc_alloc(vm_str);
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
nas_func::nas_func(nas_vm& nvm):vm(nvm)
{
    is_builtin=false;
    scope=NULL;
    dynpara=-1;
    return;
}
nas_func::~nas_func()
{
    if(scope)
        vm.del_ref(scope);
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
    nas_val* new_closure=vm.gc_alloc(vm_scop);
    new_closure->get_scop().set_closure(value_address->get_scop());
    scope=new_closure;
    return;
}
void nas_func::set_new_closure()
{
    scope=vm.gc_alloc(vm_scop);
    return;
}
nas_val* nas_func::get_scope()
{
    return scope;
}

/*functions of nas_scop*/
nas_scop::nas_scop(nas_vm& nvm):vm(nvm)
{
    return;
}
nas_scop::~nas_scop()
{
    for(auto i=elems.begin();i!=elems.end();++i)
        vm.del_ref(i->second);
    return;
}
void nas_scop::add_value(int key,nas_val* value_address)
{
    if(elems.count(key))
    {
        // if this value already exists,delete the old value and update a new value
        nas_val* old_val_address=elems[key];
        vm.del_ref(old_val_address);
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
        vm.del_ref(i->second);
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
    ref_cnt=1;
    type=vm_nil;
    return;
}
nas_val::nas_val(int nas_val_type,nas_vm& nvm)
{
    ref_cnt=1;
    type=nas_val_type;
    switch(nas_val_type)
    {
        case vm_nil:  break;
        case vm_num:  ptr.num=0;                  break;
        case vm_str:  ptr.str=new std::string;    break;
        case vm_vec:  ptr.vec=new nas_vec(nvm);   break;
        case vm_hash: ptr.hash=new nas_hash(nvm); break;
        case vm_func: ptr.func=new nas_func(nvm); break;
        case vm_scop: ptr.cls=new nas_scop(nvm);  break;
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
void nas_val::set_type(int nas_val_type,nas_vm& nvm)
{
    type=nas_val_type;
    switch(nas_val_type)
    {
        case vm_nil:  break;
        case vm_num:  ptr.num=0;                  break;
        case vm_str:  ptr.str=new std::string;    break;
        case vm_vec:  ptr.vec=new nas_vec(nvm);   break;
        case vm_hash: ptr.hash=new nas_hash(nvm); break;
        case vm_func: ptr.func=new nas_func(nvm); break;
        case vm_scop: ptr.cls=new nas_scop(nvm);  break;
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

/*functions of nas_vm*/
nas_vm::~nas_vm()
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
void nas_vm::clear()
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
nas_val* nas_vm::gc_alloc(int val_type)
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
void nas_vm::add_ref(nas_val* value_address)
{
    ++value_address->ref_cnt;
    return;
}
void nas_vm::del_ref(nas_val* value_address)
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