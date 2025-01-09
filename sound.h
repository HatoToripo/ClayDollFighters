//=============================================================================
//
// サウンド処理 [sound.h]
//
//=============================================================================
#pragma once
#define XAUDIO2_HELPER_FUNCTIONS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_op,	// titleBGM
	SOUND_LABEL_BGM_clear,	// clear BGM
	SOUND_LABEL_BGM_field,	// field BGM
	SOUND_LABEL_BGM_gameover,	// gameover BGM
	SOUND_LABEL_SE_fire,		// 火の弾生成音
	SOUND_LABEL_SE_hassha,	// 発射音
	SOUND_LABEL_SE_jump,	// ジャンプ音
	SOUND_LABEL_SE_enemy_death,		// エネミーを倒す音
	SOUND_LABEL_SE_hit,			// ダメージ音
	SOUND_LABEL_SE_spone,		// ボス出現の音
	SOUND_LABEL_SE_select,		// 選択音
	SOUND_LABEL_SE_enter,		// ゴール時の音
	SOUND_LABEL_SE_shot,		// 槍攻撃音
	SOUND_LABEL_SE_slash,		// 剣攻撃音

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

void SetNowVolume(int volumeNum);
void PauseSound(void);
void PlayPauseSound(void);
int GetVolume(void);
void SetVolume(int volume);
int GetSEVolume(void);
void SetSEVolume(int SEVolume);
int GetLabel(void);
