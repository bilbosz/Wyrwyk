# Wyrwyk - Implicit Curve Rasterizer


## Examples
<img src="https://raw.githubusercontent.com/bilbosz/Wyrwyk/master/gallery/fan.gif" width="200" />
```
sin(a)*x == -cos(a)*y
```


```
sin(a)*x == -cos(a)*y
sin( x * x + y * y ) + y > a
cos(a)*x - sin(a)*y == (sin(a)*x + cos(a)*y) * (sin(a)*x + cos(a)*y)
sin( t )
sqrt(x*x + y*y) == a
x*x*x -10*x*y + y*y*y == 0
sin(x)*x*x -10*sin(x)*y + cos(y)*y*y <= 0
sin(x)*x*x -10*sin(x)*y + cos(y)*y*y <= 2000*sin(t)
Ying Yang:
((x*x+(y+0.5)*(y+0.5)<=0.5*0.5&&!(x*x+(y+0.5)*(y+0.5)<=pow(0.5/3,2))||(x>0&&!(x*x+(y-0.5)*(y-0.5)<=0.5*0.5)&&!(x*x+(y+0.5)*(y+0.5)<=pow(0.5/3,2)))||x*x+(y-0.5)*(y-0.5)<=pow(0.5/3,2)||x*x+y*y==1))&&x*x+y*y<=1
(((cos(a)*x-sin(a)*y)*(cos(a)*x-sin(a)*y)+((sin(a)*x+cos(a)*y)+0.5)*((sin(a)*x+cos(a)*y)+0.5)<=0.5*0.5&&!((cos(a)*x-sin(a)*y)*(cos(a)*x-sin(a)*y)+((sin(a)*x+cos(a)*y)+0.5)*((sin(a)*x+cos(a)*y)+0.5)<=pow(0.5/3,2))||((cos(a)*x-sin(a)*y)>0&&!((cos(a)*x-sin(a)*y)*(cos(a)*x-sin(a)*y)+((sin(a)*x+cos(a)*y)-0.5)*((sin(a)*x+cos(a)*y)-0.5)<=0.5*0.5)&&!((cos(a)*x-sin(a)*y)*(cos(a)*x-sin(a)*y)+((sin(a)*x+cos(a)*y)+0.5)*((sin(a)*x+cos(a)*y)+0.5)<=pow(0.5/3,2)))||(cos(a)*x-sin(a)*y)*(cos(a)*x-sin(a)*y)+((sin(a)*x+cos(a)*y)-0.5)*((sin(a)*x+cos(a)*y)-0.5)<=pow(0.5/3,2)||(cos(a)*x-sin(a)*y)*(cos(a)*x-sin(a)*y)+(sin(a)*x+cos(a)*y)*(sin(a)*x+cos(a)*y)==1))&&(cos(a)*x-sin(a)*y)*(cos(a)*x-sin(a)*y)+(sin(a)*x+cos(a)*y)*(sin(a)*x+cos(a)*y)<=1
cos(x*x+y*y) == sin(x*x+y*y)
cos(x*x*x -10*x*y + y*y*y) == sin(x*x+y*y*a+b)
cos(pow(x,y))==sin(pow(x,y))
tan(x*y)==y/x
Batman:
(abs(x)>=4&&-(x/7)*(x/7)+1>0&&y>=-3*sqrt(-(x/7)*(x/7)+1)||1-(abs(abs(x)-2)-1)*(abs(abs(x)-2)-1)>0&&y>=abs(x/2)-(3*sqrt(33)-7)/112*x*x+sqrt(1-(abs(abs(x)-2)-1)*(abs(abs(x)-2)-1))-3)&&(abs(x)>=3&&-(x/7)*(x/7)+1>0&&y<=3*sqrt(-(x/7)*(x/7)+1)||abs(x)>=0.75&&abs(x)<1&&y<=9-8*abs(x)||abs(x)>=0.5&&abs(x)<0.75&&y<=3*abs(x)+0.75||abs(x)<=0.5&&y<=2.25||abs(x)>=1&&3-x*x+2*abs(x)>0&&y<=1.5-0.5*abs(x)-6*sqrt(10)/14*(sqrt(3-x*x+2*abs(x))-2))
```

## Requirements

### [CMake](https://cmake.org/)

How To Find Libraries: https://gitlab.kitware.com/cmake/community/-/wikis/doc/tutorials/How-To-Find-Libraries

### [GLEW](http://glew.sourceforge.net/) - The OpenGL Extension Wrangler Library

```shell
sudo apt-get install libglew-dev
```

Documentation: http://glew.sourceforge.net/basic.html

### [GLFW](https://www.glfw.org/) - Graphics Library Framework

```shell
sudo apt-get install libglfw3-dev
```

Documentation: https://www.glfw.org/documentation.html