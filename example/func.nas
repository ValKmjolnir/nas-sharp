var f=func(x)
{
    var m=nil;
    if(x<10)
        m=func(y)
        {
            return x/y;
        };
    else if(x<50)
        m=func(y)
        {
            return x*y;
        };
    return m;
};