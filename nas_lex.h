#ifndef __NAS_LEX_H__
#define __NAS_LEX_H__

#define IS_DIGIT(c) ('0'<=(c) && (c)<='9')
#define IS_ID(c) (('a'<=(c) && (c)<='z')||('A'<=(c) && (c)<='Z') || (c)=='_')
#define SINGLE_OPR(c) ((c)==';' || (c)==',' || (c)==';' || (c)==':' || (c)=='?' || (c)=='(' || (c)==')' || (c)=='[' || (c)==']' || (c)=='{' || (c)=='}')
#define MULT_OPR(c) ((c)=='+' || (c)=='-' || (c)=='*' || (c)=='/' || (c)=='~' || (c)=='=' || (c)=='!' || (c)=='<' || (c)=='>')
enum token_type
{
    tok_eof,
    tok_nil,
    tok_id,
    tok_num,
    tok_str,
    tok_lcurve,tok_rcurve,
    tok_lbracket,tok_rbracket,
    tok_lbrace,tok_rbrace,
    tok_var,
    tok_func,
    tok_if,
    tok_else,
    tok_while,
    tok_for,
    tok_continue,
    tok_break,
    tok_ret,
    tok_semi,
    tok_colon,
    tok_comma,
    tok_quesmark,
    tok_dot,
    tok_ellipsis,
    tok_minus,
    tok_not,
    tok_plus,
    tok_link,
    tok_div,
    tok_mult,
    tok_equal,
    tok_pluseq,
    tok_minuseq,
    tok_linkeq,
    tok_multeq,
    tok_diveq,
    tok_cmpeq,
    tok_cmpneq,
    tok_less,
    tok_leq,
    tok_grt,
    tok_geq,
    tok_and,
    tok_or
};

struct
{
    int type;
    const char* content;
} token_table[]=
{
    {tok_nil,      "nil"      },
    {tok_var,      "var"      },
    {tok_func,     "func"     },
    {tok_if,       "if"       },
    {tok_else,     "else"     },
    {tok_while,    "while"    },
    {tok_for,      "for"      },
    {tok_continue, "continue" },
    {tok_break,    "break"    },
    {tok_ret,      "return"   },
    {tok_and,      "and"      },
    {tok_or,       "or"       },
    {tok_semi,     ";"        },
    {tok_colon,    ":"        },
    {tok_comma,    ","        },
    {tok_quesmark, "?"        },
    {tok_dot,      "."        },
    {tok_ellipsis, "..."      },
    {tok_minus,    "-"        },
    {tok_not,      "!"        },
    {tok_plus,     "+"        },
    {tok_link,     "~"        },
    {tok_div,      "/"        },
    {tok_mult,     "*"        },
    {tok_equal,    "="        },
    {tok_pluseq,   "+="       },
    {tok_minuseq,  "-="       },
    {tok_linkeq,   "~="       },
    {tok_multeq,   "*="       },
    {tok_diveq,    "/="       },
    {tok_cmpeq,    "=="       },
    {tok_cmpneq,   "!="       },
    {tok_less,     "<"        },
    {tok_leq,      "<="       },
    {tok_grt,      ">"        },
    {tok_geq,      ">="       },
    {tok_lcurve,   "("        },
    {tok_rcurve,   ")"        },
    {tok_lbracket, "["        },
    {tok_rbracket, "]"        },
    {tok_lbrace,   "{"        },
    {tok_rbrace,   "}"        },
    {tok_eof,      NULL       }
};

int line;
int ptr;
std::string res;
struct{int type;int line;std::string content;} token;
void open(std::string filename)
{
    std::ifstream fin(filename,std::ios::binary);
    if(fin.fail())
    {
        die(">> failed to open file: "+filename);
        fin.close();
        return;
    }
    res="";
    while(!fin.eof())
    {
        char c=fin.get();
        if(fin.eof()) break;
        res+=c;
    }
    fin.close();
    return;
}

void init()
{
    line=1;
    ptr=0;
    error=0;
    return;
}

double strtonum(std::string str)
{
    int i=0;
    double sum=0;
    while(str[i] && str[i]!='.')
        sum=sum*10+(str[i++]-'0');
    if(!str[i]) return sum;
    ++i;
    double t=0.1;
    while(str[i])
    {
        sum+=t*(str[i++]-'0');
        t/=10.0;
    }
    return sum;
}

void next()
{
    token.type=tok_eof;
    token.line=line;
    token.content="";
    while(res[ptr]&&(res[ptr]==' ' || res[ptr]=='\t' || res[ptr]=='\r' || res[ptr]=='\n' || res[ptr]=='#'))
    {
        if(res[ptr]=='#')
            while(res[ptr] && res[ptr]!='\n')
                ++ptr;
        if(res[ptr]=='\n')++line;
        if(!res[ptr])break;
        ++ptr;
    }
    if(!res[ptr]) return;
    if(IS_DIGIT(res[ptr]))
    {
        bool err_fmt=false;
        while(IS_DIGIT(res[ptr]) || res[ptr]=='.' || IS_ID(res[ptr]))
        {
            token.content+=res[ptr];
            if(IS_ID(res[ptr])) err_fmt=true;
            ++ptr;
        }
        if(token.content.back()=='.' || err_fmt)
            die("error number.",line);
        token.type=tok_num;
    }
    else if(res[ptr]=='\'' || res[ptr]=='\"')
    {
        char tmp=res[ptr++];
        while(res[ptr] && res[ptr]!=tmp)
        {
            if(res[ptr]=='\n') ++line;
            if(res[ptr]=='\\' && res[ptr+1])
            {
                switch(res[ptr+1])
                {
                    case 'a': token.content+='\a';break;
                    case 'b': token.content+='\b';break;
                    case 'f': token.content+='\f';break;
                    case 'n': token.content+='\n';break;
                    case 'r': token.content+='\r';break;
                    case 't': token.content+='\t';break;
                    case 'v': token.content+='\v';break;
                    case '?': token.content+='\?';break;
                    case '0': token.content+='\0';break;
                    case '\\':token.content+='\\';break;
                    case '\'':token.content+='\'';break;
                    case '\"':token.content+='\"';break;
                    default:  token.content+=res[ptr];break;
                }
                ptr+=2;
                continue;
            }
            token.content+=res[ptr++];
        }
        if(!res[ptr])
            die("get eof when generating string.",line);
        ++ptr;
        token.type=tok_str;
    }
    else if(res[ptr]=='.')
    {
        if(res[ptr+1] && res[ptr+2] && res[ptr+1]=='.' && res[ptr+2]=='.')
        {
            token.content="...";
            ptr+=2;
        }
        else
            token.content=".";
        ++ptr;
        for(int i=0;token_table[i].content;++i)
            if(token_table[i].content==token.content)
                token.type=token_table[i].type;
    }
    else if(IS_ID(res[ptr]))
    {
        while(IS_ID(res[ptr]) || IS_DIGIT(res[ptr]))
            token.content+=res[ptr++];
        for(int i=0;token_table[i].content;++i)
            if(token_table[i].content==token.content)
                token.type=token_table[i].type;
        token.type=token.type==tok_eof?tok_id:token.type;
    }
    else if(SINGLE_OPR(res[ptr]))
    {
        token.content+=res[ptr++];
        for(int i=0;token_table[i].content;++i)
            if(token_table[i].content==token.content)
                token.type=token_table[i].type;
    }
    else if(MULT_OPR(res[ptr]))
    {
        token.content+=res[ptr++];
        if(res[ptr]=='=')
            token.content+=res[ptr++];
        for(int i=0;token_table[i].content;++i)
            if(token_table[i].content==token.content)
                token.type=token_table[i].type;
    }
    else
        die("unknown character: "+(res[ptr]>0? res[ptr]:'?'),line);
    return;
}
#endif