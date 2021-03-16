var new=func(name,age)
{
    var student={name:name,age:age};
    return
    {
        setname:func(name){student.name=name;},
        getname:func(){return student.name;},
        setage:func(age){student.age=age;},
        getage:func(){return student.age;}
    };
}

var s=new("steve",24);
print(s.getname(),' ',s.getage(),'\n');
s.setname("alex");
print(s.getname(),' ',s.getage(),'\n');
s.setage(19);
print(s.getname(),' ',s.getage(),'\n');
