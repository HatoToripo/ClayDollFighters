//=============================================================================
//
// 設定画面処理 [setting.cpp]
// Author : 服部圭吾
//
//=============================================================================
#include "setting.h"
#include "sprite.h"
#include "sound.h"
#include "input.h"
#include "fade.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_X					(0.0f)		// 背景の横幅
#define TEXTURE_Y					(0.0f)		// 背景の縦幅
#define TEXTURE_WIDTH				(SCREEN_WIDTH - TEXTURE_X * 2)		// ボードの横幅
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT - TEXTURE_Y * 2)	// ボードの縦幅

#define TEXTURE_LOGO_VOLMIN_WIDTH			(50.0f)		// ボリュームロゴの横幅
#define TEXTURE_LOGO_VOLMIN_HEIGHT			(25.0f)		// ボリュームロゴの縦幅

#define TEXTURE_LOGO_VOLMIN_X		(350.0f)		// MINのX座標
#define TEXTURE_LOGO_VOLMIN_Y		(75.0f)			// MINのY座標
#define TEXTURE_LOGO_VOLMAX_X		(750.0f)		// MAXのX座標

#define TEXTURE_LOGO_BGM_X		(175.0f)			// ボリュームロゴのX座標
#define TEXTURE_LOGO_BGM_Y		(TEXTURE_LOGO_VOLMIN_Y + TEXTURE_LOGO_VOLMIN_HEIGHT * 1.5)		// ボリュームロゴのY座標
#define TEXTURE_LOGO_BGM_WIDTH	(200.0f)		// ボリュームロゴのY座標
#define TEXTURE_LOGO_BGM_HEIGHT	(50.0f)		// ボリュームロゴのY座標

#define TEXTURE_LOGO_SE_Y		(TEXTURE_LOGO_BGM_Y + TEXTURE_LOGO_BGM_HEIGHT * 1.5)		// ボリュームロゴのY座標

#define TEXTURE_VOLSIZE_X			(TEXTURE_LOGO_VOLMIN_X + TEXTURE_LOGO_VOLMIN_WIDTH * 0.5 )			// ボリューム間隔のX座標
#define TEXTURE_VOLSIZE_Y			(TEXTURE_LOGO_VOLMIN_Y + TEXTURE_LOGO_VOLMIN_HEIGHT * 1.5)			// ボリューム間隔のY座標
#define TEXTURE_VOLSIZE_WIDTH		(TEXTURE_LOGO_VOLMAX_X - TEXTURE_LOGO_VOLMIN_X)					// ボリューム間隔の横幅
#define TEXTURE_VOLSIZE_HEIGHT		(50.0f)															// ボリューム間隔の縦幅

#define TEXTURE_BAR_Y				(TEXTURE_VOLSIZE_Y + TEXTURE_VOLSIZE_HEIGHT * 0.25)
#define TEXTURE_BAR_HEIGHT			(TEXTURE_VOLSIZE_HEIGHT * 0.5f)

#define TEXTURE_CIRCLE_HEIGHT		(TEXTURE_BAR_HEIGHT + 5.0f)
#define TEXTURE_CIRCLE_WIDTH		(TEXTURE_CIRCLE_HEIGHT)
#define TEXTURE_CIRCLE_Y			(TEXTURE_BAR_Y - 5.0f * 0.5f)

#define TEXTURE_BACK_X				((SCREEN_WIDTH - TEXTURE_LOGO_BGM_WIDTH) * 0.5)
#define TEXTURE_BACK_Y				(325.0f)
#define TEXTURE_GOTITLE_Y			(TEXTURE_BACK_Y + TEXTURE_LOGO_BGM_HEIGHT)

#define TEXTURE_CURSOR_WIDTH		(50.0f)
#define TEXTURE_CURSOR_HEIGHT		(50.0f)
#define TEXTURE_CURSOR_X			(TEXTURE_BACK_X)
#define TEXTURE_CURSOR_Y			(TEXTURE_BACK_Y)

enum {
	MENU_BGM,
	MENU_SE,
	MENU_RETRUN,
	MENU_GO_TITLE,

	MENU_MAX
};

enum {
	TEXTURE_BG,
	TEXTURE_VOLSIZE,
	TEXTURE_BAR,
	TEXTURE_CIRCLE,
	TEXTURE_VOL_MAX,
	TEXTURE_VOL_MIN,
	TEXTURE_BGM,
	TEXTURE_SE,
	TEXTURE_CURSOR,
	TEXTURE_BACK,
	TEXTURE_GOTITLE,
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
	"data/TEXTURE/setting_volumesize.png",
	"data/TEXTURE/bar_green.png",
	"data/TEXTURE/setting_circle.png",
	"data/TEXTURE/setting_max.png",
	"data/TEXTURE/setting_min.png",
	"data/TEXTURE/setting_bgm.png",
	"data/TEXTURE/setting_se.png",
	"data/TEXTURE/select_cursor.png",
	"data/TEXTURE/logo_back.png",
	"data/TEXTURE/setting_gotitle.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号
static int						g_VolumeNum;				// ボリュームにセットする値
static int						g_SEVolumeNum;				// SEボリュームにセットする値

static float	alpha;
static BOOL	flag_alpha;

static BOOL						g_Load = FALSE;

static float	effect_dx;
static float	effect_dy;

static BOOL						g_SettingFlag;				// 設定画面を開いていたらTRUE
static int						g_SettingMenu;				// 設定画面の選択項目

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSetting(void)
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
	g_TexNo = 0;
	g_VolumeNum = GetVolume();
	g_SEVolumeNum = GetSEVolume();
	g_SettingFlag = FALSE;
	g_SettingMenu = 0;

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
void UninitSetting(void)
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
void UpdateSetting(void)
{
	// 下キーを押したらメニューの選択項目を変更
	if (GetKeyboardTrigger(DIK_DOWN))
	{
		g_SettingMenu = (g_SettingMenu + 1) % MENU_MAX;		
		// タイトル画面なら表示しないのでスキップ
		if (g_SettingMenu % MENU_MAX == MENU_GO_TITLE && GetMode() == MODE_TITLE) 	g_SettingMenu = (g_SettingMenu + 1) % MENU_MAX;
		PlaySound(SOUND_LABEL_SE_select);
	}

	else if (IsButtonTriggered(0, BUTTON_DOWN))
	{
		g_SettingMenu = (g_SettingMenu + 1) % MENU_MAX;
		// タイトル画面なら表示しないのでスキップ
		if (g_SettingMenu % MENU_MAX == MENU_GO_TITLE && GetMode() == MODE_TITLE) 	g_SettingMenu = (g_SettingMenu + 1) % MENU_MAX;
		PlaySound(SOUND_LABEL_SE_select);
	}

	// 上キーを押したらメニューの選択項目を変更
	if (GetKeyboardTrigger(DIK_UP))
	{
		g_SettingMenu = (g_SettingMenu + MENU_MAX - 1) % MENU_MAX;
		// タイトル画面なら表示しないのでスキップ
		if (g_SettingMenu % MENU_MAX == MENU_GO_TITLE && GetMode() == MODE_TITLE) 	g_SettingMenu = (g_SettingMenu + MENU_MAX - 1) % MENU_MAX;
		PlaySound(SOUND_LABEL_SE_select);
	}

	else if (IsButtonTriggered(0, BUTTON_UP))
	{
		g_SettingMenu = (g_SettingMenu + MENU_MAX - 1) % MENU_MAX;
		// タイトル画面なら表示しないのでスキップ
		if (g_SettingMenu % MENU_MAX == MENU_GO_TITLE && GetMode() == MODE_TITLE) 	g_SettingMenu = (g_SettingMenu + MENU_MAX - 1) % MENU_MAX;
		PlaySound(SOUND_LABEL_SE_select);
	}

	switch (g_SettingMenu % MENU_MAX)
	{
	case MENU_BGM:
		// 左キーを押したらボリュームを下げる
		if (GetKeyboardTrigger(DIK_LEFT))
		{
			g_VolumeNum++;
			if (g_VolumeNum > VOLUME_MIN) g_VolumeNum = VOLUME_MIN;
			SetVolume(g_VolumeNum);
			//PauseSound();
			SetNowVolume(g_VolumeNum);
			PlayPauseSound();
			PlaySound(SOUND_LABEL_SE_select);
		}

		else if (IsButtonTriggered(0, BUTTON_LEFT))
		{
			g_VolumeNum++;
			if (g_VolumeNum > VOLUME_MIN) g_VolumeNum = VOLUME_MIN;
			SetVolume(g_VolumeNum);
			//PauseSound();
			SetNowVolume(g_VolumeNum);
			PlayPauseSound();
			PlaySound(SOUND_LABEL_SE_select);
		}

		// 右キーを押したらボリュームをあ上げる
		if (GetKeyboardTrigger(DIK_RIGHT))
		{
			g_VolumeNum--;
			if (g_VolumeNum < 0) g_VolumeNum = 0;
			SetVolume(g_VolumeNum);
			//PauseSound();
			SetNowVolume(g_VolumeNum);
			PlayPauseSound();
			PlaySound(SOUND_LABEL_SE_select);
		}

		else if (IsButtonTriggered(0, BUTTON_RIGHT))
		{
			g_VolumeNum--;
			if (g_VolumeNum < 0) g_VolumeNum = 0;
			SetVolume(g_VolumeNum);
			//PauseSound();
			SetNowVolume(g_VolumeNum);
			PlayPauseSound();
			PlaySound(SOUND_LABEL_SE_select);
		}

		break;

	case MENU_SE:
		// 左キーを押したらボリュームを下げる
		if (GetKeyboardTrigger(DIK_LEFT))
		{
			g_SEVolumeNum++;
			if (g_SEVolumeNum > VOLUME_MIN) g_SEVolumeNum = VOLUME_MIN;
			SetSEVolume(g_SEVolumeNum);
			PlaySound(SOUND_LABEL_SE_select);
		}

		else if (IsButtonTriggered(0, BUTTON_LEFT))
		{
			g_SEVolumeNum++;
			if (g_SEVolumeNum > VOLUME_MIN) g_SEVolumeNum = VOLUME_MIN;
			SetSEVolume(g_SEVolumeNum);
			PlaySound(SOUND_LABEL_SE_select);
		}

		// 右キーを押したらボリュームをあ上げる
		if (GetKeyboardTrigger(DIK_RIGHT))
		{
			g_SEVolumeNum--;
			if (g_SEVolumeNum < 0) g_SEVolumeNum = 0;
			SetSEVolume(g_SEVolumeNum);
			PlaySound(SOUND_LABEL_SE_select);
		}

		else if (IsButtonTriggered(0, BUTTON_RIGHT))
		{
			g_SEVolumeNum--;
			if (g_SEVolumeNum < 0) g_SEVolumeNum = 0;
			SetSEVolume(g_SEVolumeNum);
			PlaySound(SOUND_LABEL_SE_select);
		}
		break;

	case MENU_RETRUN:
		// 設定画面を閉じる
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			ResetSettingMenu();
			g_SettingFlag = FALSE;
			PlaySound(SOUND_LABEL_SE_enter);
		}
		// ゲームパッドで入力処理
		else if (IsButtonTriggered(0, BUTTON_B))
		{
			ResetSettingMenu();
			g_SettingFlag = FALSE;
			PlaySound(SOUND_LABEL_SE_enter);
		}
		break;

	case MENU_GO_TITLE:
		// タイトルへ戻る
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			SetFade(FADE_OUT, MODE_TITLE);
			g_SettingFlag = FALSE;
			PlaySound(SOUND_LABEL_SE_enter);
		}
		// ゲームパッドで入力処理
		else if (IsButtonTriggered(0, BUTTON_B))
		{
			SetFade(FADE_OUT, MODE_TITLE);
			g_SettingFlag = FALSE;
			PlaySound(SOUND_LABEL_SE_enter);
		}
		break;
	}


#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Player:↑ → ↓ ←　Space\n");

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSetting(void)
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

	 //ボードを描画
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_BG]);

	float px = TEXTURE_X;		// 表示位置X
	float py = TEXTURE_Y;		// 表示位置Y
	float pw = TEXTURE_WIDTH;		// 表示幅
	float ph = TEXTURE_HEIGHT;		// 表示高さ

	float tw = 1.0f;	// テクスチャの幅
	float th = 1.0f;	// テクスチャの高さ
	float tx = 0.0f;	// テクスチャの左上X座標
	float ty = 0.0f;	// テクスチャの左上Y座標

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLeftTop(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th
	);
	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// minを描画
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_VOL_MIN]);

	px = TEXTURE_LOGO_VOLMIN_X;
	py = TEXTURE_LOGO_VOLMIN_Y;
	pw = TEXTURE_LOGO_VOLMIN_WIDTH;
	ph = TEXTURE_LOGO_VOLMIN_HEIGHT;

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// maxを描画
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_VOL_MAX]);

	px = TEXTURE_LOGO_VOLMAX_X;

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// volumeを描画
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_BGM]);

	px = TEXTURE_LOGO_BGM_X;
	py = TEXTURE_LOGO_BGM_Y;
	pw = TEXTURE_LOGO_BGM_WIDTH;
	ph = TEXTURE_LOGO_BGM_HEIGHT;

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// volumeを描画
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_SE]);

	py = TEXTURE_LOGO_SE_Y;

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// backを描画
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_BACK]);

	px = TEXTURE_BACK_X;
	py = TEXTURE_BACK_Y;

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// gotitleを描画
	if (GetMode() != MODE_TITLE)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_GOTITLE]);

		py = TEXTURE_GOTITLE_Y;

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// カーソルを描画
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_CURSOR]);

	px = TEXTURE_CURSOR_X;
	py = TEXTURE_CURSOR_Y;
	pw = TEXTURE_CURSOR_WIDTH;
	ph = TEXTURE_CURSOR_HEIGHT;

	// 選択位置によって座標を切り替え
	if (g_SettingMenu % MENU_MAX == MENU_GO_TITLE)
	{
		py = TEXTURE_GOTITLE_Y;
	}

	if (g_SettingMenu % MENU_MAX == MENU_BGM)
	{
		px = TEXTURE_LOGO_BGM_X;
		py = TEXTURE_LOGO_BGM_Y;
	}

	if (g_SettingMenu % MENU_MAX == MENU_SE)
	{
		px = TEXTURE_LOGO_BGM_X;
		py = TEXTURE_LOGO_SE_Y;
	}

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);


	// ボリューム分割の線を描画
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_VOLSIZE]);

	px = TEXTURE_VOLSIZE_X;
	py = TEXTURE_VOLSIZE_Y;
	pw = TEXTURE_VOLSIZE_WIDTH;
	ph = TEXTURE_VOLSIZE_HEIGHT;

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// ボリュームのバーを描画
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_BAR]);

	py = TEXTURE_BAR_Y;
	pw = pw * (1.0f - (float)g_VolumeNum / VOLUME_MIN);
	ph = TEXTURE_BAR_HEIGHT;

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// バーの先頭の丸を描画
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_CIRCLE]);

	px += pw - TEXTURE_CIRCLE_WIDTH * 0.5f;
	py = TEXTURE_CIRCLE_Y;
	pw = TEXTURE_CIRCLE_WIDTH;
	ph = TEXTURE_CIRCLE_HEIGHT;

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// ボリューム分割の線を描画
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_VOLSIZE]);

	px = TEXTURE_VOLSIZE_X;
	py = TEXTURE_VOLSIZE_Y + TEXTURE_LOGO_BGM_HEIGHT * 1.5;
	pw = TEXTURE_VOLSIZE_WIDTH;
	ph = TEXTURE_VOLSIZE_HEIGHT;

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// ボリュームのバーを描画
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_BAR]);

	py = TEXTURE_BAR_Y + TEXTURE_LOGO_BGM_HEIGHT * 1.5;
	pw = pw * (1.0f - (float)g_SEVolumeNum / VOLUME_MIN);
	ph = TEXTURE_BAR_HEIGHT;

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// バーの先頭の丸を描画
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_CIRCLE]);

	px += pw - TEXTURE_CIRCLE_WIDTH * 0.5f;
	py = TEXTURE_CIRCLE_Y + TEXTURE_LOGO_BGM_HEIGHT * 1.5;
	pw = TEXTURE_CIRCLE_WIDTH;
	ph = TEXTURE_CIRCLE_HEIGHT;

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
}

// 設定画面フラグの取得
int GetSettingFlag(void)
{
	return g_SettingFlag;
}

// 設定画面フラグを立てる
void SetSettingFlag(void)
{
	g_SettingFlag = TRUE;
}

// 設定画面の選択番号をリセットする
void ResetSettingMenu(void)
{
	g_SettingMenu = 0;
}