# 1、sdf 用圆切去实现圆角

缺点，圆弧会过大

## circle-cut.vert

eye: 摄像机的位置

aPos:顶点坐标

输出verts变量为eye - aPos ，并转换到***视（摄像机）坐标系***下。

## circle-cut.frag

**sdfCircle函数：**

* 参数uv：是视（摄像机）坐标系下的顶点坐标
* 功能：通过SDF（有符号距离场），直接求uv到原点（eye)的距离就可以判断像素点是否在圆类

**main函数：**

将视坐标系下的顶点坐标和圆的半径传过去求值即可。

## 参考文档

https://www.cnblogs.com/constantince/p/15430286.html

# 2、

# 3、stencil技术

# 附

1）https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/08%20Advanced%20GLSL/#glsl
