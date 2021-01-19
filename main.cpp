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
        next();
        std::cout<<token.line<<' '<<token.content<<'\n';
        while(token.type!=tok_eof && !error)
        {
            next();
            std::cout<<token.line<<' '<<token.content<<'\n';
        }
    }
    return 0;
}