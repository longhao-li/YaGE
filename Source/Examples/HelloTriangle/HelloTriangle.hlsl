struct VertexInput {
    float4 position : POSITION;
    float4 color    : COLOR;
};

struct VertexOutput {
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

typedef VertexOutput PixelInput;

VertexOutput VertexMain(VertexInput vert) {
    VertexOutput output;

    output.position = vert.position;
    output.color    = vert.color;

    return output;
}

float4 PixelMain(PixelInput pixel) : SV_TARGET {
    return pixel.color;
}
