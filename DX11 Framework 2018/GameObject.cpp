#include "GameObject.h"
#include "Application.h"

GameObject::GameObject(MeshData mesh, Application* app, wchar_t* textureName)
{
	_mesh = mesh;
	_pApp = app;
	XMStoreFloat4x4(&_world, XMMatrixIdentity());
	CreateDDSTextureFromFile(_pApp->GetDevice(), textureName, nullptr, &_pTexture);
}

GameObject::~GameObject()
{
	_pTexture->Release();
	if (_pSpecMap != nullptr)
		_pSpecMap->Release();
	delete _pChild;
}

void GameObject::SetSpecularMap(wchar_t* specMapName)
{
	CreateDDSTextureFromFile(_pApp->GetDevice(), specMapName, nullptr, &_pSpecMap);
}

void GameObject::MakeForwardVector()
{
	_forward.x = cos(_rotation.z) * cos(_rotation.y);
	_forward.y = sin(_rotation.z) * cos(_rotation.y);
	_forward.z = sin(_rotation.y) * -1;
}

void GameObject::Update(XMMATRIX ParentWorld)
{
	XMVECTOR scale = XMLoadFloat3(&_scale);
	XMVECTOR pos = XMLoadFloat3(&_position);
	XMVECTOR rot = XMLoadFloat3(&_rotation);

	if (_bAffectedByWaves)
	{
		if (GetAsyncKeyState(0x41)) // A
		{
			_rotation.y -= 0.00025f * _pApp->GetTime();
			rot = XMLoadFloat3(&_rotation);
		}

		if (GetAsyncKeyState(0x44)) // A
		{
			_rotation.y += 0.00025f * _pApp->GetTime();
			rot = XMLoadFloat3(&_rotation);
		}

		MakeForwardVector();
		XMVECTOR forward = XMVector3Normalize(XMLoadFloat3(&_forward));

		if (GetAsyncKeyState(0x57)) // W
		{
			pos += forward * 0.005 * _pApp->GetTime();
			XMStoreFloat3(&_position, pos);
		}

	}

	XMStoreFloat4x4(&_world, XMMatrixScalingFromVector(scale) * XMMatrixRotationRollPitchYawFromVector(rot) * XMMatrixTranslationFromVector(pos) * ParentWorld);

	if (_pChild != nullptr)
	{
		XMMATRIX world = XMLoadFloat4x4(&_world);
		_pChild->Update(world);
	}
}
void GameObject::Draw()
{
	ConstantBuffer* cb = _pApp->GetCurrentConstantBuffer();
	ID3D11Buffer* constantBuffer = _pApp->GetConstantBuffer();
	ID3D11DeviceContext* immediateContext = _pApp->GetImmediateContext();

	immediateContext->PSSetShaderResources(0, 1, &_pTexture);
	if (_bHasSpecular)
		immediateContext->PSSetShaderResources(1, 1, &_pSpecMap);

	XMMATRIX world = XMLoadFloat4x4(&_world);
	cb->mWorld = XMMatrixTranspose(world);
	cb->hasSpecular = _bHasSpecular;
	immediateContext->UpdateSubresource(constantBuffer, 0, nullptr, cb, 0, 0);

	immediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	immediateContext->VSSetConstantBuffers(0, 1, &constantBuffer);
	immediateContext->PSSetConstantBuffers(0, 1, &constantBuffer);
	immediateContext->PSSetShader(_pPixelShader, nullptr, 0);

	immediateContext->IASetVertexBuffers(0, 1, &_mesh.VertexBuffer, &_mesh.VBStride, &_mesh.VBOffset);
	immediateContext->IASetIndexBuffer(_mesh.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	immediateContext->DrawIndexed(_mesh.IndexCount, 0, 0);

	if (_pChild != nullptr)
	{
		_pChild->Draw();
	}
}