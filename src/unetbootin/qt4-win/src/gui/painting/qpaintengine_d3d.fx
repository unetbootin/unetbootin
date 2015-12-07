bool g_mCosmeticPen;
int4 g_mChannel;
float2 g_mMaskOffset;
int2 g_mMaskSize;
float4x4 g_mMaskProjection;
float4x4 g_mViewProjection;
float4x4 g_mTransformation;
texture g_mAAMask;
texture g_mTexture;
int g_mBrushMode;
float g_mFocalDist;

#define M_PI 3.14159265358979323846

sampler PixmapSampler = sampler_state
{
    texture = <g_mTexture>;
    MIPFILTER = NONE;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

sampler TextSampler = sampler_state
{
    texture = <g_mTexture>;
    MIPFILTER = NONE;
    MINFILTER = POINT;
    MAGFILTER = POINT;
};

sampler AAMaskSampler = sampler_state
{
    texture = <g_mAAMask>;
    AddressU  = WRAP;
    AddressV  = WRAP;
    AddressW  = WRAP;
    MIPFILTER = NONE;
    MINFILTER = POINT;
    MAGFILTER = POINT;
};

struct VS_FULL
{
    float4 Position : POSITION;
    float4 Diffuse : COLOR0;
    float4 TexCoords0 : TEXCOORD0;
    float4 TexCoords1 : TEXCOORD1;
};

VS_FULL TrapezoidVS( float4 Position : POSITION,
                                float4 Diffuse : COLOR0,
                                float4 TexCoords0 : TEXCOORD0,
                                float4 TexCoords1 : TEXCOORD1)
{
    VS_FULL Output;

    float a = (TexCoords1.x * Position.x) + (TexCoords1.z * (1.0 - Position.x) ); // left or right a
    float b = (TexCoords1.y * Position.x) + (TexCoords1.w * (1.0 - Position.x) ); // left or right b
    float d = 1.0 - (Position.x * 2);

    Position.x = (a * Position.y + b) + ( sqrt( abs(a * a) ) * d );
    //Position.x += step(abs(a), 0) * d;
    Position.x += (0.5 * d);

    Output.Position = mul(Position, g_mMaskProjection);
    Output.Diffuse = Diffuse;
    Output.TexCoords0 = TexCoords0;
    Output.TexCoords1 = TexCoords1;

    return Output;
}

struct PS_OUTPUT
{
    float4 Color : COLOR0;
};

PS_OUTPUT TrapezoidPS(VS_FULL In, float2 pixelPos : VPOS)
{
    PS_OUTPUT Out;

    float top = max(pixelPos.y - 0.5, In.TexCoords0.x);
    float bottom = min(pixelPos.y + 0.5, In.TexCoords0.y);

    float area = bottom - top;

    float left = pixelPos.x - 0.5;
    float right = pixelPos.x + 0.5;

    // use line equations to compute intersections of left/right edges with top/bottom of truncated pixel
    // vecX: x = (left, top), y = (left, bottom), z = (right, top), w = (right, bottom)
    float4 vecX = In.TexCoords1.xxzz * float2(top, bottom).xyxy + In.TexCoords1.yyww;

    float2 invA = In.TexCoords0.zw;

    // transform right line to left to be able to use same calculations for both
    vecX.zw = 2 * pixelPos.x - vecX.zw;

    float2 topX = float2(vecX.x, vecX.z);
    float2 bottomX = float2(vecX.y, vecX.w);

    // transform lines such that top intersection is to the right of bottom intersection
    float2 topXTemp = max(topX, bottomX);
    float2 bottomXTemp = min(topX, bottomX);

    // make sure line slope reflects mirrored lines
    invA = lerp(invA, -invA, step(topX, bottomX));

    float2 vecLeftRight = float2(left, right);

    // compute the intersections of the lines with the left and right edges of the pixel
    // intersectY: x = (left_line, left), y = (left_line, right), z = (right_line, left), w = (right_line, right)
    float4 intersectY = top + (vecLeftRight.xyxy - topXTemp.xxyy) * invA.xxyy;

    float2 temp = lerp(area - 0.5 * (right - bottomXTemp) * (bottom - intersectY.yw), // left < bottom < right < top
                    (0.5 * (topXTemp + bottomXTemp) - left) * area,    // left < bottom < top < right
                    step(topXTemp, right));

    float2 excluded = 0.5 * (intersectY.xz - top) * (topXTemp - left); // bottom < left < top < right

    excluded = lerp(0.5 * (intersectY.yw + intersectY.xz) - top, // bottom < left < right < top
                   excluded, step(topXTemp, right));

    excluded = lerp(temp, // left < bottom < right (see calculation of temp)
                   excluded, step(bottomXTemp, left));

    excluded = lerp(float2(area, area), // right < bottom < top
                   excluded, step(bottomXTemp, right));

    excluded *= step(left, topXTemp);

    float result = (area - excluded.x - excluded.y) * step(top, bottom);
    Out.Color.r = result * g_mChannel[0];
    Out.Color.g = result * g_mChannel[1];
    Out.Color.b = result * g_mChannel[2];
    Out.Color.a = result * g_mChannel[3];

    return Out;
}

VS_FULL ViewProjectionVS( float4 Position : POSITION,
                          float4 Diffuse : COLOR0,
                          float4 TexCoords0 : TEXCOORD0,
                          float4 TexCoords1 : TEXCOORD1)
{
    VS_FULL Output;

    Output.Position = mul(Position, g_mTransformation);
    Output.Position = mul(Output.Position, g_mViewProjection);
    Output.Diffuse = Diffuse;
    Output.TexCoords0 = TexCoords0;
    Output.TexCoords1 = TexCoords1;

    return Output;
}

PS_OUTPUT DirectMaskPS(VS_FULL In, float2 pixelPos : VPOS)
{
    PS_OUTPUT Out;
    Out.Color = In.Diffuse;

    float2 maskcoords = ( (pixelPos + g_mMaskOffset) - 0.5 ) / g_mMaskSize;
    float2 clipcoords = (pixelPos - 0.5) / g_mMaskSize;

    float4 c = tex2D(AAMaskSampler, maskcoords.xy) * Out.Color.a;
    Out.Color.a  = c.r * g_mChannel[0];
    Out.Color.a += c.g * g_mChannel[1];
    Out.Color.a += c.b * g_mChannel[2];
    Out.Color.a += c.a * g_mChannel[3];

    return Out;
}

PS_OUTPUT MaskPS(VS_FULL In, float2 pixelPos : VPOS)
{
    PS_OUTPUT Out;

    if (g_mBrushMode == 1) {
        float x = In.TexCoords0.x;
        float y = In.TexCoords0.y;
        x = x - int(x);
        y = y - int(y);
        Out.Color = tex2D(PixmapSampler, float2(x, y));
        Out.Color.a = Out.Color.a * In.Diffuse.a;
    } else if (g_mBrushMode == 2) {
        Out.Color = tex1D(PixmapSampler, In.TexCoords0.x);
    } else if (g_mBrushMode == 3) {
        float t = atan2(In.TexCoords0.y, -In.TexCoords0.x) / (2 * M_PI);
        Out.Color = tex1D(PixmapSampler, t + 0.5);
    } else if (g_mBrushMode == 4) {
        float2 tc = float2(In.TexCoords0.x, abs(In.TexCoords0.y));
        float a = (tc.x - g_mFocalDist) / tc.y;
        float b = g_mFocalDist;

        float A = 1 + (a * a);
        float B = 2.0 * a * b;
        float C = (b * b) - 1;

        float y = (-B + sqrt(B*B - 4.0*A*C)) / (2.0*A);
        Out.Color = tex1D(PixmapSampler, (tc.y / y) );
    } else if (g_mBrushMode == 5) {
        Out.Color = tex2D(PixmapSampler, In.TexCoords0.xy);
        Out.Color = Out.Color * In.Diffuse;
    } else {
        Out.Color = In.Diffuse;
    }

    float2 maskcoords = ( (pixelPos + g_mMaskOffset) - 0.5 ) / g_mMaskSize;

    float4 c = tex2D(AAMaskSampler, maskcoords.xy) * Out.Color.a;
    Out.Color.a  = c.r * g_mChannel[0];
    Out.Color.a += c.g * g_mChannel[1];
    Out.Color.a += c.b * g_mChannel[2];
    Out.Color.a += c.a * g_mChannel[3];

    return Out;
}

struct VS_NORMAL
{
    float4 Position : POSITION;
    float4 Diffuse : COLOR0;
    float4 TexCoords : TEXCOORD0;
};

VS_NORMAL MaskProjectionVS(VS_NORMAL In)
{
    VS_NORMAL Output;

    Output.Position = mul(In.Position, g_mMaskProjection);
    Output.Diffuse = In.Diffuse;
    Output.TexCoords = In.TexCoords;

    return Output;
}

float4 DirectSimplePS(float4 Color : COLOR0) : COLOR0
{
    return Color;
}

float4 SimplePS(float4 Color : COLOR0, float4 TexCoords : TEXCOORD0) : COLOR0
{
    if (g_mBrushMode == 1) {
        float opacity = Color.a;
        float x = TexCoords.x;
        float y = TexCoords.y;
        x = x - int(x);
        y = y - int(y);
        Color = tex2D(PixmapSampler, float2(x, y));
        Color.a = Color.a * opacity;
    } else if (g_mBrushMode == 2) {
        Color = tex1D(PixmapSampler, TexCoords.x);
    } else if (g_mBrushMode == 3) {
        float t = atan2(TexCoords.y, -TexCoords.x) / (2 * M_PI);
        Color = tex1D(PixmapSampler, t + 0.5);
    } else if (g_mBrushMode == 4) {
        float2 tc = float2(TexCoords.x, abs(TexCoords.y));
        float a = (tc.x - g_mFocalDist) / tc.y;
        float b = g_mFocalDist;

        float A = 1 + (a * a);
        float B = 2.0 * a * b;
        float C = (b * b) - 1;

        float y = (-B + sqrt(B*B - 4.0*A*C)) / (2.0*A);
        Color = tex1D(PixmapSampler, (tc.y / y) );
    } else if (g_mBrushMode == 5) {
        Color = tex2D(PixmapSampler, TexCoords.xy) * Color;
    }

    return Color;
}

float4 TextPS(float4 Color : COLOR0, float4 TexCoords : TEXCOORD0) : COLOR0
{
    Color.a *= tex2D(TextSampler, TexCoords.xy).a;
    return Color;
}

float4 ClearTypePS(float4 Color : COLOR0, float4 TexCoords : TEXCOORD0) : COLOR0
{
//     if (g_mUsePixmap) {
//     float4 MaskColor = tex2D(PixmapSampler, TexCoords.xy);
//     Color = float4(1.0, 0.0, 0.0, 1.0);
//     Color.a = (1 - MaskColor.a) + MaskColor.a * Color.a;
//     Color.r = (1.0 - MaskColor.r) + (MaskColor.r * Color.r);
//     Color.g = (1.0 - MaskColor.g) + (MaskColor.g * Color.g);
//     Color.b = (1.0 - MaskColor.b) + (MaskColor.b * Color.b);
//     Color = MaskColor;
    return tex2D(PixmapSampler, TexCoords.xy);
}

VS_NORMAL NoTxAliasedVS(VS_NORMAL In)
{
    VS_NORMAL Output;

    Output.Position = mul(In.Position, g_mViewProjection);
    Output.Diffuse = In.Diffuse;
    Output.TexCoords = In.TexCoords;

    return Output;
}

VS_NORMAL AliasedVS(VS_NORMAL In)
{
    VS_NORMAL Output;

    Output.Position = mul(In.Position, g_mTransformation);
    Output.Position = mul(Output.Position, g_mViewProjection);
    Output.Diffuse = In.Diffuse;
    Output.TexCoords = In.TexCoords;

    return Output;
}

VS_NORMAL AliasedLinesVS(VS_NORMAL In)
{
    VS_NORMAL Output;

	float4 start = float4(In.Position.x, In.Position.y, 0.5, In.Position.w);
	float4 end = float4(In.TexCoords.z, In.TexCoords.w, 0.5, In.Position.w);
	if (g_mCosmeticPen) {
		start = mul(start, g_mTransformation);
		end =  mul(end, g_mTransformation);
	}

    float2 line_vec = end - start;
    float2 vec = normalize(line_vec);
    float2 norm = float2(-vec.y, vec.x);

    float pen_width = In.Position.z;
    norm = norm * pen_width * 0.5;
    vec = vec * pen_width * 0.5;

    Output.Position.w = In.Position.w;
    Output.Position.x = start.x + (vec.x * In.TexCoords.x);
    Output.Position.x = Output.Position.x + (norm.x * In.TexCoords.y);
    Output.Position.x = Output.Position.x + (line_vec.x * step(0, In.TexCoords.x));
    Output.Position.y = start.y + (vec.y * In.TexCoords.x);
    Output.Position.y = Output.Position.y + (norm.y * In.TexCoords.y);
    Output.Position.y = Output.Position.y + (line_vec.y * step(0, In.TexCoords.x));
    Output.Position.z = 0.5;

    if (!g_mCosmeticPen) {
	    Output.Position = mul(Output.Position, g_mTransformation);
	}
    Output.Position = mul(Output.Position, g_mViewProjection);

    Output.Diffuse = In.Diffuse;
    Output.TexCoords = In.TexCoords;

    return Output;
}


technique Antialiased
{
    pass PASS_AA_CREATEMASK
    {
        StencilEnable = False;
        ZWriteEnable = False;
        ColorWriteEnable = 0x0f;
        ZEnable = False;

        SrcBlend = One;
        DestBlend = One;

        VertexShader = compile vs_3_0 TrapezoidVS();
        PixelShader = compile ps_3_0 TrapezoidPS();
    }

    pass PASS_AA_DRAW
    {
        StencilEnable = False;
        ZFunc = Greater;
        ZWriteEnable = False;
        ZEnable = True;
        ColorWriteEnable = 0x0f;

        VertexShader = compile vs_3_0 ViewProjectionVS();
        PixelShader = compile ps_3_0 MaskPS();
    }

    pass PASS_AA_DRAW_DIRECT
    {
        StencilEnable = False;
        ZFunc = Greater;
        ZEnable = True;
        ZWriteEnable = False;
        ColorWriteEnable = 0x0f;

        VertexShader = compile vs_3_0 ViewProjectionVS();
        PixelShader = compile ps_3_0 DirectMaskPS();
    }
}

technique Aliased
{
    pass PASS_STENCIL_ODDEVEN
    {
        TwoSidedStencilMode = False;
        StencilEnable = True;
        StencilPass = Invert;
        StencilFunc = Always;
        ColorWriteEnable = 0;

        ZEnable = False;
        ZWriteEnable = False;

        VertexShader = compile vs_1_1 NoTxAliasedVS();
        PixelShader = compile ps_2_0 DirectSimplePS();
    }

    pass PASS_STENCIL_WINDING
    {
        TwoSidedStencilMode = True;
        StencilEnable = True;
        StencilRef = 0;
        StencilMask = 0xFFFFFFFF;

        CCW_StencilPass = Incr;
        CCW_StencilFunc = Always;

        StencilPass = Decr;
        StencilFunc = Always;

        ColorWriteEnable = 0;

        ZEnable = False;
        ZWriteEnable = False;

        VertexShader = compile vs_1_1 NoTxAliasedVS();
        PixelShader = compile ps_2_0 DirectSimplePS();
    }

    pass PASS_STENCIL_DRAW
    {
        TwoSidedStencilMode = False;
        StencilEnable = True;
        StencilFunc = NotEqual;
        StencilMask = 0xFFFFFFFF;
        StencilRef = 0;
        StencilPass = Zero;
        StencilFail = Zero;
        StencilZFail = Zero;

        ColorWriteEnable = 0x0f;
        ZEnable = True;
        ZWriteEnable = False;
        ZFunc = Greater;

        VertexShader = compile vs_1_1 AliasedVS();
        PixelShader = compile ps_2_0 SimplePS();
    }

    pass PASS_STENCIL_DRAW_DIRECT
    {
        TwoSidedStencilMode = False;
        StencilEnable = True;
        StencilFunc = NotEqual;
        StencilMask = 0xFFFFFFFF;
        StencilRef = 0;
        StencilPass = Zero;
        StencilFail = Zero;
        StencilZFail = Zero;

        ColorWriteEnable = 0x0f;
        ZEnable = True;
        ZWriteEnable = False;
        ZFunc = Greater;

        VertexShader = compile vs_1_1 AliasedVS();
        PixelShader = compile ps_2_0 DirectSimplePS();
    }

    pass PASS_STENCIL_CLIP
    {
        TwoSidedStencilMode = False;
        StencilEnable = True;
        StencilFunc = NotEqual;
        StencilMask = 0xFFFFFFFF;
        StencilRef = 0;
        StencilPass = Zero;
        StencilFail = Zero;
        StencilZFail = Zero;

        ColorWriteEnable = 0;
        ZEnable = True;
        ZWriteEnable = True;
        ZFunc = Always;

        VertexShader = compile vs_1_1 NoTxAliasedVS();
        PixelShader = compile ps_2_0 DirectSimplePS();
    }

    pass PASS_STENCIL_NOSTENCILCHECK
    {
        StencilEnable = False;

        ZEnable = True;
        ZWriteEnable = False;
        ZFunc = Greater;

        ColorWriteEnable = 0x0f;

        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;

        VertexShader = compile vs_1_1 AliasedVS();
        PixelShader = compile ps_2_0 SimplePS();
    }

    pass PASS_STENCIL_NOSTENCILCHECK_DIRECT
    {
        StencilEnable = False;

        ZEnable = True;
        ZWriteEnable = False;
        ZFunc = Greater;

        ColorWriteEnable = 0x0f;

        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;

        VertexShader = compile vs_1_1 AliasedVS();
        PixelShader = compile ps_2_0 DirectSimplePS();
    }

    pass PASS_TEXT
    {
        StencilEnable = False;

        ZEnable = True;
        ZWriteEnable = False;
        ZFunc = Greater;

        ColorWriteEnable = 0x0f;

        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;

        VertexShader = compile vs_1_1 AliasedVS();
        PixelShader = compile ps_2_0 TextPS();
    }

    pass PASS_CLEARTYPE_TEXT
    {
        StencilEnable = False;

        ZEnable = True;
        ZWriteEnable = False;
        ZFunc = Greater;

        ColorWriteEnable = 0x0f;

//         SrcBlend = SrcAlpha;
//         DestBlend = InvSrcAlpha;

//        SrcBlend = DestColor;
//        DestBlend = Zero;
        SrcBlend = BlendFactor;
        DestBlend = InvSrcColor;

//         SrcBlend = Zero;
//         DestBlend = SrcColor;

//         SrcBlend = One;
//         DestBlend = Zero;

        VertexShader = compile vs_3_0 AliasedVS();
        PixelShader = compile ps_3_0 ClearTypePS();
    }

    pass PASS_ALIASED_LINES
    {
        TwoSidedStencilMode = False;
        StencilEnable = True;
        StencilPass = Invert;
        StencilFunc = Always;
        ColorWriteEnable = 0;

        ZEnable = False;
        ZWriteEnable = False;

        VertexShader = compile vs_1_1 AliasedLinesVS();
        PixelShader = compile ps_2_0 DirectSimplePS();
    }

    pass PASS_ALIASED_LINES_DIRECT
    {
        StencilEnable = False;

        ZEnable = True;
        ZWriteEnable = False;
        ZFunc = Greater;

        ColorWriteEnable = 0x0f;

        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;

        VertexShader = compile vs_1_1 AliasedLinesVS();
        PixelShader = compile ps_2_0 DirectSimplePS();
    }
}

