#include "nas.h"

int main()
{
#ifdef _WIN32
    system("chcp 65001");
	system("cls");
#endif
    std::string filename;
    while(1)
    {
        init();
        std::cout<<">> ";
        std::cin>>filename;
        if(filename=="exit") break;
        open(filename);
        if(error) continue;
        nas_ast par=proc();
        res="";
        if(error) continue;
        proc_gen(par);
        par.get_children().clear();
        run_vm();
    }
    return 0;
}