#include "Header.hlsli"

groupshared MeshletIndices meshletIndices;

[numthreads(THREADS_PER_GROUP, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    bool isVisible = false;
    
    if (dispatchThreadID.x < meshletNumber)
        isVisible = true;
    
    if (isVisible)
        meshletIndices.indices[dispatchThreadID.x] = dispatchThreadID.x;
    
    uint currentMeshetNumber = WaveActiveCountBits(isVisible);
    
    DispatchMesh(currentMeshetNumber, 1, 1, meshletIndices);
}