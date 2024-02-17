static const int THREADS_PER_GROUP = 64;
static const int MAX_VERTICES_PER_MESHLET = 64;
static const int MAX_INDICES_PER_MESHLET = 84;

cbuffer MeshConstants : register(b0)
{
    float4x4 worldViewProj;
    uint meshletNumber;
};

struct VertexInput
{
    float4 position : POSITION;
};

struct VertexOutput
{
    float4 position : SV_Position;
    float4 color : COLOR0;
};

struct PixelInput
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

struct PixelOutput
{
    float4 color : SV_Target0;
};

struct Vertex
{
    float4 position;
};

struct Meshlet
{
    uint vertexNumber;
    uint vertexStartIndex;
    uint triangleNumber;
    uint triangleStartIndex;
};

struct MeshletIndices
{
    uint indices[THREADS_PER_GROUP];
};
