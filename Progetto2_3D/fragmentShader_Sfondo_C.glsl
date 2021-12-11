#version 330 core
#ifdef GL_ES
precision mediump float;
#endif

#ifdef GL_ES
precision mediump float;
#endif
uniform float timeS;
uniform vec2 mouse;
uniform vec2 resolution;

void main( void ) {
	vec2 p = (gl_FragCoord.xy - 0.5 * resolution) / min(resolution.x, resolution.y);
	vec3 c = vec3(0.05, 0.01, 0.01); // BG color
	vec3 lightCol = vec3(0.8, 0.8, 0.5);
    
	for(int i = 0; i < 9; ++i) {
        	float t = 1. * 3.14 * float(i) / 4. * fract(timeS * 0.1);
        	float x = cos(t);
        	float y = sin(t);
        	vec2 o = .5 * vec2(x, y);
        	c += 0.02 / (length(p - o)) * lightCol;
	}
	
	gl_FragColor = vec4(c,1);
}