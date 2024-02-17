#include "Header.hlsli"

StructuredBuffer<Vertex> vertexBuffer : register(t0);
Buffer<uint> uniqueIndexBuffer : register(t1);
Buffer<uint> triangleBuffer : register(t2);
StructuredBuffer<Meshlet> meshletBuffer : register(t3);

[NumThreads(THREADS_PER_GROUP, 1, 1)]
[OutputTopology("triangle")]
void main(uint3 groupThreadId : SV_GroupThreadID, uint3 groupId : SV_GroupID, in payload MeshletIndices meshletIndices,
    out vertices VertexOutput outputVertices[MAX_VERTICES_PER_MESHLET], out indices uint3 outputIndices[MAX_INDICES_PER_MESHLET])
{
    uint meshletIndex = meshletIndices.indices[groupId.x];
    
    if (meshletIndex >= meshletNumber)
        return;
    
    Meshlet meshlet = meshletBuffer[meshletIndex];
    
    SetMeshOutputCounts(meshlet.vertexNumber, meshlet.triangleNumber);
    
    uint vertexIndex = groupThreadId.x;
    
    if (vertexIndex < meshlet.vertexNumber)
    {
        uint absoluteVertexIndex = uniqueIndexBuffer[meshlet.vertexStartIndex + vertexIndex];
        VertexInput inputVertex = vertexBuffer[absoluteVertexIndex];
        
        VertexOutput vertex = (VertexOutput)0;
        vertex.position = mul(worldViewProj, float4(inputVertex.position.xyz, 1.0f));
        
        float3 colorFromUint = uint3((meshletIndex >> 4) & 3, (meshletIndex >> 2) & 3, meshletIndex & 3) / 3.0f;
        
        vertex.color = float4(colorFromUint, 1.0f);
        outputVertices[vertexIndex] = vertex;
    }

    uint triangleIndex = groupThreadId.x;
    
    if (triangleIndex < meshlet.triangleNumber)
    {
        uint packedIndex = triangleBuffer[meshlet.triangleStartIndex + triangleIndex];
        outputIndices[triangleIndex] = uint3(packedIndex & 0x3ff, (packedIndex >> 10) & 0x3ff, (packedIndex >> 20) & 0x3ff);
    }
}