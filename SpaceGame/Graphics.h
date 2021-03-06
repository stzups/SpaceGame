#pragma once

#include "EngineException.h"
#include "Windows.h"
#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <vector>
#include <wrl.h>
#include "DXGIInfoManager.h"
#include <mutex>
#include <d3dcompiler.h>
#include <DirectXMath.h>

class Graphics
{
	friend class Bindable;
public:
	Graphics(HWND hWnd);
	~Graphics();
	void Present(UINT syncInterval, UINT flags);
	void Clear(float red, float green, float blue);
	void CreateDevice();
	void CreateResources();
	void SetFullscreenState(bool fullscreen);
	void ResizeBuffers(UINT width, UINT height);
	void DrawIndexed(UINT count);
	void SetProjection(DirectX::FXMMATRIX projection);
	DirectX::XMMATRIX GetProjection() const;
	void OnDeviceLost();
	void ReportLiveObjects();
	UINT GetBackBufferWidth();
	UINT GetBackBufferHeight();
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
private:
	std::mutex mutex;
	HWND hWnd;
	DxgiInfoManager infoManager;
	DirectX::XMMATRIX projection;
	BOOL wasFullscreen;
private:
	UINT backBufferWidth;
	UINT backBufferHeight;
	D3D_FEATURE_LEVEL featureLevel;
	DXGI_SWAP_EFFECT swapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
	const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	const UINT backBufferCount = 2;
public:
	class Exception : public EngineException
	{
		using EngineException::EngineException;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMessages = {});
		const char* what() const override;
		const char* GetType() const override;
		HRESULT GetErrorCode() const;
		std::string GetErrorInfo() const;
	private:
		HRESULT hr;
		std::string info;
	};

	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const override;
	};

	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMessages);
		const char* what() const override;
		const char* GetType() const override;
		std::string GetErrorInfo() const;
	private:
		std::string info;
	};
};