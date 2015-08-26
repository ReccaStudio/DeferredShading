#pragma once
#include <D3Dcompiler.h>


namespace DirectX11Util
{
	inline HRESULT CompileFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut, const D3D10_SHADER_MACRO* pDefines = NULL )
	{
		HRESULT hr = S_OK;

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
		// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
		// Setting this flag improves the shader debugging experience, but still allows 
		// the shaders to be optimized and to run exactly the way they will run in 
		// the release configuration of this program.
		dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

		ID3DBlob* pErrorBlob;
		hr = D3DX11CompileFromFile( szFileName, pDefines, NULL, szEntryPoint, szShaderModel,
			dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
		if ( FAILED( hr ) )
		{
			if ( pErrorBlob != NULL )
				OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
			ReleaseCOM( pErrorBlob );
			return hr;
		}
		ReleaseCOM( pErrorBlob );

		return S_OK;
	}

	ID3DX11Effect* CreateEffectFromMemory( ID3D11Device *pd3dDevice, CONST WCHAR* szFileName );


	ID3D11VertexShader* CreateVertexShader( ID3D11Device *pd3dDevice, CONST WCHAR* szFileName );


	ID3D11PixelShader* CreatePixelShader( ID3D11Device *pd3dDevice, CONST WCHAR* szFileName );


	ID3D11Buffer* CreateBuffer( ID3D11Device *pd3dDevice, UINT ByteWidth, D3D11_USAGE Usage = D3D11_USAGE_DYNAMIC,
		UINT CPUAccessFlags = D3D11_CPU_ACCESS_WRITE );


	ID3D11SamplerState* CreateSamplerState( ID3D11Device* pd3dDevice, D3D11_FILTER filter,
		D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_WRAP );
}
