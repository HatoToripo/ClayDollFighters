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
	SOUND_LABEL_BGM_field0,	// field0 BGM
	SOUND_LABEL_BGM_field1,	// field1 BGM
	SOUND_LABEL_BGM_field2,	// field2 BGM
	SOUND_LABEL_BGM_field3,	// field3 BGM
	SOUND_LABEL_BGM_clear,	// clear BGM
	SOUND_LABEL_SE_fire,		// �΂̒e������
	SOUND_LABEL_SE_hassha,	// ���ˉ�
	SOUND_LABEL_SE_jump,	// �W�����v��
	SOUND_LABEL_SE_teki_taosu,		// �G�l�~�[��|����
	SOUND_LABEL_SE_death,		// �v���C���[���������Ƃ��̉�
	SOUND_LABEL_SE_goal,		// �S�[�����̉�
	SOUND_LABEL_SE_select,		// �S�[�����̉�
	SOUND_LABEL_SE_enter,		// �S�[�����̉�

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
