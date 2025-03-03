//=============================================================================
//
// エディタープレイヤー処理 [editplayer.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"
#include "player.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define EDITPLAYER_MAX		(1)

//*****************************************************************************
// 構造体定義
//*****************************************************************************
class EDITPLAYER
{
public:
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	BOOL				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	int					shadowIdx;			// 影のインデックス番号

	BOOL				use;

	float				size;

	// 階層アニメーション用のメンバー変数
	float				time[PLAYER_ANIM_MAX];				// 線形補間用
	int					tblNo[PLAYER_ANIM_MAX];				// 行動データのテーブル番号
	int					tblMax[PLAYER_ANIM_MAX];			// そのテーブルのデータ数
	int					animNum;							// 現在のアニメーション番号

	BOOL				jump;			// ジャンプフラグ
	int					jumpCnt;		// ジャンプ中のカウント

	// 親は、NULL、子供は親のアドレスを入れる
	EDITPLAYER				*parent;			// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	// クォータニオン
	XMFLOAT4			Quaternion;

	XMFLOAT3			UpVector;			// 自分が立っている所

	void Animation(int animNum);
	void Animation(int animNum1, int animNum2);
	void DecHP(int atk);
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEditPlayer(void);
void UninitEditPlayer(void);
void UpdateEditPlayer(void);
void DrawEditPlayer(void);

EDITPLAYER *GetEditPlayer(void);
EDITPLAYER* GetEditPlayerParts(void);

//void PlayerAnimation(int animNum, int i);
//void PlayerAnimationBlend(int animNum1, int animNum2, int i);

INTERPOLATION_DATA* GetStopTbl(int animNum);
INTERPOLATION_DATA* GetMoveTbl(int animNum);
INTERPOLATION_DATA* GetDashTbl(int animNum);
