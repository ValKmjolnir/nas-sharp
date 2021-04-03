# nas-sharp script

this is a language for experiment

stack-based virtual machine

call-threading instruction dispatch

strong typing: cannot plus a number with a string(even this string is numerable)

LL(1) parser

easy to add your own built-in functions and use them

don't need to learn too much to use this language

# value types

function has closure

```javascript

var i=nil;
var i=1;
var i=0.1;
var i='str';
var i="str";
var i=[0,1,nil,[],{},func(){}];
var i={a:0,b:'str','s':[],s1:{}};
var i=func(x,elem...)
{
    var sum=x;
    for(var i=0;i<size(elem);i+=1)
        sum+=elem[i];
    return sum;
}
```

# operators

when calling a hash,use

> hash.member_name;

or

> hash['member_name'];

when calling a vector/string,use

> vec[index];

when calling a function,use

> fun(arguments);

```javascript
1+1;
2-2;
3*3;
4/4;
'hello '~'world';
!0;
!!0;
-1;

i+=1;
i-=1;
i*=1;
i/=1;
i~='str';

1==1;
1!=2;
1<2;
1<=2;
1>0;
1>=1;

1 and 2;
1 or 2;
```

# loop
```javascript
while(condition)
{
    exprs;
    continue;
    break;
}
for(definition;condition;afterloop)
{
    exprs;
    continue;
    break;
}
```

# if-else
```javascript
if(1)
{
    ;
}
else if(2)
{
    ;
}
else
{
    ;
}
```