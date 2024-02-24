#version 460 core
layout(vertices = 3) out;

//in vec2 textCoord[];

//out vec2 vertexTextCoord[];


layout(std140, binding = 0) uniform camAndProject
{
    mat4 view;
    mat4 projection;
    vec4 viewPosition;
};

uniform float maxDistLod;
uniform float subDivisionLvl;


vec4 scale(vec4 data, float scaleFactor);

void main()
{
    int tesMax = 64;
    int tesMin = 1;

    //Scale patch according to distance of the center of the patch to the camera 
    /*
    vec4 uVec = gl_in[1].gl_Position - gl_in[0].gl_Position;
    vec4 vVec = gl_in[3].gl_Position - gl_in[0].gl_Position;
    vec4 patchCenterPos = uVec / 2 + vVec / 2;
    vec4 patchCenterToCameraVec = vec4(viewPos,0.0)-patchCenterPos;

    //scale
    for (int scaleIndex = 1; scaleIndex <= 8; ++scaleIndex)
    {
        if (length(patchCenterToCameraVec) < (maxDistLod * scaleIndex / 8.0))
        {
            if (scaleIndex == 1)
                break;

            gl_out[gl_InvocationID].gl_Position = scale(gl_in[gl_InvocationID].gl_Position,scaleIndex);
            break;
        }
    }
    */

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    //vertexTextCoord[gl_InvocationID] = textCoord[gl_InvocationID];

    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = 16;
        gl_TessLevelOuter[1] = 16;
        gl_TessLevelOuter[2] = 16;
        gl_TessLevelOuter[3] = 16;
                               
        gl_TessLevelInner[0] = 16;
        gl_TessLevelInner[1] = 16;
    }

    //LOD ALGORITHM REALLY WORKS BADLY NEED TO BE FIXED
    /*
    //handle LOD
    //get edge distance (get distance from camera of a vertex and the other vertex on the edge and averange both, repeate for the 4 outer)

    //average position for each outerEdge

    vec4 outer0Pos = (gl_out[0].gl_Position + gl_out[3].gl_Position) / 2;
    vec4 outer1Pos = (gl_out[0].gl_Position + gl_out[1].gl_Position) / 2;
    vec4 outer2Pos = (gl_out[1].gl_Position + gl_out[2].gl_Position) / 2;
    vec4 outer3Pos = (gl_out[2].gl_Position + gl_out[3].gl_Position) / 2;

    vec3 CameraToEdgeVec;

    //set TessLevel for outerEdge and change size accordingly
    for (int lodIndex = 1; lodIndex <= 8; ++lodIndex)
    {
        CameraToEdgeVec = outer0Pos.xyz - viewPos;
            if ((length(CameraToEdgeVec) < (maxDistLod * lodIndex / 8.0)) && gl_TessLevelOuter[0] == 1) //if tess level is not equal to 1 it means it already has been set to the right value
                gl_TessLevelOuter[0] = tesMax / lodIndex;


        CameraToEdgeVec = viewPos - outer1Pos.xyz;
        if ((length(CameraToEdgeVec) < (maxDistLod * lodIndex / 8.0)) && gl_TessLevelOuter[1] == 1)
            gl_TessLevelOuter[1] = tesMax / lodIndex;


        CameraToEdgeVec = viewPos - outer2Pos.xyz;
        if ((length(CameraToEdgeVec) < (maxDistLod * lodIndex / 8.0)) && gl_TessLevelOuter[2] == 1)
            gl_TessLevelOuter[2] = tesMax / lodIndex;


        CameraToEdgeVec = viewPos - outer3Pos.xyz;
        if ((length(CameraToEdgeVec) < (maxDistLod * lodIndex / 8.0)) && gl_TessLevelOuter[3] == 1)
            gl_TessLevelOuter[3] = tesMax / lodIndex;
    }

    //set TessLevel for InnerEdge (it's the average of the four outer)
    float outerAverage = (gl_TessLevelOuter[0] + gl_TessLevelOuter[1] + gl_TessLevelOuter[2] + gl_TessLevelOuter[3]) / 4;
    gl_TessLevelInner[0] = outerAverage;
    gl_TessLevelInner[1] = outerAverage;

    */
}

vec4 scale(vec4 data, float scaleFactor)
{
    mat4 scaleMat;

    scaleMat[0] = vec4(scaleFactor, 0.0, 0.0, 0.0);   //col1
    scaleMat[1] = vec4(0.0, scaleFactor, 0.0, 0.0);  //col2
    scaleMat[2] = vec4(0.0, 0.0, scaleFactor, 0.0);  //col3
    scaleMat[3] = vec4(0.0, 0.0, 0.0, 1.0);          //col4

    return scaleMat * data;

}