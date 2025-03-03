
//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : 
//
//=============================================================================
#include "title.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "setting.h"
#include "tutorial.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 

#define TEXTURE_TITLE_X				(SCREEN_WIDTH / 5)		// �^�C�g���̉���
#define TEXTURE_TITLE_Y				(SCREEN_HEIGHT / 10)		// �^�C�g���̏c��
#define TEXTURE_TITLE_WIDTH			(SCREEN_WIDTH * 3 / 5)		// �^�C�g���̏c��
#define TEXTURE_TITLE_HEIGHT		(SCREEN_HEIGHT * 2 / 10)		// �^�C�g���̏c��

#define TEXTURE_X_LOGO				((float)(TEXTURE_WIDTH / 4))
#define TEXTURE_Y_LOGO				(280.0f)
#define TEXTURE_WIDTH_LOGO			(480.0f)			// ���S�̏c��
#define TEXTURE_HEIGHT_LOGO			(50.0f)			// ���S�̉���
#define TEXTURE_INTERVAL_LOGO		(50.0f)				// ���S�̊Ԋu

enum 
{
	START_GAME,
	SETTING,
#ifdef _DEBUG
	EDITOR,
#endif
	QUIT_GAME,
	MENU_MAX
};

enum
{
	TEXTURE_BG,
	TEXTURE_TITLE,
	TEXTURE_START,
	TEXTURE_SELECT,
	TEXTURE_SETTING,
#ifdef _DEBUG
	TEXTURE_EDITOR,

#endif
	TEXTURE_QUIT,
	TEXTURE_CURSOR,

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
	"data/TEXTURE/title.png",
	"data/TEXTURE/title_startgame.png",
	"data/TEXTURE/title_selectstage.png",
	"data/TEXTURE/title_settings.png",
#ifdef _DEBUG
	"data/TEXTURE/title_editor.png",
#endif
	"data/TEXTURE/title_quitgame.png",
	"data/TEXTURE/title_cursor.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�
static int						g_Cursor;					// �J�[�\���̈ʒu

float	alpha;
BOOL	flag_alpha;

static BOOL						g_Load = FALSE;

static float	effect_dx;
static float	effect_dy;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
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
	g_TexNo = TEXTURE_BG;
	g_Cursor = 0;

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
void UninitTitle(void)
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
void UpdateTitle(void)
{
	// ���L�[����������J�[�\���̈ʒu��������
	if (GetKeyboardTrigger(DIK_DOWN))
	{
		g_Cursor = (g_Cursor + 1) % MENU_MAX;
		PlaySound(SOUND_LABEL_SE_select);
		// �Z�[�u�f�[�^�����������烍�[�h��ʂɑJ�ڂł��Ȃ��悤�ɂ���
		//if(g_Cursor % MENU_MAX == LOAD_GAME && CheckData() == FALSE) 	g_Cursor++;
	}

	else if (IsButtonTriggered(0, BUTTON_DOWN))
	{
		g_Cursor = (g_Cursor + 1) % MENU_MAX;
		PlaySound(SOUND_LABEL_SE_select);
		// �Z�[�u�f�[�^�����������烍�[�h��ʂɑJ�ڂł��Ȃ��悤�ɂ���
		//if(g_Cursor % MENU_MAX == LOAD_GAME && CheckData() == FALSE) 	g_Cursor++;
	}

	
	// ��L�[����������J�[�\���̈ʒu���グ��
	if (GetKeyboardTrigger(DIK_UP))
	{
		g_Cursor = (g_Cursor + MENU_MAX - 1) % MENU_MAX;
		PlaySound(SOUND_LABEL_SE_select);

		// �Z�[�u�f�[�^�����������烍�[�h��ʂɑJ�ڂł��Ȃ��悤�ɂ���
		//if (g_Cursor % MENU_MAX == LOAD_GAME && CheckData() == FALSE) 	g_Cursor--;
	}
	else if (IsButtonTriggered(0, BUTTON_UP))
	{
		g_Cursor = (g_Cursor + MENU_MAX - 1) % MENU_MAX;
		PlaySound(SOUND_LABEL_SE_select);

		// �Z�[�u�f�[�^�����������烍�[�h��ʂɑJ�ڂł��Ȃ��悤�ɂ���
		//if (g_Cursor % MENU_MAX == LOAD_GAME && CheckData() == FALSE) 	g_Cursor--;
	}

	switch (g_Cursor % MENU_MAX)
	{
	// �V�K�Q�[���J�n
	case START_GAME:
		if (GetKeyboardTrigger(DIK_RETURN))
		{// Enter��������A�X�e�[�W��؂�ւ���
			SetFade(FADE_OUT, MODE_GAME);
			PlaySound(SOUND_LABEL_SE_enter);
			SetTutorialFlg();
		}
		// �Q�[���p�b�h�œ��͏���
		else if (IsButtonTriggered(0, BUTTON_B))
		{
			SetFade(FADE_OUT, MODE_GAME);
			PlaySound(SOUND_LABEL_SE_enter);
			SetTutorialFlg();
		}
		break;

	
	// �ݒ��ʂ�\��
	case SETTING:
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			SetSettingFlag();
			PlaySound(SOUND_LABEL_SE_enter);
		}
		// �Q�[���p�b�h�œ��͏���
		else if (IsButtonTriggered(0, BUTTON_B))
		{
			SetSettingFlag();
			PlaySound(SOUND_LABEL_SE_enter);
		}
		break;

#ifdef _DEBUG
	case EDITOR:
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			SetMode(MODE_EDITOR);
			PlaySound(SOUND_LABEL_SE_enter);
		}
		// �Q�[���p�b�h�œ��͏���
		else if (IsButtonTriggered(0, BUTTON_B))
		{
			SetMode(MODE_EDITOR);
			PlaySound(SOUND_LABEL_SE_enter);
		}
		break;
#endif

	case QUIT_GAME:
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			SetMode(MODE_MAX);
			PlaySound(SOUND_LABEL_SE_enter);
		}
		// �Q�[���p�b�h�œ��͏���
		else if (IsButtonTriggered(0, BUTTON_B))
		{
			SetMode(MODE_MAX);
			PlaySound(SOUND_LABEL_SE_enter);
		}
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("Player:�� �� �� ���@Space\n");

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
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

	// �^�C�g���̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_BG]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.1f, 1.0f, 0.1f);

		// �|���S���`��
		//GetDeviceContext()->Draw(4, 0);
	}

	// �^�C�g���̕`��
		// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_TITLE]);

	//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = TEXTURE_TITLE_X;		// SelectStage�̕\���ʒuX
	float py = TEXTURE_TITLE_Y;		// SelectStage�̕\���ʒuY
	float pw = TEXTURE_TITLE_WIDTH;		// �Q�[�W�̕\����
	float ph = TEXTURE_TITLE_HEIGHT;		// �Q�[�W�̕\������

	float tw = 1.0f;	// �e�N�X�`���̕�
	float th = 1.0f;	// �e�N�X�`���̍���
	float tx = 0.0f;	// �e�N�X�`���̍���X���W
	float ty = 0.0f;	// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_START]);

	//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
	px = TEXTURE_X_LOGO;		// SelectStage�̕\���ʒuX
	py = TEXTURE_Y_LOGO;		// SelectStage�̕\���ʒuY
	pw = TEXTURE_WIDTH_LOGO;		// �Q�[�W�̕\����
	ph = TEXTURE_HEIGHT_LOGO;		// �Q�[�W�̕\������

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_SETTING]);

	//���̃��S��`��
	py += TEXTURE_INTERVAL_LOGO;		// Setting�̕\���ʒuY

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);


#ifdef _DEBUG
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_EDITOR]);

	//���̃��S��`��
	py += TEXTURE_INTERVAL_LOGO;		// Setting�̕\���ʒuY

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

#endif

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_QUIT]);

	//���̃��S��`��
	py += TEXTURE_INTERVAL_LOGO;		// QuitGame�̕\���ʒuY

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_CURSOR]);

	//�J�[�\����`��
	py = TEXTURE_Y_LOGO + TEXTURE_INTERVAL_LOGO * (g_Cursor % MENU_MAX);		// �J�[�\���̕\���ʒuY

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);
}





