//=============================================================================
//
// �T�E���h���� [sound.h]
//
//=============================================================================
#pragma once
#define XAUDIO2_HELPER_FUNCTIONS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "xaudio2.h"						// �T�E���h�����ŕK�v

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_op,	// titleBGM
	SOUND_LABEL_BGM_clear,	// clear BGM
	SOUND_LABEL_BGM_field,	// field BGM
	SOUND_LABEL_BGM_gameover,	// gameover BGM
	SOUND_LABEL_SE_fire,		// �΂̒e������
	SOUND_LABEL_SE_hassha,	// ���ˉ�
	SOUND_LABEL_SE_jump,	// �W�����v��
	SOUND_LABEL_SE_enemy_death,		// �G�l�~�[��|����
	SOUND_LABEL_SE_hit,			// �_���[�W��
	SOUND_LABEL_SE_spone,		// �{�X�o���̉�
	SOUND_LABEL_SE_select,		// �I����
	SOUND_LABEL_SE_enter,		// �S�[�����̉�
	SOUND_LABEL_SE_shot,		// ���U����
	SOUND_LABEL_SE_slash,		// ���U����

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
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
