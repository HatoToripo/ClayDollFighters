//=============================================================================
//
// メイン処理 [main.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"
#include "sound.h"
#include "model.h"
#include "player.h"
#include "enemy.h"
#include "game.h"
#include "setting.h"
#include "fade.h"
#include "title.h"
#include "result.h"
#include "debugproc.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"				// ウインドウのクラス名
#define WINDOW_NAME		"ClayDoll Fighters"	// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

void CheckHit(void);


//*****************************************************************************
// グローバル変数:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPSカウンタ
char	g_DebugStr[2048] = WINDOW_NAME;		// デバッグ文字表示用

#endif

int g_Mode = MODE_TITLE;					// 起動時の画面を設定
//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	// 時間計測用
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// ウィンドウの左座標
		CW_USEDEFAULT,																		// ウィンドウの上座標
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// ウィンドウ横幅
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// ウィンドウ縦幅
		NULL,
		NULL,
		hInstance,
		NULL);

	// DirectXの初期化(ウィンドウを作成してから行う)
	BOOL mode = TRUE;

	int id = MessageBox(NULL, "Windowモードでプレイしますか？", "起動モード", MB_YESNOCANCEL | MB_ICONQUESTION);
	switch (id)
	{
	case IDYES:		// YesならWindowモードで起動
		mode = TRUE;
		break;
	case IDNO:		// Noならフルスクリーンモードで起動
		mode = FALSE;
		break;
	case IDCANCEL:	// CANCELなら終了
	default:
		return -1;
		break;
	}

	// DirectXの初期化(ウィンドウを作成してから行う)
	if (FAILED(Init(hInstance, hWnd, mode)))
	{
		return -1;
	}

	// フレームカウント初期化
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// メッセージループ
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳と送出
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
				dwFrameCount = 0;							// カウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60秒ごとに実行
			{
				dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存

#ifdef _DEBUG	// デバッグ版の時だけFPSを表示する
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// 更新処理
				if (g_Mode == MODE_MAX) break;
				Draw();				// 描画処理

#ifdef _DEBUG	// デバッグ版の時だけ表示する
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// 分解能を戻す

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUG

	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;
	// (Your code process Win32 messages)

#endif

	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);

#ifdef _DEBUG

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // imguiのキーボード操作
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // imguiのパッド操作


	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hWnd);            // YOUR HWNDにhWndを渡す
	ImGui_ImplDX11_Init(GetDevice(), GetDeviceContext());    // YOUR_D3D_DEVICEをGetDeviceに変更 // YOUR_D3D_DEVICE_CONTEXTをGetDeviceContextに変更

#endif

	InitLight();

	InitCamera();

	SetViewPort(TYPE_FULL_SCREEN);

	// 入力処理の初期化
	InitInput(hInstance, hWnd);

	// サウンドの初期化
	InitSound(hWnd);



	SetLightEnable(TRUE);

	// 背面ポリゴンをカリング
	SetCullingMode(CULL_MODE_BACK);

	// フェードの初期化
	InitFade();

	// 最初のモードをセット
	SetMode(g_Mode);	// ここはSetModeのままで！


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	// 終了のモードをセット
	SetMode(MODE_MAX);

	// サウンド終了処理
	UninitSound();

	// カメラの終了処理
	UninitCamera();

	//入力の終了処理
	UninitInput();

#ifdef _DEBUG

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif

	UninitRenderer();
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	// 入力の更新処理
	UpdateInput();

#ifdef _DEBUG

	// (Your code process and dispatch Win32 messages)
// Imguiの画面を作る
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow(); // imgui_demo.cpp内のものを表示（サンプル表示）

#endif

	// ライトの更新処理
	UpdateLight();

	// カメラ更新
	UpdateCamera();

	// モードによって処理を分ける
	switch (g_Mode)
	{
	case MODE_TITLE:		// タイトル画面の更新
		if (GetSettingFlag() == TRUE)
		{
			UpdateSetting();
		}
		else
		{
			UpdatePlayer();
			UpdateTitle();
		}
		break;

	case MODE_GAME:			// ゲーム画面の更新
		if (GetSettingFlag() == FALSE)
		{
			UpdateGame();
		}

		else
		{
			UpdateSetting();
		}
		break;

	case MODE_RESULT:		// リザルト画面の更新
		UpdateResult();
		break;
	case MODE_GAMEOVER:
		SetFade(FADE_OUT, MODE_GAME);
		break;
	}

	// フェード処理の更新
	UpdateFade();


}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// バックバッファクリア
	Clear();
	ClearDSV();

	SetCamera();

	// モードによって処理を分ける
	switch (g_Mode)
	{
	case MODE_TITLE:		// タイトル画面の描画
		SetViewPort(TYPE_FULL_SCREEN);

		if (GetSettingFlag() == TRUE)
		{
			// 2Dの物を描画する処理
			// Z比較なし
			SetDepthEnable(FALSE);

			// ライティングを無効
			SetLightEnable(FALSE);

			DrawSetting();

			// ライティングを有効に
			SetLightEnable(TRUE);

			// Z比較あり
			SetDepthEnable(TRUE);
		}
		else
		{
			PLAYER* player = GetPlayer();
			CAMERA* camera = GetCamera();


			XMFLOAT3 pos = player->pos;

			camera->pos = player->pos;
			pos.x += 20.0f;
			pos.y += 5.0f;			// カメラ酔いを防ぐためにクリアしている
			pos.z -= 5.0f;
			SetCameraAT(pos);
			SetCamera();

			DrawPlayer();
			// 2Dの物を描画する処理
			// Z比較なし
			SetDepthEnable(FALSE);

			// ライティングを無効
			SetLightEnable(FALSE);

			DrawTitle();
			// ライティングを有効に
			SetLightEnable(TRUE);

			// Z比較あり
			SetDepthEnable(TRUE);

		}
		break;

	case MODE_GAME:			// ゲーム画面の描画
		if (GetSettingFlag() == FALSE)
		{
			DrawGame();
		}
		else
		{
			// 2Dの物を描画する処理
			// Z比較なし
			SetDepthEnable(FALSE);

			// ライティングを無効
			SetLightEnable(FALSE);

			// セッティングの描画
			DrawSetting();

			// ライティングを有効に
			SetLightEnable(TRUE);

			// Z比較あり
			SetDepthEnable(TRUE);
		}
		break;

	case MODE_RESULT:		// リザルト画面の描画
		SetViewPort(TYPE_FULL_SCREEN);

		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawResult();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
		break;

	case MODE_GAMEOVER:
		break;
	}

	{	// フェード処理
		SetViewPort(TYPE_FULL_SCREEN);

		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		// フェード描画
		DrawFade();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
	}

#ifdef _DEBUG
	// デバッグ表示
	DrawDebugProc();

	// imgui画面描画
// (Your code clears your framebuffer, renders your other stuff etc.)
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	// (Your code calls swapchain's Present() function)
	// 
	// バックバッファ、フロントバッファ入れ替え
#endif

	Present();

}

long GetMousePosX(void)
{
	return g_MouseX;
}


long GetMousePosY(void)
{
	return g_MouseY;
}


#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif


//=============================================================================
// モードの設定
//=============================================================================
void SetMode(int mode)
{
	// モードを変える前に全部メモリを解放しちゃう

	// 設定画面の終了処理
	UninitSetting();

	// タイトル画面の終了処理
	UninitTitle();

	// ゲーム画面の終了処理
	UninitGame();

	// リザルト画面の終了処理
	UninitResult();

	g_Mode = mode;	// 次のモードをセットしている

	switch (g_Mode)
	{
	case MODE_TITLE:
		// タイトル画面の初期化
		UninitCamera();
		InitCamera();
		InitPlayer();
		InitTitle();
		InitSetting();
		PlaySound(SOUND_LABEL_BGM_op);
		break;

	case MODE_GAME:
		// カメラもここで初期化しておく事にした
		UninitCamera();
		InitCamera();
		// ゲーム画面の初期化
		InitSetting();
		InitGame();
		PlaySound(SOUND_LABEL_BGM_field);
		break;

	case MODE_RESULT:
		// リザルト画面の初期化
		InitResult();
		PlaySound(SOUND_LABEL_BGM_clear);
		break;
		// ゲーム終了時の処理
	case MODE_MAX:
		// エネミーの終了処理
		UninitEnemy();
		
		// プレイヤーの終了処理
		UninitPlayer();
		break;
	}
}

//=============================================================================
// モードの取得
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}

