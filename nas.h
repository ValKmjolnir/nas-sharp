#ifndef __NAS_H__
#define __NAS_H__

#pragma GCC optimize(2)

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <stack>
#include <list>
#include <vector>
#include <map>
#include <queue>
#include <cmath>
#include <ctime>

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
#include "nas_gc.h"
#include "nas_builtin.h"
#include "nas_code.h"
#include "nas_vm.h"


#endif