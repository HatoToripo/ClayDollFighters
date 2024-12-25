//=============================================================================
//
// �ݒ��ʏ��� [setting.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define VOLUME_MIN					(4)					// �{�����[���̍ŏ��l



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSetting(void);
void UninitSetting(void);
void UpdateSetting(void);
void DrawSetting(void);

int GetVolume(void);

int GetSettingFlag(void);
void SetSettingFlag(void);
void ResetSettingMenu(void);
