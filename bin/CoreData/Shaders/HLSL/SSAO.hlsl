#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"
#include "ScreenPos.hlsl"

void VS(float4 iPos : POSITION,
    out float2 oScreenPos : TEXCOORD0,
    out float4 oPos : OUTPOSITION)
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
    oScreenPos = GetScreenPosPreDiv(oPos);
}

#ifdef COMPILEPS
// Port from: https://github.com/jsj2008/Zombie-Blobs/blob/278e16229ccb77b2e11d788082b2ccebb9722ace/src/postproc.fs

// see T M?ller, 1999: Efficiently building a matrix to rotate one vector to another
float3x3 rotateNormalVecToAnother(float3 f, float3 t) {
    float3 v = cross(f, t);
    float c = dot(f, t);
    float h = (1.0 - c) / (1.0 - c * c);

    return float3x3(c + h * v.x * v.x, h * v.x * v.y - v.z, h * v.x * v.z + v.y,
                    h * v.x * v.y + v.z, c + h * v.y * v.y, h * v.y * v.z - v.x,
                    h * v.x * v.z - v.y, h * v.y * v.z + v.x, c + h * v.z * v.z);
}

float3 normal_from_depth(float depth, float2 texcoords) {
    float2 offset1 = float2(0.0, 1.0) * cGBufferInvSize;
    float2 offset2 = float2(1.0, 0.0) * cGBufferInvSize;
    
    float depth1 = Sample2D(EmissiveMap, texcoords + offset1).r;
    float depth2 = Sample2D(EmissiveMap, texcoords + offset2).r;
    
    float3 p1 = float3(offset1, depth1 - depth);
    float3 p2 = float3(offset2, depth2 - depth);
    
    float3 normal = cross(p1, p2);
    normal.z = -normal.z;
    
    return normalize(normal);
}

void PS(float2 iScreenPos : TEXCOORD0,
    out float4 oColor : OUTCOLOR0)
{
    const float aoStrength = 1.0;
    
    float2 tx = iScreenPos;
    float2 px = cGBufferInvSize;
    
    float depth = Sample2D(EmissiveMap, iScreenPos).r;
    float3 normal = normal_from_depth(depth, iScreenPos);
    
    // radius is in world space unit
    const float radius = 1.0;
    float zRange = radius / (cFarClipPS - cNearClipPS);
    
    // calculate inverse matrix of the normal by rotate it to identity
    float3x3 InverseNormalMatrix = rotateNormalVecToAnother(normal, float3(0.0, 0.0, 1.0));
    
    // result of line sampling
    // See Loos & Sloan: Volumetric Obscurance
    // http://www.cs.utah.edu/~loos/publications/vo/vo.pdf
    float hemi = 0.0;
    float maxi = 0.0;
    
    for (int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            // make virtual sphere of unit volume, more closer to center, more ambient occlusion contributions
            float rx = 0.3 * float(x);
            float ry = 0.3 * float(y);
            float rz = sqrt(1.0 - rx * rx - ry * ry);
            
            float3 screenCoord = float3(float(x) * px.x, float(y) * px.y, 0.0);
            // 0.25 times smaller when farest, 5.0 times bigger when nearest.
            float2 coord = tx + (5.0 - 4.75 * depth) * screenCoord.xy;
            // fetch depth from texture
            screenCoord.z = Sample2D(EmissiveMap, coord).r;
            // move to origin
            screenCoord.z -= depth;

            // ignore occluders which are too far away
            if (screenCoord.z < -zRange) continue;

            // Transform to normal-oriented hemisphere space
            float3 localCoord = mul(InverseNormalMatrix, screenCoord);
            // ralative depth in the world space radius
            float dr = localCoord.z / zRange;
            // calculate contribution
            float v = clamp(rz + dr * aoStrength, 0.0, 2.0 * rz);

            maxi += rz;
            hemi += v;
        }
    }

    float ao = clamp(hemi / maxi, 0.0, 1.0);
#ifdef AO_ONLY
    oColor = float4(ao, ao, ao, 1.0);
#else
    oColor = float4(Sample2D(DiffMap, iScreenPos).rgb * ao, 1.0);
#endif
}

#endif