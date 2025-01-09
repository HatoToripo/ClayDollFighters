//=============================================================================
//
// �Q�[����ʏ��� [game.cpp]
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
// �}�N����`
//*****************************************************************************



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void CheckHit(void);



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	// �|�[�YON/OFF


//=============================================================================
// ����������
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// �t�B�[���h�̏�����
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 100, 13.0f, 13.0f);

	// ���C�g��L����	// �e�̏���������
	InitShadow();

	// �v���C���[�̏�����
	InitPlayer();

	// �G�l�~�[�̏�����
	InitEnemy();

	// �ǂ̏�����
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	// ��(�����p�̔�����)
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	// �؂𐶂₷
	//InitTree();

	// ���m�}�[�N�̏�����
	InitMark();

	// ���}�[�N�̏�����
	InitArrow();

	// �e�̏�����
	InitBullet();

	// �X�R�A�̏�����
	InitScore();

	// �`���[�g���A���̏�����
	InitTutorial();

	// �^�C���̏�����
	InitTime();

	// UI�̏�����
	InitUI();

	// �p�[�e�B�N���̏�����
	InitParticle();

	// �G�t�F�N�g�̏�����
	InitEffect();

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGame(void)
{
	// �G�t�F�N�g�̏I������
	UninitEffect();

	// �p�[�e�B�N���̏I������
	UninitParticle();

	// UI�̏I������
	UninitUI();

	// �^�C���̏I������
	UninitTime();

	// �`���[�g���A���̏I������
	UninitTutorial();

	// �X�R�A�̏I������
	UninitScore();

	// �e�̏I������
	UninitBullet();

	// ���}�[�N�̏I������
	UninitArrow();

	// ���m�}�[�N�̏I������
	UninitMark();

	// �؂̏I������
	//UninitTree();

	// �ǂ̏I������
	UninitMeshWall();

	// �n�ʂ̏I������
	UninitMeshField();

	// �G�l�~�[�̏I������
	UninitEnemy();

	// �v���C���[�̏I������
	UninitPlayer();

	// �e�̏I������
	UninitShadow();

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	//if (GetKeyboardTrigger(DIK_V))
	//{
	//	g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
	//	SetViewPort(g_ViewPortType_Game);
	//}

	//if (GetKeyboardTrigger(DIK_P))
	//{
	//	g_bPause = g_bPause ? FALSE : TRUE;
	//}


#endif

	if(g_bPause == FALSE)
		return;

	// �`���[�g���A�����Ȃ�X�V���Ȃ�
	if (GetTutorialFlg() == FALSE)
	{
		// �n�ʏ����̍X�V
		UpdateMeshField();

		// �v���C���[�̍X�V����
		UpdatePlayer();

		// �G�l�~�[�̍X�V����
		UpdateEnemy();

		// �Ǐ����̍X�V
		UpdateMeshWall();

		// �؂̍X�V����
		//UpdateTree();

		// ���m�}�[�N�̍X�V����
		UpdateMark();

		// ���}�[�N�̍X�V����
		UpdateArrow();

		// �e�̍X�V����
		UpdateBullet();

		// �p�[�e�B�N���̍X�V����
		UpdateParticle();

		// �G�t�F�N�g�̍X�V����
		UpdateEffect();

		// �e�̍X�V����
		UpdateShadow();

		// �����蔻�菈��
		CheckHit();

		// �X�R�A�̍X�V����
		UpdateScore();

		// �^�C���̍X�V����
		UpdateTime();

		// UI�̍X�V����
		UpdateUI();
	}
	else
	{
		// �`���[�g���A����ʂ̍X�V
		UpdateTutorial();
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGame0(void)
{
	// 3D�̕���`�悷�鏈��
	// �n�ʂ̕`�揈��
	DrawMeshField();

	// �e�̕`�揈��
	DrawShadow();

	// �G�l�~�[�̕`�揈��
	DrawEnemy();

	// �v���C���[�̕`�揈��
	DrawPlayer();

	// �e�̕`�揈��
	DrawBullet();

	// �ǂ̕`�揈��
	DrawMeshWall();

	// �؂̕`�揈��
	//DrawTree();

	// ���m�}�[�N�̕`�揈��
	DrawMark();

	// ���}�[�N�̕`�揈��
	DrawArrow();

	// �p�[�e�B�N���̕`�揈��
	DrawParticle();

	// �G�t�F�N�g�̕`�揈��
	DrawEffect();

	// 2D�̕���`�悷�鏈��
	// Z��r�Ȃ�
	SetDepthEnable(FALSE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	if (GetTutorialFlg() == TRUE)
	{
		// �`���[�g���A���̕`�揈��
		DrawTutorial();
	}
	else
	{
		// �X�R�A�̕`�揈��
		//DrawScore();

		// �^�C���̕`�揈��
		DrawTime();

		// UI�̕`�揈��
		DrawUI();
	}
	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// Z��r����
	SetDepthEnable(TRUE);
}


void DrawGame(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// �f�o�b�O�\��
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	// �v���C���[���_
	pos = GetPlayer()->pos;
	pos.y = 0.0f;			// �J����������h�����߂ɃN���A���Ă���
	ENEMY* boss = GetBoss();
	if (GetBossFlg() == TRUE && GetBossSponeCnt() < BOSS_SPONE_TIME)
	{
		pos = boss->pos;
		pos.y = 0.0f;
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
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawGame0();

		// �G�l�~�[���_
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();

		// �G�l�~�[���_
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}

}


//=============================================================================
// �����蔻�菈��
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();		// �G�l�~�[�̃|�C���^�[��������
	PLAYER *player = GetPlayer();	// �v���C���[�̃|�C���^�[��������

	// �G�ƃv���C���[�L����
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		//�G�̗L���t���O���`�F�b�N����
		if (enemy[i].use == FALSE) continue;

		// ���G����`�F�b�N
		if (player->colCnt == 0)
		{
			//BC�̓����蔻��
			if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size))
			{
				// �v���C���[�Ƀ_���[�W
				player->DecHP(enemy[i].atkVal);
			}
		}
	}

	ENEMY* boss = GetBoss();
	if (GetBossFlg() == FALSE)
	{
		// �G�l�~�[���S�����S�������ԑJ��
		int enemy_count = 0;
		for (int i = 0; i < ENEMY_MAX; i++)
		{
			if (enemy[i].use == FALSE) continue;
			enemy_count++;
		}
		// �G�l�~�[���O�C�H
		if (enemy_count == 0 || GetTime() == 0)
		{
			// �G�l�~�[���폜
			ReleaseEnemy();

			for (int i = 0; i < BOSS_MAX; i++)
			{
				// �{�X��z�u
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

			// �{�X��0�C�H
			if (boss_count == 0)
			{
				SetFade(FADE_OUT, MODE_RESULT);
			}
		}
	}
}


