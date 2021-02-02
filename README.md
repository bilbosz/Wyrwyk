# Wyrwyk - Implicit Curve Rasterizer

## Examples
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
Heart:

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