### 采样器的序号要与激活的纹理序号对应:
GL_TEXTURE0 : 0
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, tex);
    
glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

### framebuffers
到目前为止，我们使用了几种不同类型的屏幕缓冲：用于写入颜色值的颜色缓冲，用于写入深度信息的深度缓冲，以及允许我们基于一些条件丢弃指定片段的模板缓冲。把这几种缓冲结合起来叫做帧缓冲(Framebuffer)，它被储存于内存中。OpenGL给了我们自己定义帧缓冲的自由，我们可以选择性的定义自己的颜色缓冲、深度和模板缓冲。
我们目前所做的渲染操作都是是在默认的帧缓冲之上进行的。当你创建了你的窗口的时候默认帧缓冲就被创建和配置好了（GLFW为我们做了这件事）。通过创建我们自己的帧缓冲我们能够获得一种额外的渲染方式。
#### RenderTarget
在3D计算机图形领域，渲染目标是现代图形处理单元（GPU）的一个特征，它允许将3D场景渲染到中间存储缓冲区或渲染目标纹理（RTT），而不是帧缓冲区或后缓冲区。然后可以通过像素着色器操纵此RTT ，以便在显示最终图像之前将其他效果应用于最终图像.
有以下两种作用：
1：首当其冲的就是离屏渲染。我们可以将需要渲染的场景作为纹理帖到其他地方。
2：在多PASS的时候会用到，将两个不同PASS的效果渲染到不同的RenderTarget（渲染缓冲区）上，然后在进行ADD送入帧缓冲区。
#### off-screen rendering
GLuint fbo;
glGenFramebuffers(1, &fbo);
// 绑定到GL_FRAMEBUFFER目标后，接下来所有的读、写帧缓冲的操作都会影响到当前绑定的帧缓冲。
glBindFramebuffer(GL_FRAMEBUFFER, fbo);

后续所有渲染操作将渲染到当前绑定的帧缓冲的附加缓冲中，由于我们的帧缓冲不是默认的帧缓冲，渲染命令对窗口的视频输出不会产生任何影响。出于这个原因，它被称为离屏渲染（off-screen rendering），就是渲染到一个另外的缓冲中.

为了让所有的渲染操作对主窗口产生影响我们必须通过绑定为0来使默认帧缓冲被激活：
glBindFramebuffer(GL_FRAMEBUFFER, 0);