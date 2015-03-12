//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
uniform extern float4x4 xWVP;
uniform extern float4x4 xWM;

//Scene Rendering Globals
uniform extern float4x4 xWorldInverseTransform;
uniform extern float4 xDiffMaterial;
uniform extern float4 xLightDiffColour;
uniform extern float3 xLightPosition;
uniform extern float3 xLightDirection;
uniform extern float xLightFalloff;

//Shadow Mapping Globals
uniform extern texture xShadowMap;
uniform extern float4x4 xLightWVP;

static const float cEpsilon = 0.01f;

sampler ShadowMapS : register(s1) = sampler_state
{
	Texture = <xShadowMap>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

//--------------------------------------------------------------------------------------
// Vertex shader output structures
//--------------------------------------------------------------------------------------
struct MapVS_OUTPUT
{
    float4 Position		: POSITION0;
	float2 Depth		: TEXCOORD0;
};

struct ShadowVS_OUTPUT
{
    float4 Position		: POSITION0;
	float4 Normal		: TEXCOORD0;
	float4 WPosition	: TEXCOORD1;
	float4 ShadowCoords	: TEXCOORD2;
};

struct LightVS_OUTPUT
{
    float4 Position		: POSITION0;
	float4 Normal		: TEXCOORD0;
	float4 WPosition	: TEXCOORD1;
};


//--------------------------------------------------------------------------------------
// Shadow map generation
//--------------------------------------------------------------------------------------

MapVS_OUTPUT ShadowMapVertexShader( float3 vPos : POSITION0)
{
    MapVS_OUTPUT Output = (MapVS_OUTPUT)0;
    
	Output.Position = mul(float4(vPos,1.0f), xWVP);
	Output.Depth = Output.Position.zw;
    
    return Output;    
}

float4 ShadowMapPixelShader(float2 depth: TEXCOORD0) : COLOR
{ 
    return depth.x/depth.y;
}


//--------------------------------------------------------------------------------------
// Render Geometry
//--------------------------------------------------------------------------------------

float4 GeometryVertexShader( float3 vPos : POSITION0) :POSITION0
{    
	return mul(float4(vPos,1.0f), xWVP);
}

float4 GeometryPixelShader(float4 c: COLOR0) : COLOR
{ 
    return xDiffMaterial*xLightDiffColour;
}

//--------------------------------------------------------------------------------------
// Render NonShadowing Light
//--------------------------------------------------------------------------------------
LightVS_OUTPUT DiffuseVertexShader( float3 vPos : POSITION0, float3 vNormal : NORMAL0)
{
    LightVS_OUTPUT Output = (LightVS_OUTPUT)0;

	Output.Position = mul(float4(vPos,1.0f), xWVP);

	Output.WPosition = mul(float4(vPos,1.0f), xWM);

	Output.Normal = mul(float4(vNormal, 0.0f), xWorldInverseTransform);
	Output.Normal = normalize(Output.Normal);

    return Output;    
}

float4 DiffusePixelShader(float4 normal : TEXCOORD0, float4 position : TEXCOORD1) : COLOR
{
	
	normal = normalize(normal);	

	float3 lightVector = xLightPosition - position.xyz;

	//Calculate attenuation of the light reaching the fragment (distance from light squared)
	float3 atten = lightVector.x*lightVector.x+lightVector.y*lightVector.y+lightVector.z*lightVector.z;

	normalize(lightVector);

	//Calculate the observed intensity of this fragment (Lambert cosine law)
	float s = max(dot(lightVector, normal.xyz),0.0f);
	float3 diffuse = s*(xDiffMaterial*xLightDiffColour).rgb;

	//Calculate falloff of light as the angle between the light direction and light vector approach 90 degrees
	float falloff = dot(xLightDirection, normalize(-lightVector));
	
	//Calculate final diffuse value
	diffuse = (diffuse/atten);	
	if(falloff<xLightFalloff)
		diffuse = diffuse*falloff/xLightFalloff;

	return float4(diffuse, xDiffMaterial.a);
}

//--------------------------------------------------------------------------------------
// Render Shadowing Light
//--------------------------------------------------------------------------------------

ShadowVS_OUTPUT ShadowingVertexShader( float3 vPos : POSITION0, float3 vNormal : NORMAL0)
{
    ShadowVS_OUTPUT Output = (ShadowVS_OUTPUT)0;

	Output.Position = mul(float4(vPos,1.0f), xWVP);
	Output.WPosition = mul(float4(vPos,1.0f), xWM);

	Output.Normal = mul(float4(vNormal, 0.0f), xWorldInverseTransform);
	Output.Normal = normalize(Output.Normal);

	Output.ShadowCoords = mul(float4(vPos, 1.0f), xLightWVP);

    return Output;    
}

float4 ShadowingPixelShader(float4 normal : TEXCOORD0, float4 position : TEXCOORD1,
	float4 shadowCoords	: TEXCOORD2) : COLOR
{
	float4 diffuse = DiffusePixelShader(normal, position);
			
	shadowCoords.xy /= shadowCoords.w;
	shadowCoords.x = 0.5f*shadowCoords.x + 0.5f;
	shadowCoords.y = -0.5f*shadowCoords.y + 0.5f;

	float4 texColour = tex2D(ShadowMapS, shadowCoords.xy);

	float pixelDepth = shadowCoords.z/shadowCoords.w;
	float mapDepth = tex2D(ShadowMapS, shadowCoords.xy).r+cEpsilon;

	//Shadow Mapping
	return diffuse*(pixelDepth<mapDepth);
}


//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------

technique GenerateShadowMap
{
    pass P0
    {          
        vertexShader = compile vs_2_0 ShadowMapVertexShader();
        pixelShader  = compile ps_2_0 ShadowMapPixelShader();
		AlphaBlendEnable = false;
	}
}

technique RenderGeometry
{
	pass P0
	{
		vertexShader = compile vs_2_0 GeometryVertexShader();
        pixelShader  = compile ps_2_0 GeometryPixelShader();

		FillMode = Solid;
		AlphaBlendEnable = false;
	}
}

technique RenderLight
{
	pass P0
	{
		vertexShader = compile vs_2_0 DiffuseVertexShader();
        pixelShader  = compile ps_2_0 DiffusePixelShader();

		FillMode = Solid;
		AlphaBlendEnable = true;
		SrcBlend = One;
		DestBlend = One;
		ZWriteEnable = false;
	}
}

technique RenderShadowedLight
{
	pass P0
	{
		vertexShader = compile vs_2_0 ShadowingVertexShader();
        pixelShader  = compile ps_2_0 ShadowingPixelShader();

		FillMode = Solid;
		AlphaBlendEnable = true;
		SrcBlend = One;
		DestBlend = One;
		ZWriteEnable = false;
	}
}