#pragma once

#include "d3dUtil.h"
#include "GameTimer.h"
#include "RendererCore.h"

class RendererCore;

class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);
	virtual ~D3DApp();

	HINSTANCE AppInst() const;
	HWND MainWnd() const;
	float AspectRatio() const;

	int Run();

	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float dt) = 0;
	virtual void DrawScene() = 0;

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }


protected:
	bool InitMainWindow();
	bool InitDirect3D();

	void CalculateFrameStats();

protected:

	HINSTANCE m_hAppInst;
	HWND      m_hMainWnd;
	bool      m_bAppPaused;
	bool      m_bMinimized;
	bool      m_bMaximized;
	bool      m_bResizing;

	GameTimer m_Timer;


	wstring m_wsMainWndCaption;
	
	//Renderer* m_pRenderer;
};

