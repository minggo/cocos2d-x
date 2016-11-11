const char* cc3D_ETC1ASColorTex_frag = R"(

#ifdef GL_ES
varying mediump vec2 TextureCoordOut;
#else
varying vec2 TextureCoordOut;
#endif
uniform vec4 u_color;

void main(void)
{
vec4 texColor = vec4(texture2D(CC_Texture0, TextureCoordOut).rgb, texture2D(CC_Texture1, TextureCoordOut).r);

    texColor.rgb *= texColor.a; // Premultiply with Alpha channel

    gl_FragColor = u_color * texColor;
}
)";
