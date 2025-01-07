//=============================================================================
//
// UI�\������ [ui.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "ui.h"
#include "player.h"
#include "enemy.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(16)	// HP�T�C�Y
#define TEXTURE_HEIGHT				(32)	// 

#define GAUGE_WIDTH					(32.0f * PLAYER_HP_MAX)	// �Q�[�W�T�C�Y
#define GAUGE_HEIGHT				(32)	// 
#define TEXTURE_MAX					(4)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(3)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)	// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/heart.png",
	"data/TEXTURE/bar_white.png",
	"data/TEXTURE/bar_blue.png",
	"data/TEXTURE/pipo-hikarimono002.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static int						g_Hp;					// �X�R�A

static BOOL						g_Load = FALSE;
static int						g_AnimNum;					// �G�t�F�N�g�̃A�j���[�V����
static float					g_CountAnim;				// �A�j���[�V�����J�E���^

//=============================================================================
// ����������
//=============================================================================
HRESULT InitUI(void)
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


	// �v���C���[�̏�����
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 10.0f, 20.0f, 0.0f };
	g_TexNo = 0;

	g_Hp = PLAYER_HP_MAX;	// �̗͂̏�����

	g_CountAnim = 0.0f;
	g_AnimNum = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitUI(void)
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
void UpdateUI(void)
{
	PLAYER* player = GetPlayer();
	g_Hp = player->hp;
	g_TexNo = 0;
	g_AnimNum = (g_AnimNum + 1) % 10;
	g_CountAnim += 1.0f;
	if (g_CountAnim > ANIM_WAIT)
	{
		g_CountAnim = 0.0f;
		// �p�^�[���̐؂�ւ�
		g_AnimNum = (g_AnimNum + 1) % ANIM_PATTERN_NUM;
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawUI(void)
{
	PLAYER* player = GetPlayer();

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

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	// ��������������
	for (int i = 0; i < g_Hp; i++)
	{
		// ����\�����錅�̐���
		float x = (float)((i % 2));

		// HP�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Pos.x + g_w*i;	// HP�̕\���ʒuX
		float py = g_Pos.y;			// HP�̕\���ʒuY
		float pw = g_w;				// HP�̕\����
		float ph = g_h;				// HP�̕\������

		float tw = 0.5f;		// �e�N�X�`���̕�
		float th = 1.0f;		// �e�N�X�`���̍���
		float tx = x * tw;			// �e�N�X�`���̍���X���W
		float ty = 0.0f;			// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
	g_TexNo++;

	// �Q�[�W�̕`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		float px = g_Pos.x;	// �Q�[�W�̕\���ʒuX
		float py = g_Pos.y + TEXTURE_HEIGHT;			// �Q�[�W�̕\���ʒuY
		float pw = GAUGE_WIDTH;				// �Q�[�W�̕\����
		float ph = g_h;				// �Q�[�W�̕\������

		float tw = 1.0f;		// �e�N�X�`���̕�
		float th = 1.0f;		// �e�N�X�`���̍���
		float tx = 0.0f;			// �e�N�X�`���̍���X���W
		float ty = 0.0f;			// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		g_TexNo++;
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);
		pw = pw * 1.0f / ENEMY_MAX * player->gauge;				// �Q�[�W�̕\����

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		g_TexNo++;
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);
		px += pw * 0.5f;
		py -= 5.0f;
		pw = 20.0f;
		ph += 20.0f;
		tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
		th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
		tx = (float)(g_AnimNum % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
		ty = (float)(g_AnimNum / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}
