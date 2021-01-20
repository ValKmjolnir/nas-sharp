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

#include "nas_lex.h"
#include "nas_ast.h"
#include "nas_par.h"
#include "nas_code.h"
#endif