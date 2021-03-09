var fib=func(x)
{
    if(x<2) return x;
    return fib(x-1)+fib(x-2);
};

for(var i=0;i<4000000;i+=1);
print(fib(10),'\n');