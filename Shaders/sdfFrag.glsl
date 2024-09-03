#version 330 core


in vec2 UV;

uniform float time;

out vec4 fragmentColour;


float sdfCircle(vec2 position, float radius){
    return length(position) - radius;
}


void main(){
    //fragmentColour = vec4(1.0f);


    vec3 black = vec3(0.0f);
    vec3 white = vec3(1.0f);
    vec3 red = vec3(1.0f, 0.0f, 0.0f);
    vec3 blue = vec3(0.0f, 0.0f, 1.0f);
    vec3 orange = vec3(1.0f, 0.3f, 0.00f);

    float aspect = 1280.0/720.0;
    vec2 uv = 2.0f*(UV - 0.5f);
    uv = vec2(aspect*uv.x, uv.y);
    vec3 colour = vec3(uv.x, uv.y, 0.0);


    // SDF of a circle
    float radius = 0.5f;
    vec2 centre = vec2(0.0f);
    float sdf = sdfCircle(uv - centre, radius);
    colour = sdf > 0.0f ? orange : blue;


    // makes things outside the circle bright, and inside becomes dark
    //colour = colour * exp(10.0f*sdf);
    //colour = colour * exp(abs(10.0f*sdf));
    colour = colour * (1.0f - exp(-2.0f * abs(1.0f*sdf)) );

    // waves
    float k = 150.0f;
    float w = 10.0f;
    colour = 0.8f*colour + 0.2f*sin(k*sdf - w*time);

    // add a white border to the circle
    colour = mix(white, colour, smoothstep(0.0, 0.05, abs(2.0f*sdf)));


    // moar blending
    colour = mix(white, colour, 5.0f*abs(sdf));

    fragmentColour = vec4(colour, 1.0f);


}