#version 330 core

out vec4 FragColor;

uniform vec4 shapeColor;
uniform vec2 playerPos; // User's position
uniform bool applyEffect;

void main()
{

    // Hardcoded resolution
    vec2 resolution = vec2(2000.0, 2000.0);

    // Normalize fragment coordinates (convert to range from 0 to 1)
    vec2 fragCoord = gl_FragCoord.xy;
   // vec2  newPlayerPos = playerPos/resolution;
    // Calculate the distance from the current fragment to the user's position
    float distance = distance(fragCoord, playerPos);

    // Define the radius of the circle
    float radius = 350.00; // Adjust this value for a larger or smaller circle

    // Calculate alpha based on distance (more opaque as the distance increases)
    float alpha = clamp(distance / radius, 0.0, 1.0);

    // Define a color for the circle and another for the outside
    vec4 circleColor = vec4(1.0, 1.0, 1.0, 1.0 - alpha); // Less transparent with increasing distance
    vec4 outsideColor = vec4(1.0, 1.0, 1.0, 1.0); // Fully opaque

    // Check if the current fragment is inside the circle

    if (distance < radius)
    {
        // Inside the circle: calculate alpha based on distance
        float alpha = 1.0 - (distance / radius);
        FragColor = vec4(shapeColor.rgb, alpha);
        // Inside the circle: multiply shapeColor by the circle's color
    }
    else
    {
        // Outside the circle: multiply shapeColor by the outside color
        FragColor = shapeColor * circleColor;
    }
}