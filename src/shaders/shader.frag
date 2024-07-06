uniform sampler2D texture;
uniform float solidBlockVisibility;


void main()
{
	vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

	float addRed = gl_Color.r * solidBlockVisibility;
	pixel.r = mix(pixel.r, 0.65, addRed);

	gl_FragColor = pixel;
}