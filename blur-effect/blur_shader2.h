#include "../common.h"

//shader
const char *vs = R"(
                       #version 330 core
                       layout (location = 0) in vec2 aPos;
                       layout (location = 1) in vec2 aTexCoords;
                       out vec2 TexCoords;
                       void main()
                       {
                           TexCoords = aTexCoords;
                           gl_Position = vec4(aPos, 0.0,1.0);
                       }
                       )";


const GLchar * fs = R"(
            #version 330 core
            out vec4 FragColor;
            in vec2 TexCoords;
            // texture samplers
            uniform sampler2D texture1;
            void main()
            {
                vec2 tc = vec2(TexCoords.s, 1.0 - TexCoords.t);
                FragColor =  texture(texture1, tc);
            }
            )";


const GLchar * linear_filter_fs = R"(
                                #version 330 core
                                in vec2 TexCoords;
                                uniform sampler2D image;
                                uniform sampler2D image2;
                                uniform vec2 resolution;
                                uniform int filterMode;//0 -- vert ,1 -- linear
                                out vec4 FragmentColor;
                                vec4 fGaussianPass(vec2 tc)
                                {
                                    vec2 verticalDir = vec2(0.0, 1.0/resolution.y);
                                    vec2 horizontalDir = vec2(1., 0.0/resolution.x);
                                    //float Gauss[9] = float[](  0.93, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1 );
                                    //float WT_NORMALIZE= (1.0/(1.0+2.0*(0.93 + 0.8 + 0.7 + 0.6 + 0.5 + 0.4 + 0.3 + 0.2 + 0.1)));
                                    float Gauss[9] = float[](  0.93, 0.0, 0.7, 0.0, 0.5, 0.0, 0.3, 0.2, 0.1 );
                                    float WT_NORMALIZE= (1.0/(1.0+2.0*(0.93 + 0.0 + 0.7 + 0.0 + 0.5 + 0.0 + 0.3 + 0.2 + 0.1)));
                                    vec4 c2;
                                    vec4 c = texture2D(image, tc) * (WT_NORMALIZE);
                                    vec2 step = horizontalDir;//水平过滤
                                    if (filterMode == 1)
                                    {
                                        step = verticalDir;//垂直过滤
                                    }
                                    vec2 dir = step;
                                    for (int i = 0; i < 9; i++)
                                    {
                                        c2 = texture2D(image, tc + dir);
                                        c += c2 * (Gauss[i] * WT_NORMALIZE);
                                        c2 = texture2D(image, tc - dir);
                                        c += c2 * (Gauss[i] * WT_NORMALIZE);
                                        dir += step;
                                    }
                                    if (filterMode == 1)
                                    {
                                       float glowFactor = 1;
                                       c *= glowFactor;
                                    }
                                    return c;
                                }
                                vec4 fFinalCompositing(vec2 tc)
                                {
                                    int glowMode = 1;
                                    vec4 c = texture2D(image, tc);
                                    vec4 sc = texture2D(image2 , tc);
                                    if(glowMode == 1){             //外发光
                                        c = 1.0-(1.0-c)*(1.0-sc);     //滤色混合（屏幕）
                                        c-=sc;                  //去掉中间
                                      }else{                    //内发光
                                        c*=sc;                  //正片叠底删除多余像素
                                      }
                                      return c;
                                 }
                                void main(void)
                                {
                                    vec4 outcolor;
                                    if (filterMode < 2)
                                    {
                                         outcolor = fGaussianPass(TexCoords);
                                    }
                                    else
                                    {
                                         outcolor = fFinalCompositing(TexCoords);
                                    }
                                    FragmentColor = outcolor;
                                }

            )";




