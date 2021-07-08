#version 330

uniform sampler2D tex;

out vec4 pixelColor;

in vec4 iColor;
in vec4 l;
in vec4 n;
in vec4 v;
in vec2 iTC;

void main(void) {
	vec4 ml = normalize(l);
    vec4 mn = normalize(n);
    vec4 mv = normalize(v);
    vec4 mr = reflect(-ml, mn);

    vec4 kd = texture(tex, iTC);
    vec4 ks = vec4(1,1,1,1);

    float nl = clamp(dot(mn, ml), 0, 1);
    float rv = pow(clamp(dot(mr, mv), 0, 1), 50);

    pixelColor = kd;
}
