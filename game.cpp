//=============================================================================
//
// ゲーム画面処理 [game.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"

#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "tree.h"
#include "mark.h"
#include "arrow.h"
#include "radar.h"
#include "bullet.h"
#include "score.h"
#include "tutorial.h"
#include "time.h"
#include "ui.h"
#include "particle.h"
#include "effect.h"
#include "collision.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ANIM_CNT		(4)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void CheckHit(void);



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	// ポーズON/OFF

static int g_PlayerNum = 0;
static int g_TimeCnt = 0;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// フィールドの初期化
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 100, 13.0f, 13.0f);

	// ライトを有効化	// 影の初期化処理
	InitShadow();

	// プレイヤーの初期化
	InitPlayer();

	// エネミーの初期化
	InitEnemy();

	// 壁の初期化
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	// 壁(裏側用の半透明)
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	// 木を生やす
	//InitTree();

	// 検知マークの初期化
	InitMark();

	// 矢印マークの初期化
	InitArrow();

	// 検知範囲の初期化
	InitRadar();

	// 弾の初期化
	InitBullet();

	// スコアの初期化
	InitScore();

	// チュートリアルの初期化
	InitTutorial();

	// タイムの初期化
	InitTime();

	// UIの初期化
	InitUI();

	// パーティクルの初期化
	InitParticle();

	// エフェクトの初期化
	InitEffect();

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGame(void)
{
	// エフェクトの終了処理
	UninitEffect();

	// パーティクルの終了処理
	UninitParticle();

	// UIの終了処理
	UninitUI();

	// タイムの終了処理
	UninitTime();

	// チュートリアルの終了処理
	UninitTutorial();

	// スコアの終了処理
	UninitScore();

	// 弾の終了処理
	UninitBullet();

	// 検知範囲の終了処理
	UninitRadar();

	// 矢印マークの終了処理
	UninitArrow();

	// 検知マークの終了処理
	UninitMark();

	// 木の終了処理
	//UninitTree();

	// 壁の終了処理
	UninitMeshWall();

	// 地面の終了処理
	UninitMeshField();

	// エネミーの終了処理
	UninitEnemy();

	// プレイヤーの終了処理
	UninitPlayer();

	// 影の終了処理
	UninitShadow();

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = TYPE_LEFT_HALF_SCREEN;
		SetViewPort(g_ViewPortType_Game);
	}

	else if(GetKeyboardTrigger(DIK_F))
	{
		g_ViewPortType_Game = TYPE_FULL_SCREEN;
		SetViewPort(g_ViewPortType_Game);
	}


	//if (GetKeyboardTrigger(DIK_P))
	//{
	//	g_bPause = g_bPause ? FALSE : TRUE;
	//}


#endif

	if(g_bPause == FALSE)
		return;

	// チュートリアル中なら更新しない
	if (GetTutorialFlg() == FALSE)
	{
		g_TimeCnt = (g_TimeCnt + 1) % ANIM_CNT;
		if (g_ViewPortType_Game != TYPE_FULL_SCREEN && g_TimeCnt != 0)
			return;

		// 地面処理の更新
		UpdateMeshField();

		// プレイヤーの更新処理
		UpdatePlayer();

		// エネミーの更新処理
		UpdateEnemy();

		// 壁処理の更新
		UpdateMeshWall();

		// 木の更新処理
		//UpdateTree();

		// 検知マークの更新処理
		UpdateMark();

		// 矢印マークの更新処理
		UpdateArrow();

		// 検知範囲の更新処理
		UpdateRadar();

		// 弾の更新処理
		UpdateBullet();

		// パーティクルの更新処理
		UpdateParticle();

		// エフェクトの更新処理
		UpdateEffect();

		// 影の更新処理
		UpdateShadow();

		// 当たり判定処理
		CheckHit();

		// スコアの更新処理
		UpdateScore();

		// タイムの更新処理
		UpdateTime();

		// UIの更新処理
		UpdateUI();
	}
	else
	{
		// チュートリアル画面の更新
		UpdateTutorial();
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGame0(void)
{
	// 3Dの物を描画する処理
	// 地面の描画処理
	DrawMeshField();

	// 影の描画処理
	DrawShadow();

	// エネミーの描画処理
	DrawEnemy();

	// プレイヤーの描画処理
	DrawPlayer();

	// 弾の描画処理
	DrawBullet();

	// 壁の描画処理
	DrawMeshWall();

	// 木の描画処理
	//DrawTree();

	// 検知マークの描画処理
	DrawMark();

	// 矢印マークの描画処理
	DrawArrow();

	// 検知範囲の描画処理
	DrawRadar();

	// パーティクルの描画処理
	DrawParticle();

	// エフェクトの描画処理
	DrawEffect();

	// 2Dの物を描画する処理
	// Z比較なし
	SetDepthEnable(FALSE);

	// ライティングを無効
	SetLightEnable(FALSE);

	if (GetTutorialFlg() == TRUE)
	{
		// チュートリアルの描画処理
		DrawTutorial();
	}
	else
	{
		// スコアの描画処理
		//DrawScore();

		// タイムの描画処理
		DrawTime();

		// UIの描画処理
		DrawUI();
	}
	// ライティングを有効に
	SetLightEnable(TRUE);

	// Z比較あり
	SetDepthEnable(TRUE);
}


void DrawGame(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// デバッグ表示
	//PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	PLAYER* player = GetPlayer();
	CAMERA* camera = GetCamera();
	// プレイヤー視点
	pos = player->pos;
	pos.y = 5.0f;			// カメラ酔いを防ぐためにクリアしている
	ENEMY* boss = GetBoss();
	if (GetBossFlg() == TRUE && GetBossSponeCnt() < BOSS_SPONE_TIME)
	{
		pos = boss->pos;
		pos.y = 5.0f;
		AddBossSponeCnt();
		if (GetBossSponeCnt() >= BOSS_SPONE_TIME / 3 * 2)
		{
			SetBoss();
		}
	}

	SetCameraAT(pos);
	SetCamera();

	switch(g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			player[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		}
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		g_PlayerNum = 0;
		pos = player->pos;
		pos.y = 0.0f;			// カメラ酔いを防ぐためにクリアしている
		pos.y -= 8.0f;			// カメラ酔いを防ぐためにクリアしている
		pos.z += 30.0f;			// カメラ酔いを防ぐためにクリアしている
		player->scl = XMFLOAT3(2.0f, 1.0f, 2.0f);
		SetCameraAT(pos);
		camera->pos.y = player->pos.y + 5.0f;
		SetCamera();

		DrawGame0();

		//// エネミー視点
		//pos = GetEnemy()->pos;
		//pos.y = 0.0f;
		//SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		g_PlayerNum = 1;
		(player + 1)->scl = XMFLOAT3(2.0f, 1.0f, 2.0f);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		g_PlayerNum = 0;
		pos = GetPlayer()->pos;
		pos.y = 0.0f;			// カメラ酔いを防ぐためにクリアしている
		pos.z += 15.0f;			// カメラ酔いを防ぐためにクリアしている
		SetCameraAT(pos);
		SetCamera();

		DrawGame0();

		//// エネミー視点
		//pos = GetEnemy()->pos;
		//pos.y = 0.0f;
		//SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		g_PlayerNum = 1;
		DrawGame0();
		break;

	}

}


//=============================================================================
// 当たり判定処理
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();		// エネミーのポインターを初期化
	PLAYER *player = GetPlayer();	// プレイヤーのポインターを初期化

	// 敵とプレイヤーキャラ
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		//敵の有効フラグをチェックする
		if (enemy[i].use == FALSE) continue;

		// 無敵判定チェック
		if (player->colCnt == 0)
		{
			//BCの当たり判定
			if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size))
			{
				// プレイヤーにダメージ
				player->DecHP(enemy[i].atkVal);
			}
		}
	}

	ENEMY* boss = GetBoss();
	if (GetBossFlg() == FALSE)
	{
		// エネミーが全部死亡したら状態遷移
		int enemy_count = 0;
		for (int i = 0; i < ENEMY_MAX; i++)
		{
			if (enemy[i].use == FALSE) continue;
			enemy_count++;
		}
		// エネミーが０匹？
		if (enemy_count == 0 || GetTime() == 0)
		{
			// エネミーを削除
			ReleaseEnemy();

			for (int i = 0; i < BOSS_MAX; i++)
			{
				// ボスを配置
				AddBossSponeCnt();
				SetEffect(XMFLOAT3(boss->pos.x, boss->pos.y - ENEMY_OFFSET_Y * EFFECT_HEIGHT / 15.0f, boss->pos.z), EFFECT_WIDTH, EFFECT_HEIGHT, EFFECT_SPONE);
				PlaySound(SOUND_LABEL_SE_spone);
			}
		}
	}

	else
	{
		if (GetBossSponeCnt() >= BOSS_SPONE_TIME)
		{
			int boss_count = 0;
			for (int i = 0; i < BOSS_MAX; i++)
			{
				if (boss[i].use == FALSE) continue;
				boss_count++;
			}

			// ボスが0匹？
			if (boss_count == 0)
			{
				SetFade(FADE_OUT, MODE_RESULT);
			}
		}
	}
}

int GetViewPortTypeGame(void)
{
	return g_ViewPortType_Game;
}

int GetPlayerNum(void)
{
	return g_PlayerNum;
}