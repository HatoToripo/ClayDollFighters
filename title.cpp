
//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : 
//
//=============================================================================
#include "title.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "setting.h"
#include "tutorial.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 

#define TEXTURE_TITLE_X				(SCREEN_WIDTH / 5)		// タイトルの横幅
#define TEXTURE_TITLE_Y				(SCREEN_HEIGHT / 10)		// タイトルの縦幅
#define TEXTURE_TITLE_WIDTH			(SCREEN_WIDTH * 3 / 5)		// タイトルの縦幅
#define TEXTURE_TITLE_HEIGHT		(SCREEN_HEIGHT * 2 / 10)		// タイトルの縦幅

#define TEXTURE_X_LOGO				((float)(TEXTURE_WIDTH / 4))
#define TEXTURE_Y_LOGO				(280.0f)
#define TEXTURE_WIDTH_LOGO			(480.0f)			// ロゴの縦幅
#define TEXTURE_HEIGHT_LOGO			(50.0f)			// ロゴの横幅
#define TEXTURE_INTERVAL_LOGO		(50.0f)				// ロゴの間隔

enum 
{
	START_GAME,
	SETTING,
#ifdef _DEBUG
	EDITOR,
#endif
	QUIT_GAME,
	MENU_MAX
};

enum
{
	TEXTURE_BG,
	TEXTURE_TITLE,
	TEXTURE_START,
	TEXTURE_SELECT,
	TEXTURE_SETTING,
#ifdef _DEBUG
	TEXTURE_EDITOR,

#endif
	TEXTURE_QUIT,
	TEXTURE_CURSOR,

	TEXTURE_MAX
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {

	"data/TEXTURE/setting_back.png",
	"data/TEXTURE/title.png",
	"data/TEXTURE/title_startgame.png",
	"data/TEXTURE/title_selectstage.png",
	"data/TEXTURE/title_settings.png",
#ifdef _DEBUG
	"data/TEXTURE/title_editor.png",
#endif
	"data/TEXTURE/title_quitgame.png",
	"data/TEXTURE/title_cursor.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号
static int						g_Cursor;					// カーソルの位置

float	alpha;
BOOL	flag_alpha;

static BOOL						g_Load = FALSE;

static float	effect_dx;
static float	effect_dy;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// 変数の初期化
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(g_w/2, g_h/2, 0.0f);
	g_TexNo = TEXTURE_BG;
	g_Cursor = 0;

	alpha = 1.0f;
	flag_alpha = TRUE;

	effect_dx = 100.0f;
	effect_dy = 100.0f;


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTitle(void)
{
	// 下キーを押したらカーソルの位置を下げる
	if (GetKeyboardTrigger(DIK_DOWN))
	{
		g_Cursor = (g_Cursor + 1) % MENU_MAX;
		PlaySound(SOUND_LABEL_SE_select);
		// セーブデータが無かったらロード画面に遷移できないようにする
		//if(g_Cursor % MENU_MAX == LOAD_GAME && CheckData() == FALSE) 	g_Cursor++;
	}

	else if (IsButtonTriggered(0, BUTTON_DOWN))
	{
		g_Cursor = (g_Cursor + 1) % MENU_MAX;
		PlaySound(SOUND_LABEL_SE_select);
		// セーブデータが無かったらロード画面に遷移できないようにする
		//if(g_Cursor % MENU_MAX == LOAD_GAME && CheckData() == FALSE) 	g_Cursor++;
	}

	
	// 上キーを押したらカーソルの位置を上げる
	if (GetKeyboardTrigger(DIK_UP))
	{
		g_Cursor = (g_Cursor + MENU_MAX - 1) % MENU_MAX;
		PlaySound(SOUND_LABEL_SE_select);

		// セーブデータが無かったらロード画面に遷移できないようにする
		//if (g_Cursor % MENU_MAX == LOAD_GAME && CheckData() == FALSE) 	g_Cursor--;
	}
	else if (IsButtonTriggered(0, BUTTON_UP))
	{
		g_Cursor = (g_Cursor + MENU_MAX - 1) % MENU_MAX;
		PlaySound(SOUND_LABEL_SE_select);

		// セーブデータが無かったらロード画面に遷移できないようにする
		//if (g_Cursor % MENU_MAX == LOAD_GAME && CheckData() == FALSE) 	g_Cursor--;
	}

	switch (g_Cursor % MENU_MAX)
	{
	// 新規ゲーム開始
	case START_GAME:
		if (GetKeyboardTrigger(DIK_RETURN))
		{// Enter押したら、ステージを切り替える
			SetFade(FADE_OUT, MODE_GAME);
			PlaySound(SOUND_LABEL_SE_enter);
			SetTutorialFlg();
		}
		// ゲームパッドで入力処理
		else if (IsButtonTriggered(0, BUTTON_B))
		{
			SetFade(FADE_OUT, MODE_GAME);
			PlaySound(SOUND_LABEL_SE_enter);
			SetTutorialFlg();
		}
		break;

	
	// 設定画面を表示
	case SETTING:
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			SetSettingFlag();
			PlaySound(SOUND_LABEL_SE_enter);
		}
		// ゲームパッドで入力処理
		else if (IsButtonTriggered(0, BUTTON_B))
		{
			SetSettingFlag();
			PlaySound(SOUND_LABEL_SE_enter);
		}
		break;

#ifdef _DEBUG
	case EDITOR:
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			SetMode(MODE_EDITOR);
			PlaySound(SOUND_LABEL_SE_enter);
		}
		// ゲームパッドで入力処理
		else if (IsButtonTriggered(0, BUTTON_B))
		{
			SetMode(MODE_EDITOR);
			PlaySound(SOUND_LABEL_SE_enter);
		}
		break;
#endif

	case QUIT_GAME:
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			SetMode(MODE_MAX);
			PlaySound(SOUND_LABEL_SE_enter);
		}
		// ゲームパッドで入力処理
		else if (IsButtonTriggered(0, BUTTON_B))
		{
			SetMode(MODE_MAX);
			PlaySound(SOUND_LABEL_SE_enter);
		}
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Player:↑ → ↓ ←　Space\n");

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_BG]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.1f, 1.0f, 0.1f);

		// ポリゴン描画
		//GetDeviceContext()->Draw(4, 0);
	}

	// タイトルの描画
		// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_TITLE]);

	//ゲージの位置やテクスチャー座標を反映
	float px = TEXTURE_TITLE_X;		// SelectStageの表示位置X
	float py = TEXTURE_TITLE_Y;		// SelectStageの表示位置Y
	float pw = TEXTURE_TITLE_WIDTH;		// ゲージの表示幅
	float ph = TEXTURE_TITLE_HEIGHT;		// ゲージの表示高さ

	float tw = 1.0f;	// テクスチャの幅
	float th = 1.0f;	// テクスチャの高さ
	float tx = 0.0f;	// テクスチャの左上X座標
	float ty = 0.0f;	// テクスチャの左上Y座標

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_START]);

	//ゲージの位置やテクスチャー座標を反映
	px = TEXTURE_X_LOGO;		// SelectStageの表示位置X
	py = TEXTURE_Y_LOGO;		// SelectStageの表示位置Y
	pw = TEXTURE_WIDTH_LOGO;		// ゲージの表示幅
	ph = TEXTURE_HEIGHT_LOGO;		// ゲージの表示高さ

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_SETTING]);

	//次のロゴを描画
	py += TEXTURE_INTERVAL_LOGO;		// Settingの表示位置Y

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);


#ifdef _DEBUG
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_EDITOR]);

	//次のロゴを描画
	py += TEXTURE_INTERVAL_LOGO;		// Settingの表示位置Y

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

#endif

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_QUIT]);

	//次のロゴを描画
	py += TEXTURE_INTERVAL_LOGO;		// QuitGameの表示位置Y

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_CURSOR]);

	//カーソルを描画
	py = TEXTURE_Y_LOGO + TEXTURE_INTERVAL_LOGO * (g_Cursor % MENU_MAX);		// カーソルの表示位置Y

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
}





