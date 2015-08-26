#include "stdafx.h"
#include "DirectX11Util.h"


ID3DX11Effect* DirectX11Util::CreateEffectFromMemory( ID3D11Device *pd3dDevice, CONST WCHAR* szFileName )
{
	ID3DX11Effect *pEffect = NULL;

	std::ifstream fin( szFileName, std::ios::binary );

	fin.seekg( 0, std::ios_base::end );
	int size = (int)fin.tellg();
	fin.seekg( 0, std::ios_base::beg );
	std::vector<char> compiledShader( size );

	fin.read( &compiledShader[0], size );
	fin.close();

	HR( D3DX11CreateEffectFromMemory( &compiledShader[0], size,
		0, pd3dDevice, &pEffect ) );

	return pEffect;
}

ID3D11VertexShader* DirectX11Util::CreateVertexShader( ID3D11Device *pd3dDevice, CONST WCHAR* szFileName )
{
	ID3D11VertexShader *pVS = NULL;

	std::ifstream fin( szFileName, std::ios::binary );

	fin.seekg( 0, std::ios_base::end );
	int size = (int)fin.tellg();
	fin.seekg( 0, std::ios_base::beg );
	std::vector<char> compiledShader( size );

	fin.read( &compiledShader[0], size );
	fin.close();

	HR( pd3dDevice->CreateVertexShader( &compiledShader, size, NULL, &pVS ) );

	return pVS;
}

ID3D11PixelShader* DirectX11Util::CreatePixelShader( ID3D11Device *pd3dDevice, CONST WCHAR* szFileName )
{
	ID3D11PixelShader *pPS = NULL;

	std::ifstream fin( szFileName, std::ios::binary );

	fin.seekg( 0, std::ios_base::end );
	int size = (int)fin.tellg();
	fin.seekg( 0, std::ios_base::beg );
	std::vector<char> compiledShader( size );

	fin.read( &compiledShader[0], size );
	fin.close();

	HR( pd3dDevice->CreatePixelShader( &compiledShader, size, NULL, &pPS ) );

	return pPS;
}

ID3D11Buffer* DirectX11Util::CreateBuffer( ID3D11Device *pd3dDevice, UINT ByteWidth, D3D11_USAGE Usage,
	UINT CPUAccessFlags )
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );

	desc.Usage = Usage;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = CPUAccessFlags;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.ByteWidth = ByteWidth;

	ID3D11Buffer* pBuffer = NULL;
	HR( pd3dDevice->CreateBuffer( &desc, NULL, &pBuffer ) );
	return pBuffer;
}

ID3D11SamplerState* DirectX11Util::CreateSamplerState( ID3D11Device* pd3dDevice, D3D11_FILTER filter,
	D3D11_TEXTURE_ADDRESS_MODE mode )
{
	D3D11_SAMPLER_DESC desc;
	ZeroMemory( &desc, sizeof( D3D11_SAMPLER_DESC ) );

	desc.AddressU = mode;
	desc.AddressV = mode;
	desc.AddressW = mode;
	desc.Filter = filter;

	ID3D11SamplerState* pSS;
	HR( pd3dDevice->CreateSamplerState( &desc, &pSS ) );
	return pSS;
}
