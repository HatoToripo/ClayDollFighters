//=============================================================================
//
// 設定画面処理 [setting.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define VOLUME_MIN					(4)					// ボリュームの最小値



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSetting(void);
void UninitSetting(void);
void UpdateSetting(void);
void DrawSetting(void);

int GetVolume(void);

int GetSettingFlag(void);
void SetSettingFlag(void);
void ResetSettingMenu(void);
