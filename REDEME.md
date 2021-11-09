requirements:

+ libglm-dev
+ libglfw3-dev
+ gld
+ libgdk-pixbuf2.0-dev
+ libegl1-mesa-dev
+ libgles2-mesa-dev

blur-effect base on [idea](http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/)

# main.cpp

无屏幕创建渲染上下文的流程：

需要使用gbm + egl共同来创建渲染上下文

1.open_drm_device 拿到drm的设备节点fd。

2.gbm_create_device(fd)  创建gbm_device

3.再通过gbm_surface_create(gbm_device)   创建gbm_surface.

4.eglGetDisplay(gbm_device)  拿到egl_display

5.eglInitialize   初始化

6.eglBindAPI(EGL_OPENGL_ES_API)   绑定gles

7.eglCreateContext(egl_display)   

8.eglCreateWindowSurface  创建EGLSurface

9.eglMakeCurrent

