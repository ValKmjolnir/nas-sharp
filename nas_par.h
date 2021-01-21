#ifndef __NAS_PAR_H__
#define __NAS_PAR_H__

int in_loop=0;
int in_func=0;
void panic()
{
    while(token.type!= tok_semi && token.type!=tok_rbrace && token.type!=tok_eof)
        next();
    return;
}
void match(int type)
{
    if(token.type!=type)
    {
        switch(type)
        {
            case tok_id:die("expect identifier.",token.line);break;
            case tok_num:die("expect number.",token.line);break;
            case tok_str:die("expect string.",token.line);break;
            default:
            {
                for(int i=0;token_table[i].content;++i)
                    if(type==token_table[i].type)
                    {
                        die("expect \""+(std::string)token_table[i].content+"\".",token.line);
                        break;
                    }
                break;
            }
        }
        panic();
    }
    next();
    return;
}
nas_ast proc();
nas_ast expr();
nas_ast exprs();
nas_ast listgen();
nas_ast hashgen();
nas_ast funcgen();
nas_ast definition();
nas_ast conditional();
nas_ast whilegen();
nas_ast forgen();
nas_ast retgen();
nas_ast calc();
nas_ast or_expr();
nas_ast and_expr();
nas_ast cmp_expr();
nas_ast additive();
nas_ast multive();
nas_ast scalar();
bool check_assign(nas_ast&);
nas_ast call();

nas_ast proc()
{
    in_loop=0;
    in_func=0;
    next();
    nas_ast root(token.line,ast_root);
    while(token.type!=tok_eof)
        root.add_child(expr());
    return root;
}

nas_ast expr()
{
    nas_ast node(token.line);
    switch(token.type)
    {
        case tok_nil:
        case tok_id:
        case tok_minus:
        case tok_not:
        case tok_num:
        case tok_str:
        case tok_func:
        case tok_lcurve:
        case tok_lbracket:
        case tok_lbrace: node=calc();        break;
        case tok_var:    node=definition();  break;
        case tok_if:     node=conditional(); break;
        case tok_while:  node=whilegen();    break;
        case tok_for:    node=forgen();      break;
        case tok_continue:
            if(!in_loop) die("cannot use continue outside loops.",token.line);
            match(tok_continue);
            node.set_type(ast_continue);
            break;
        case tok_break:
            if(!in_loop) die("cannot use break outside loops.",token.line);
            match(tok_break);
            node.set_type(ast_break);
            break;
        case tok_ret:    node=retgen();      break;
        case tok_semi:break;
        default:
            die(" expect expression.",token.line);
            panic();
            break;
    }
    int bk_tp=node.get_type();
    if(token.type==tok_semi || (bk_tp!=ast_while && bk_tp!=ast_for && bk_tp!=ast_conditional))
        match(tok_semi);
    return node;
}

nas_ast exprs()
{
    nas_ast node(token.line,ast_block);
    if(token.type!=tok_lbrace)
    {
        node.add_child(expr());
        return node;
    }
    match(tok_lbrace);
    while(token.type!=tok_rbrace && token.type!=tok_eof)
        node.add_child(expr());
    match(tok_rbrace);
    return node;
}

nas_ast listgen()
{
    nas_ast node(token.line,ast_list);
    match(tok_lbracket);
    while(token.type!=tok_rbracket)
    {
        node.add_child(calc());
        if(token.type!=tok_comma) break;
        match(tok_comma);
    }
    match(tok_rbracket);
    return node;
}

nas_ast hashgen()
{
    nas_ast node(token.line,ast_hash);
    match(tok_lbrace);
    while(token.type!=tok_rbrace)
    {
        nas_ast hashmember(token.line,ast_hashmember);
        hashmember.set_str(token.content);
        match(token.type==tok_id? tok_id:tok_str);
        match(tok_colon);
        hashmember.add_child(calc());
        node.add_child(hashmember);
        if(token.type!=tok_comma) break;
        match(tok_comma);
    }
    match(tok_rbrace);
    return node;
}

nas_ast funcgen()
{
    ++in_func;
    nas_ast node(token.line,ast_func);
    match(tok_func);
    if(token.type==tok_lcurve)
    {
        nas_ast paralist(token.line,ast_paralist);
        match(tok_lcurve);
        while(token.type!=tok_rcurve)
        {
            nas_ast para(token.line,ast_para);
            para.set_str(token.content);
            match(tok_id);
            if(token.type==tok_ellipsis)
            {
                match(tok_ellipsis);
                para.set_type(ast_dynpara);
                paralist.add_child(para);
                break;
            }
            paralist.add_child(para);
            if(token.type!=tok_comma) break;
            match(tok_comma);
        }
        match(tok_rcurve);
        node.add_child(paralist);
    }
    node.add_child(exprs());
    --in_func;
    return node;
}

nas_ast definition()
{
    nas_ast node(token.line,ast_definition);
    match(tok_var);
    node.set_str(token.content);
    match(tok_id);
    match(tok_equal);
    node.add_child(calc());
    return node;
}

nas_ast conditional()
{
    nas_ast node(token.line,ast_conditional);
    nas_ast ifnode(token.line,ast_if);
    match(tok_if);
    match(tok_lcurve);
    ifnode.add_child(calc());
    match(tok_rcurve);
    ifnode.add_child(exprs());
    node.add_child(ifnode);
    if(token.type==tok_else)
    {
        nas_ast elsenode(token.line,ast_else);
        match(tok_else);
        elsenode.add_child(exprs());
        node.add_child(elsenode);
    }
    return node;
}

nas_ast whilegen()
{
    ++in_loop;
    nas_ast node(token.line,ast_while);
    match(tok_while);
    match(tok_lcurve);
    node.add_child(calc());
    match(tok_rcurve);
    node.add_child(exprs());
    --in_loop;
    return node;
}

nas_ast forgen()
{
    ++in_loop;
    nas_ast node(token.line,ast_for);
    match(tok_for);
    match(tok_lcurve);
    if(token.type!=tok_semi)
        node.add_child(token.type==tok_var?definition():calc());
    else
    {
        nas_ast nullnode;
        node.add_child(nullnode);
    }
    match(tok_semi);
    if(token.type!=tok_semi)
        node.add_child(calc());
    else
    {
        nas_ast nullnode;
        node.add_child(nullnode);
    }
    match(tok_semi);
    if(token.type!=tok_rcurve)
        node.add_child(calc());
    else
    {
        nas_ast nullnode;
        node.add_child(nullnode);
    }
    match(tok_rcurve);
    node.add_child(exprs());
    --in_loop;
    return node;
}

nas_ast retgen()
{
    if(!in_func) die("cannot use return outside functions.",token.line);
    nas_ast node(token.line,ast_ret);
    match(tok_ret);
    switch(token.type)
    {
        case tok_nil:
        case tok_id:
        case tok_minus:
        case tok_not:
        case tok_num:
        case tok_str:
        case tok_func:
        case tok_lcurve:
        case tok_lbracket:
        case tok_lbrace: node.add_child(calc());break;
        default:
        {
            nas_ast nilnode(token.line,ast_nil);
            node.add_child(nilnode);
        }
    }
    return node;
}

nas_ast calc()
{
    nas_ast node=or_expr();
    if(token.type==tok_quesmark)
    {
        nas_ast tmp(token.type,ast_trino);
        match(tok_quesmark);
        tmp.add_child(node);
        tmp.add_child(calc());
        match(tok_colon);
        tmp.add_child(calc());
        node=tmp;
    }
    return node;
}

nas_ast or_expr()
{
    nas_ast node=and_expr();
    while(token.type==tok_or)
    {
        nas_ast tmp(token.line,ast_or);
        match(tok_or);
        tmp.add_child(node);
        tmp.add_child(and_expr());
        node=tmp;
    }
    return node;
}

nas_ast and_expr()
{
    nas_ast node=cmp_expr();
    while(token.type==tok_and)
    {
        nas_ast tmp(token.line,ast_and);
        match(tok_and);
        tmp.add_child(node);
        tmp.add_child(cmp_expr());
        node=tmp;
    }
    return node;
}

nas_ast cmp_expr()
{
    nas_ast node=additive();
    while(tok_cmpeq<=token.type && token.type<=tok_geq)
    {
        nas_ast tmp(token.line,token.type-tok_cmpeq+ast_cmpeq);
        match(token.type);
        tmp.add_child(node);
        tmp.add_child(additive());
        node=tmp;
    }
    return node;
}

nas_ast additive()
{
    nas_ast node=multive();
    while(token.type==tok_plus || token.type==tok_minus || token.type==tok_link)
    {
        nas_ast tmp(token.line);
        switch(token.type)
        {
            case tok_plus:  tmp.set_type(ast_plus);  break;
            case tok_minus: tmp.set_type(ast_minus); break;
            case tok_link:  tmp.set_type(ast_link);  break;
        }
        match(token.type);
        tmp.add_child(node);
        tmp.add_child(multive());
        node=tmp;
    }
    return node;
}

nas_ast multive()
{
    nas_ast node=scalar();
    while(token.type==tok_mult || token.type==tok_div)
    {
        nas_ast tmp(token.line,token.type==tok_mult?ast_mult:ast_div);
        match(token.type);
        tmp.add_child(node);
        tmp.add_child(scalar());
        node=tmp;
    }
    return node;
}

nas_ast scalar()
{
    if(token.type==tok_minus || token.type==tok_not)
    {
        nas_ast node(token.line,token.type==tok_minus? ast_minus:ast_not);
        match(token.type);
        node.add_child(scalar());
        return node;
    }
    return call();
}

bool check_assign(nas_ast& node)
{
    if(node.get_type()==ast_id)
        return true;
    if(node.get_type()==ast_call && node.get_children().front().get_type()==ast_id)
    {
        for(int i=0;i<node.get_children().size();++i)
            if(node.get_children()[i].get_type()==ast_callf)
                return false;
        return true;
    }
    return false;
}

nas_ast call()
{
    nas_ast node(token.line,ast_call);
    switch(token.type)
    {
        case tok_nil:{nas_ast tmp(token.line,ast_nil); node=tmp;} match(tok_nil);break;
        case tok_id: {nas_ast tmp(token.line,ast_id ); tmp.set_str(token.content); node=tmp;} match(tok_id);break;
        case tok_str:{nas_ast tmp(token.line,ast_str); tmp.set_str(token.content); node=tmp;} match(tok_str);break;
        case tok_num:{nas_ast tmp(token.line,ast_num); tmp.set_num(strtonum(token.content)); node=tmp;} match(tok_num);break;
        case tok_func:     node=funcgen();break;
        case tok_lbracket: node=listgen();break;
        case tok_lbrace:   node=hashgen();break;
        case tok_lcurve:
            match(tok_lcurve);
            node=calc();
            match(tok_rcurve);
            break;
    }
    if(token.type==tok_dot || token.type==tok_lbracket || token.type==tok_lcurve)
    {
        nas_ast tmp(token.line,ast_call);
        tmp.add_child(node);
        node=tmp;
    }
    while(token.type==tok_dot || token.type==tok_lbracket || token.type==tok_lcurve)
    {
        if(token.type==tok_dot)
        {
            match(tok_dot);
            nas_ast tmp(token.line,ast_callh);
            tmp.set_str(token.content);
            node.add_child(tmp);
            match(tok_id);
        }
        else if(token.type==tok_lbracket)
        {
            match(tok_lbracket);
            nas_ast tmp(token.line,ast_callv);
            tmp.add_child(calc());
            node.add_child(tmp);
            match(tok_rbracket);
        }
        else
        {
            match(tok_lcurve);
            nas_ast tmp(token.line,ast_callf);
            while(token.type!=tok_rcurve)
            {
                tmp.add_child(calc());
                if(token.type!=tok_comma) break;
                match(tok_comma);
            }
            match(tok_rcurve);
            node.add_child(tmp);
        }
    }
    if(tok_equal<=token.type && token.type<=tok_diveq)
    {
        if(!check_assign(node))
            die("cannot assign unreachable memory space.",node.get_line());
        nas_ast tmp(token.line,token.type-tok_equal+ast_eq);
        match(token.type);
        tmp.add_child(node);
        tmp.add_child(calc());
        node=tmp;
    }
    return node;
}
#endif