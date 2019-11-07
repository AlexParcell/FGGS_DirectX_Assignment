#include "GameObject.h"
#include "Application.h"

void GameObject::Update()
{
	XMStoreFloat4x4(&world, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixRotationY(app->GetTime()) * XMMatrixTranslation(0.0f, 0.0f, 0.0f));
}

void GameObject::Draw()
{
	ConstantBuffer* cb = app->GetCurrentConstantBuffer();
	ID3D11Buffer* constantBuffer = app->GetConstantBuffer();
	ID3D11DeviceContext* immediateContext = app->GetImmediateContext();

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
}