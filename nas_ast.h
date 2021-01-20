#ifndef __NAS_AST_H__
#define __NAS_AST_H__

enum ast_node_type
{
    ast_null,
    ast_root,
    ast_block,
    ast_nil,
    ast_id,
    ast_num,
    ast_str,
    ast_list,
    ast_hash,
    ast_hashmember,
    ast_func,
    ast_paralist,
    ast_para,
    ast_dynpara,
    ast_definition,
    ast_conditional,
    ast_if,
    ast_else,
    ast_while,
    ast_for,
    ast_continue,
    ast_break,
    ast_ret,
    ast_trino,
    ast_eq,
    ast_pluseq,
    ast_minuseq,
    ast_linkeq,
    ast_multeq,
    ast_diveq,
    ast_or,
    ast_and,
    ast_cmpeq,
    ast_neq,
    ast_less,
    ast_leq,
    ast_grt,
    ast_geq,
    ast_not,
    ast_plus,
    ast_minus,
    ast_mult,
    ast_div,
    ast_link,
    ast_call,
    ast_callh,
    ast_callv,
    ast_callf,
};

struct
{
    int type;
    const char* content;
}ast_node[]=
{
    {ast_null,"null"},
    {ast_root,"root"},
    {ast_block,"blk"},
    {ast_nil,"nil"},
    {ast_id,"id"},
    {ast_num,"num"},
    {ast_str,"str"},
    {ast_list,"list"},
    {ast_hash,"hash"},
    {ast_hashmember,"member"},
    {ast_func,"func"},
    {ast_paralist,"paralist"},
    {ast_para,"para"},
    {ast_dynpara,"dynpara"},
    {ast_definition,"def"},
    {ast_conditional,"if-else"},
    {ast_if,"if"},
    {ast_else,"else"},
    {ast_while,"while"},
    {ast_for,"for"},
    {ast_continue,"continue"},
    {ast_break,"break"},
    {ast_ret,"return"},
    {ast_trino,"trino"},
    {ast_eq,"="},
    {ast_pluseq,"+="},
    {ast_minuseq,"-="},
    {ast_multeq,"*="},
    {ast_diveq,"/="},
    {ast_linkeq,"~="},
    {ast_or,"or"},
    {ast_and,"and"},
    {ast_cmpeq,"=="},
    {ast_neq,"!="},
    {ast_less,"<"},
    {ast_leq,"<="},
    {ast_grt,">"},
    {ast_geq,">="},
    {ast_not,"!"},
    {ast_plus,"+"},
    {ast_minus,"-"},
    {ast_mult,"*"},
    {ast_div,"/"},
    {ast_link,"~"},
    {ast_call,"call"},
    {ast_callh,"callh"},
    {ast_callv,"callv"},
    {ast_callf,"callf"},
    {-1,NULL}
};

class nas_ast
{
private:
    int line;
    int type;
    std::string str;
    double num;
    std::vector<nas_ast> children;
public:
    nas_ast(int,int);
    nas_ast(const nas_ast&);
    nas_ast& operator=(const nas_ast&);
    void operator+=(const nas_ast&);
    void set_type(int);
    void set_str(std::string);
    void set_num(double);
    void add_child(nas_ast);
    int get_line();
    int get_type();
    std::string get_str();
    double get_num();
    std::vector<nas_ast>& get_children();
    void print(int);
};

nas_ast::nas_ast(int _line=0,int _type=ast_null)
{
    line=_line;
    type=_type;
    return;
}

nas_ast::nas_ast(const nas_ast& tmp)
{
    line=tmp.line;
    type=tmp.type;
    str=tmp.str;
    num=tmp.num;
    children=tmp.children;
    return;
}

nas_ast& nas_ast::operator=(const nas_ast& tmp)
{
    line=tmp.line;
    type=tmp.type;
    str=tmp.str;
    num=tmp.num;
    children=tmp.children;
    return *this;
}

void nas_ast::operator+=(const nas_ast& tmp)
{
    int size=tmp.children.size();
    for(int i=0;i<size;++i)
        children.push_back(tmp.children[i]);
    return;
}

void nas_ast::set_type(int _type)
{
    type=_type;
    return;
}

void nas_ast::set_str(std::string _str)
{
    str=_str;
    return;
}

void nas_ast::set_num(double _num)
{
    num=_num;
    return;
}

void nas_ast::add_child(nas_ast node)
{
    children.push_back(node);
    return;
}

int nas_ast::get_line()
{
    return line;
}

int nas_ast::get_type()
{
    return type;
}

std::string nas_ast::get_str()
{
    return str;
}

double nas_ast::get_num()
{
    return num;
}

std::vector<nas_ast>& nas_ast::get_children()
{
    return children;
}

void nas_ast::print(int depth)
{
    for(int i=0;i<depth;++i)
        std::cout<<"|  ";
    for(int i=0;ast_node[i].content;++i)
        if(ast_node[i].type==type)
        {
            std::cout<<ast_node[i].content;
            break;
        }
    switch(type)
    {
        case ast_definition:
        case ast_id:
        case ast_str:
        case ast_para:
        case ast_dynpara:
        case ast_hashmember:
        case ast_callh: std::cout<<": "<<str;break;
        case ast_num:   std::cout<<": "<<num;break;
    }
    std::cout<<"\n";
    for(int i=0;i<children.size();++i)
        children[i].print(depth+1);
    return;
}
#endif