
var map=nil;

var check=func(x,y)
{
    if(x>14) x=0;
    if(y>19) y=0;
    return map[x][y];
}

var new_map=func()
{
    var tmp=[];
    for(var i=0;i<15;i+=1)
    {
        var t=[];
        for(var j=0;j<20;j+=1)
            append(t,nil);
        append(tmp,t);
    }
    return tmp;
}

var prt=func()
{
    system("cls");
    var s='';
    for(var i=0;i<15;i+=1)
    {
        for(var j=0;j<20;j+=1)
            s~=map[i][j]~' ';
        s~='\n';
    }
    print(s);
}

func()
{
    rand(time(0));
    map=new_map();
    for(var i=0;i<15;i+=1)
        for(var j=0;j<20;j+=1)
            map[i][j]=rand()>0.7?'O':' ';
    var calc=[[0,1],[1,0],[0,-1],[-1,0],[1,1],[1,-1],[-1,-1],[-1,1]];
    for(var r=0;r<100;r+=1)
    {
        prt();
        var tmp=new_map();
        for(var i=0;i<15;i+=1)
            for(var j=0;j<20;j+=1)
            {
                var cnt=0;
                for(var k=0;k<8;k+=1)
                    cnt+=(check(i+calc[k][0],j+calc[k][1])=='O');
                if(cnt==2)
                    tmp[i][j]=map[i][j];
                else if(cnt==3)
                    tmp[i][j]='O';
                else
                    tmp[i][j]=' ';
            }
        map=tmp;
    }
    return;
}();