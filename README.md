# Wyrwyk - Implicit Curve Rasterizer

## Examples
```
sin(a)*x == -cos(a)*y
sin( x * x + y * y ) + y > a
cos(a)*x - sin(a)*y == (sin(a)*x + cos(a)*y) * (sin(a)*x + cos(a)*y)
sin( t )
sqrt(x*x + y*y) == a
```

## Requirements

### [CMake](https://cmake.org/)

How To Find Libraries: https://gitlab.kitware.com/cmake/community/-/wikis/doc/tutorials/How-To-Find-Libraries

### [GLEW](http://glew.sourceforge.net/) - Graphics Library Framework

```shell
sudo apt-get install libglew-dev
```

Documentation: http://glew.sourceforge.net/basic.html

### [GLFW](https://www.glfw.org/) - The OpenGL Extension Wrangler Library

```shell
sudo apt-get install libglfw3-dev
```

Documentation: https://www.glfw.org/documentation.html