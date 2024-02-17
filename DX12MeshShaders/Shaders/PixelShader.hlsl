#include "Header.hlsli"

PixelOutput main(PixelInput input)
{
    PixelOutput output = (PixelOutput) 0;
    
    output.color = input.color;
    
    return output;
}