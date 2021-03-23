var student=func(name,age)
{
    var val={
        name:name,
        age:age
    };
    return
    {
        setname:func(name){val.name=name;},
        getname:func(){return val.name;},
        setage:func(age){val.age=age;},
        getage:func(){return val.age;}
    };
}

var s=student("steve",24);
print(s.getname(),' ',s.getage(),'\n');
s.setname("alex");
print(s.getname(),' ',s.getage(),'\n');
s.setage(19);
print(s.getname(),' ',s.getage(),'\n');
