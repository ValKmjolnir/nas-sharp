#ifndef __NAS_H__
#define __NAS_H__

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <list>
#include <vector>
#include <map>

int error=0;
void die(std::string info="",int line=-1)
{
    ++error;
    std::cout<<">> [error] ";
    if(line>0) std::cout<<"line "<<line<<": ";
    std::cout<<info<<'\n';
    return;
}

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

#include "nas_lex.h"
#include "nas_ast.h"
#include "nas_par.h"
#endif