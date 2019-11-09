#include "GameObject.h"
#include "Application.h"

GameObject::GameObject(MeshData _mesh, Application* _app, wchar_t* textureName)
{
	mesh = _mesh;
	app = _app;
	XMStoreFloat4x4(&world, XMMatrixIdentity());
	CreateDDSTextureFromFile(app->GetDevice(), textureName, nullptr, &Texture);
	CreateDDSTextureFromFile(app->GetDevice(), L"Cube_SPEC.dds", nullptr, &specMap);
}

GameObject::~GameObject()
{
	Texture->Release();
	delete Child;
}

void GameObject::Update(XMMATRIX ParentWorld)
{
	if (!IsChild)
	{
		XMFLOAT3 rot;
		XMStoreFloat3(&rot, Rotation);
		rot.y = app->GetTime();
		Rotation = XMLoadFloat3(&rot);
	}

	XMStoreFloat4x4(&world, XMMatrixScalingFromVector(Scale) * XMMatrixRotationRollPitchYawFromVector(Rotation) * XMMatrixTranslationFromVector(Position) * ParentWorld);

	if (Child != nullptr)
	{
		XMMATRIX _world = XMLoadFloat4x4(&world);
		Child->Update(_world);
	}
}

void GameObject::Draw()
{
	ConstantBuffer* cb = app->GetCurrentConstantBuffer();
	ID3D11Buffer* constantBuffer = app->GetConstantBuffer();
	ID3D11DeviceContext* immediateContext = app->GetImmediateContext();

	immediateContext->PSSetShaderResources(0, 1, &Texture);
	immediateContext->PSSetShaderResources(1, 1, &specMap);

	XMMATRIX _world = XMLoadFloat4x4(&world);
	cb->mWorld = XMMatrixTranspose(_world);
	immediateContext->UpdateSubresource(constantBuffer, 0, nullptr, cb, 0, 0);

	immediateContext->VSSetShader(vertexShader, nullptr, 0);
	immediateContext->VSSetConstantBuffers(0, 1, &constantBuffer);
	immediateContext->PSSetConstantBuffers(0, 1, &constantBuffer);
	immediateContext->PSSetShader(pixelShader, nullptr, 0);

	immediateContext->IASetVertexBuffers(0, 1, &mesh.VertexBuffer, &mesh.VBStride, &mesh.VBOffset);
	immediateContext->IASetIndexBuffer(mesh.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	immediateContext->DrawIndexed(mesh.IndexCount, 0, 0);

	if (Child != nullptr)
	{
		Child->SetPixelShader(pixelShader);
		Child->SetVertexShader(vertexShader);
		Child->Draw();
	}
}