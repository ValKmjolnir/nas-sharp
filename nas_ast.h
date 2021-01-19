#ifndef __NAS_AST_H__
#define __NAS_AST_H__

enum ast_node_type
{
    ast_null,
    ast_root,
    ast_num,
    ast_str
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
    void set_str(std::string);
    void set_num(double);
    void add_child(nas_ast&);
    std::string get_str();
    double get_num();
    std::vector<nas_ast>& get_children();
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

void nas_ast::add_child(nas_ast& node)
{
    children.push_back(node);
    return;
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
#endif