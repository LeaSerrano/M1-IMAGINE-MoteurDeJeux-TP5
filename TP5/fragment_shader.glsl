#version 330 core

// Ouput data
out vec4 color;

in vec2 o_uv0;

uniform sampler2D grassTexture;
uniform sampler2D rockTexture;
uniform sampler2D snowrocksTexture;
uniform sampler2D sunTexture;
uniform sampler2D earthTexture;
uniform sampler2D moonTexture;

in vec4 height;

uniform bool isTexturedForFragment;
uniform bool isSolarSystemSceneFragment;

uniform int planet;

uniform vec3 objectColor;

void main(){

        if (isTexturedForFragment && !isSolarSystemSceneFragment) {
                if (height.r > 0.8) {
                        color = texture(grassTexture, o_uv0);
                }
                else if (height.r < 0.6) {
                        color = texture(snowrocksTexture, o_uv0);
                }
                else {
                        color = texture(rockTexture, o_uv0);
                }
                //color = vec4(o_uv0, 0, 1);
                //color = texture(sunTexture, o_uv0);
        }
        else if (isTexturedForFragment && isSolarSystemSceneFragment) {
                if (planet == 0) {
                        color = texture(sunTexture, o_uv0);
                }
                else if (planet == 1) {
                        color = texture(earthTexture, o_uv0);
                }
                else if (planet == 2) {
                        color = texture(moonTexture, o_uv0);
                }
        }
        else {
                color = vec4(objectColor, 1);
        }

}