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


const GLchar * linear_vert_fs = R"(
                                #version 330 core
                                in vec2 TexCoords;
                                uniform sampler2D image;
                                uniform vec2 resolution;
                                out vec4 FragmentColor;
                                uniform float offset[3];
                                uniform float weight[3];
                                void main(void)
                                {
                                    FragmentColor = texture2D( image, TexCoords) *  weight[0];
                                    for (int i=1; i<5; i++) {
                                        FragmentColor += texture2D( image, TexCoords + vec2(0.0, offset[i])/resolution.y) * weight[i];
                                        FragmentColor += texture2D( image, TexCoords - vec2(0.0, offset[i])/resolution.y) * weight[i];
                                        //FragmentColor += texture2D( image, TexCoords - vec2(0.0, offset[i]/resolution.y)) * weight[i];
                                        //FragmentColor += texture2D( image, TexCoords + vec2(0.0, offset[i]/resolution.y)) * weight[i];
                                    }
                                }

            )";

const GLchar * linear_horiz_fs = R"(
                                 #version 330 core
                                 in vec2 TexCoords;
                                 uniform sampler2D image;
                                 uniform vec2 resolution;
                                 out vec4 FragmentColor;
                                 uniform float offset[3];
                                 uniform float weight[3];
                                 void main(void)
                                 {
                                     FragmentColor = texture2D( image, TexCoords) * weight[0];
                                     for (int i=1; i<3; i++) {
                                        FragmentColor += texture2D( image, TexCoords + vec2(offset[i], 0.0)/resolution.x ) * weight[i];
                                        FragmentColor += texture2D( image, TexCoords - vec2(offset[i], 0.0)/resolution.x ) * weight[i];
                                     }
                                 }


            )";






