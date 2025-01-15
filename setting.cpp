//=============================================================================
//
// �ݒ��ʏ��� [setting.cpp]
// Author : �����\��
//
//=============================================================================
#include "setting.h"
#include "sprite.h"
#include "sound.h"
#include "input.h"
#include "fade.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_X					(0.0f)		// �w�i�̉���
#define TEXTURE_Y					(0.0f)		// �w�i�̏c��
#define TEXTURE_WIDTH				(SCREEN_WIDTH - TEXTURE_X * 2)		// �{�[�h�̉���
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT - TEXTURE_Y * 2)	// �{�[�h�̏c��

#define TEXTURE_LOGO_VOLMIN_WIDTH			(50.0f)		// �{�����[�����S�̉���
#define TEXTURE_LOGO_VOLMIN_HEIGHT			(25.0f)		// �{�����[�����S�̏c��

#define TEXTURE_LOGO_VOLMIN_X		(350.0f)		// MIN��X���W
#define TEXTURE_LOGO_VOLMIN_Y		(75.0f)			// MIN��Y���W
#define TEXTURE_LOGO_VOLMAX_X		(750.0f)		// MAX��X���W

#define TEXTURE_LOGO_BGM_X		(175.0f)			// �{�����[�����S��X���W
#define TEXTURE_LOGO_BGM_Y		(TEXTURE_LOGO_VOLMIN_Y + TEXTURE_LOGO_VOLMIN_HEIGHT * 1.5)		// �{�����[�����S��Y���W
#define TEXTURE_LOGO_BGM_WIDTH	(200.0f)		// �{�����[�����S��Y���W
#define TEXTURE_LOGO_BGM_HEIGHT	(50.0f)		// �{�����[�����S��Y���W

#define TEXTURE_LOGO_SE_Y		(TEXTURE_LOGO_BGM_Y + TEXTURE_LOGO_BGM_HEIGHT * 1.5)		// �{�����[�����S��Y���W

#define TEXTURE_VOLSIZE_X			(TEXTURE_LOGO_VOLMIN_X + TEXTURE_LOGO_VOLMIN_WIDTH * 0.5 )			// �{�����[���Ԋu��X���W
#define TEXTURE_VOLSIZE_Y			(TEXTURE_LOGO_VOLMIN_Y + TEXTURE_LOGO_VOLMIN_HEIGHT * 1.5)			// �{�����[���Ԋu��Y���W
#define TEXTURE_VOLSIZE_WIDTH		(TEXTURE_LOGO_VOLMAX_X - TEXTURE_LOGO_VOLMIN_X)					// �{�����[���Ԋu�̉���
#define TEXTURE_VOLSIZE_HEIGHT		(50.0f)															// �{�����[���Ԋu�̏c��

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
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

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


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�
static int						g_VolumeNum;				// �{�����[���ɃZ�b�g����l
static int						g_SEVolumeNum;				// SE�{�����[���ɃZ�b�g����l

static float	alpha;
static BOOL	flag_alpha;

static BOOL						g_Load = FALSE;

static float	effect_dx;
static float	effect_dy;

static BOOL						g_SettingFlag;				// �ݒ��ʂ��J���Ă�����TRUE
static int						g_SettingMenu;				// �ݒ��ʂ̑I������

//=============================================================================
// ����������
//=============================================================================
HRESULT InitSetting(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
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


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �ϐ��̏�����
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
// �I������
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
// �X�V����
//=============================================================================
void UpdateSetting(void)
{
	// ���L�[���������烁�j���[�̑I�����ڂ�ύX
	if (GetKeyboardTrigger(DIK_DOWN))
	{
		g_SettingMenu = (g_SettingMenu + 1) % MENU_MAX;		
		// �^�C�g����ʂȂ�\�����Ȃ��̂ŃX�L�b�v
		if (g_SettingMenu % MENU_MAX == MENU_GO_TITLE && GetMode() == MODE_TITLE) 	g_SettingMenu = (g_SettingMenu + 1) % MENU_MAX;
		PlaySound(SOUND_LABEL_SE_select);
	}

	else if (IsButtonTriggered(0, BUTTON_DOWN))
	{
		g_SettingMenu = (g_SettingMenu + 1) % MENU_MAX;
		// �^�C�g����ʂȂ�\�����Ȃ��̂ŃX�L�b�v
		if (g_SettingMenu % MENU_MAX == MENU_GO_TITLE && GetMode() == MODE_TITLE) 	g_SettingMenu = (g_SettingMenu + 1) % MENU_MAX;
		PlaySound(SOUND_LABEL_SE_select);
	}

	// ��L�[���������烁�j���[�̑I�����ڂ�ύX
	if (GetKeyboardTrigger(DIK_UP))
	{
		g_SettingMenu = (g_SettingMenu + MENU_MAX - 1) % MENU_MAX;
		// �^�C�g����ʂȂ�\�����Ȃ��̂ŃX�L�b�v
		if (g_SettingMenu % MENU_MAX == MENU_GO_TITLE && GetMode() == MODE_TITLE) 	g_SettingMenu = (g_SettingMenu + MENU_MAX - 1) % MENU_MAX;
		PlaySound(SOUND_LABEL_SE_select);
	}

	else if (IsButtonTriggered(0, BUTTON_UP))
	{
		g_SettingMenu = (g_SettingMenu + MENU_MAX - 1) % MENU_MAX;
		// �^�C�g����ʂȂ�\�����Ȃ��̂ŃX�L�b�v
		if (g_SettingMenu % MENU_MAX == MENU_GO_TITLE && GetMode() == MODE_TITLE) 	g_SettingMenu = (g_SettingMenu + MENU_MAX - 1) % MENU_MAX;
		PlaySound(SOUND_LABEL_SE_select);
	}

	switch (g_SettingMenu % MENU_MAX)
	{
	case MENU_BGM:
		// ���L�[����������{�����[����������
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

		// �E�L�[����������{�����[�������グ��
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
		// ���L�[����������{�����[����������
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

		// �E�L�[����������{�����[�������グ��
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
		// �ݒ��ʂ����
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			ResetSettingMenu();
			g_SettingFlag = FALSE;
			PlaySound(SOUND_LABEL_SE_enter);
		}
		// �Q�[���p�b�h�œ��͏���
		else if (IsButtonTriggered(0, BUTTON_B))
		{
			ResetSettingMenu();
			g_SettingFlag = FALSE;
			PlaySound(SOUND_LABEL_SE_enter);
		}
		break;

	case MENU_GO_TITLE:
		// �^�C�g���֖߂�
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			SetFade(FADE_OUT, MODE_TITLE);
			g_SettingFlag = FALSE;
			PlaySound(SOUND_LABEL_SE_enter);
		}
		// �Q�[���p�b�h�œ��͏���
		else if (IsButtonTriggered(0, BUTTON_B))
		{
			SetFade(FADE_OUT, MODE_TITLE);
			g_SettingFlag = FALSE;
			PlaySound(SOUND_LABEL_SE_enter);
		}
		break;
	}


#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("Player:�� �� �� ���@Space\n");

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSetting(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	 //�{�[�h��`��
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_BG]);

	float px = TEXTURE_X;		// �\���ʒuX
	float py = TEXTURE_Y;		// �\���ʒuY
	float pw = TEXTURE_WIDTH;		// �\����
	float ph = TEXTURE_HEIGHT;		// �\������

	float tw = 1.0f;	// �e�N�X�`���̕�
	float th = 1.0f;	// �e�N�X�`���̍���
	float tx = 0.0f;	// �e�N�X�`���̍���X���W
	float ty = 0.0f;	// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLeftTop(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th
	);
	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// min��`��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_VOL_MIN]);

	px = TEXTURE_LOGO_VOLMIN_X;
	py = TEXTURE_LOGO_VOLMIN_Y;
	pw = TEXTURE_LOGO_VOLMIN_WIDTH;
	ph = TEXTURE_LOGO_VOLMIN_HEIGHT;

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// max��`��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_VOL_MAX]);

	px = TEXTURE_LOGO_VOLMAX_X;

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// volume��`��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_BGM]);

	px = TEXTURE_LOGO_BGM_X;
	py = TEXTURE_LOGO_BGM_Y;
	pw = TEXTURE_LOGO_BGM_WIDTH;
	ph = TEXTURE_LOGO_BGM_HEIGHT;

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// volume��`��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_SE]);

	py = TEXTURE_LOGO_SE_Y;

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// back��`��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_BACK]);

	px = TEXTURE_BACK_X;
	py = TEXTURE_BACK_Y;

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// gotitle��`��
	if (GetMode() != MODE_TITLE)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_GOTITLE]);

		py = TEXTURE_GOTITLE_Y;

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �J�[�\����`��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_CURSOR]);

	px = TEXTURE_CURSOR_X;
	py = TEXTURE_CURSOR_Y;
	pw = TEXTURE_CURSOR_WIDTH;
	ph = TEXTURE_CURSOR_HEIGHT;

	// �I���ʒu�ɂ���č��W��؂�ւ�
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

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);


	// �{�����[�������̐���`��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_VOLSIZE]);

	px = TEXTURE_VOLSIZE_X;
	py = TEXTURE_VOLSIZE_Y;
	pw = TEXTURE_VOLSIZE_WIDTH;
	ph = TEXTURE_VOLSIZE_HEIGHT;

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// �{�����[���̃o�[��`��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_BAR]);

	py = TEXTURE_BAR_Y;
	pw = pw * (1.0f - (float)g_VolumeNum / VOLUME_MIN);
	ph = TEXTURE_BAR_HEIGHT;

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// �o�[�̐擪�̊ۂ�`��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_CIRCLE]);

	px += pw - TEXTURE_CIRCLE_WIDTH * 0.5f;
	py = TEXTURE_CIRCLE_Y;
	pw = TEXTURE_CIRCLE_WIDTH;
	ph = TEXTURE_CIRCLE_HEIGHT;

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// �{�����[�������̐���`��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_VOLSIZE]);

	px = TEXTURE_VOLSIZE_X;
	py = TEXTURE_VOLSIZE_Y + TEXTURE_LOGO_BGM_HEIGHT * 1.5;
	pw = TEXTURE_VOLSIZE_WIDTH;
	ph = TEXTURE_VOLSIZE_HEIGHT;

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// �{�����[���̃o�[��`��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_BAR]);

	py = TEXTURE_BAR_Y + TEXTURE_LOGO_BGM_HEIGHT * 1.5;
	pw = pw * (1.0f - (float)g_SEVolumeNum / VOLUME_MIN);
	ph = TEXTURE_BAR_HEIGHT;

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// �o�[�̐擪�̊ۂ�`��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_CIRCLE]);

	px += pw - TEXTURE_CIRCLE_WIDTH * 0.5f;
	py = TEXTURE_CIRCLE_Y + TEXTURE_LOGO_BGM_HEIGHT * 1.5;
	pw = TEXTURE_CIRCLE_WIDTH;
	ph = TEXTURE_CIRCLE_HEIGHT;

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);
}

// �ݒ��ʃt���O�̎擾
int GetSettingFlag(void)
{
	return g_SettingFlag;
}

// �ݒ��ʃt���O�𗧂Ă�
void SetSettingFlag(void)
{
	g_SettingFlag = TRUE;
}

// �ݒ��ʂ̑I��ԍ������Z�b�g����
void ResetSettingMenu(void)
{
	g_SettingMenu = 0;
}