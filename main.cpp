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
        if(error) continue;
        par.print(0);
    }
    return 0;
}