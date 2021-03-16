while(1 and 2)
{
    break;
    continue;
    while(0)
    {
        continue;
        break;
    }
}

for(var i=0;i<100;i+=1)
    for(var j=i+1;;j+=1)
    {
        break;
        continue;
    }

var fib=func(x)
{
    if(x<2) return x;
    return fib(x-1)+fib(x-2);
}
for(var i=0;i<31;i+=1)
    print(i,' ',fib(i),'\n');