//=============================================================================
//
// �G�t�F�N�g���� [effect.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "effect.h"
#include "enemy.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define EFFECT_FRAME		(30)
#define TEXTURE_MAX			(EFFECT_FRAME * EFFECT_NUM)				// �e�N�X�`���̐�

#define	EFFECT_MAX			(20)				// �G�t�F�N�g�ő吔

//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexEffect(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static EFFECT					g_aEffect[EFFECT_MAX];	// �؃��[�N
static BOOL					g_bAlpaTest;		// �A���t�@�e�X�gON/OFF

static int					g_TexNo;			// �e�N�X�`���ԍ�
static int					g_TexAnim;			// �e�N�X�`���A�j���p

static char *g_TextureName[] =
{
	"data/Effect_BigHit/Effect_BigHit_1_000.png",
	"data/Effect_BigHit/Effect_BigHit_1_001.png",
	"data/Effect_BigHit/Effect_BigHit_1_002.png",
	"data/Effect_BigHit/Effect_BigHit_1_003.png",
	"data/Effect_BigHit/Effect_BigHit_1_004.png",
	"data/Effect_BigHit/Effect_BigHit_1_005.png",
	"data/Effect_BigHit/Effect_BigHit_1_006.png",
	"data/Effect_BigHit/Effect_BigHit_1_007.png",
	"data/Effect_BigHit/Effect_BigHit_1_008.png",
	"data/Effect_BigHit/Effect_BigHit_1_009.png",
	"data/Effect_BigHit/Effect_BigHit_1_010.png",
	"data/Effect_BigHit/Effect_BigHit_1_011.png",
	"data/Effect_BigHit/Effect_BigHit_1_012.png",
	"data/Effect_BigHit/Effect_BigHit_1_013.png",
	"data/Effect_BigHit/Effect_BigHit_1_014.png",
	"data/Effect_BigHit/Effect_BigHit_1_015.png",
	"data/Effect_BigHit/Effect_BigHit_1_016.png",
	"data/Effect_BigHit/Effect_BigHit_1_017.png",
	"data/Effect_BigHit/Effect_BigHit_1_018.png",
	"data/Effect_BigHit/Effect_BigHit_1_019.png",
	"data/Effect_BigHit/Effect_BigHit_1_020.png",
	"data/Effect_BigHit/Effect_BigHit_1_021.png",
	"data/Effect_BigHit/Effect_BigHit_1_022.png",
	"data/Effect_BigHit/Effect_BigHit_1_023.png",
	"data/Effect_BigHit/Effect_BigHit_1_024.png",
	"data/Effect_BigHit/Effect_BigHit_1_025.png",
	"data/Effect_BigHit/Effect_BigHit_1_026.png",
	"data/Effect_BigHit/Effect_BigHit_1_027.png",
	"data/Effect_BigHit/Effect_BigHit_1_028.png",
	"data/Effect_BigHit/Effect_BigHit_1_029.png",

	"data/Effect_Explosion/Effect_Explosion_1_000.png",
	"data/Effect_Explosion/Effect_Explosion_1_001.png",
	"data/Effect_Explosion/Effect_Explosion_1_002.png",
	"data/Effect_Explosion/Effect_Explosion_1_003.png",
	"data/Effect_Explosion/Effect_Explosion_1_004.png",
	"data/Effect_Explosion/Effect_Explosion_1_005.png",
	"data/Effect_Explosion/Effect_Explosion_1_006.png",
	"data/Effect_Explosion/Effect_Explosion_1_007.png",
	"data/Effect_Explosion/Effect_Explosion_1_008.png",
	"data/Effect_Explosion/Effect_Explosion_1_009.png",
	"data/Effect_Explosion/Effect_Explosion_1_010.png",
	"data/Effect_Explosion/Effect_Explosion_1_011.png",
	"data/Effect_Explosion/Effect_Explosion_1_012.png",
	"data/Effect_Explosion/Effect_Explosion_1_013.png",
	"data/Effect_Explosion/Effect_Explosion_1_014.png",
	"data/Effect_Explosion/Effect_Explosion_1_015.png",
	"data/Effect_Explosion/Effect_Explosion_1_016.png",
	"data/Effect_Explosion/Effect_Explosion_1_017.png",
	"data/Effect_Explosion/Effect_Explosion_1_018.png",
	"data/Effect_Explosion/Effect_Explosion_1_019.png",
	"data/Effect_Explosion/Effect_Explosion_1_020.png",
	"data/Effect_Explosion/Effect_Explosion_1_021.png",
	"data/Effect_Explosion/Effect_Explosion_1_022.png",
	"data/Effect_Explosion/Effect_Explosion_1_023.png",
	"data/Effect_Explosion/Effect_Explosion_1_024.png",
	"data/Effect_Explosion/Effect_Explosion_1_025.png",
	"data/Effect_Explosion/Effect_Explosion_1_026.png",
	"data/Effect_Explosion/Effect_Explosion_1_027.png",
	"data/Effect_Explosion/Effect_Explosion_1_028.png",
	"data/Effect_Explosion/Effect_Explosion_1_029.png",

	"data/Effect_Anima/Effect_Anima_1_000.png",
	"data/Effect_Anima/Effect_Anima_1_001.png",
	"data/Effect_Anima/Effect_Anima_1_002.png",
	"data/Effect_Anima/Effect_Anima_1_003.png",
	"data/Effect_Anima/Effect_Anima_1_004.png",
	"data/Effect_Anima/Effect_Anima_1_005.png",
	"data/Effect_Anima/Effect_Anima_1_006.png",
	"data/Effect_Anima/Effect_Anima_1_007.png",
	"data/Effect_Anima/Effect_Anima_1_008.png",
	"data/Effect_Anima/Effect_Anima_1_009.png",
	"data/Effect_Anima/Effect_Anima_1_010.png",
	"data/Effect_Anima/Effect_Anima_1_011.png",
	"data/Effect_Anima/Effect_Anima_1_012.png",
	"data/Effect_Anima/Effect_Anima_1_013.png",
	"data/Effect_Anima/Effect_Anima_1_014.png",
	"data/Effect_Anima/Effect_Anima_1_015.png",
	"data/Effect_Anima/Effect_Anima_1_016.png",
	"data/Effect_Anima/Effect_Anima_1_017.png",
	"data/Effect_Anima/Effect_Anima_1_018.png",
	"data/Effect_Anima/Effect_Anima_1_019.png",
	"data/Effect_Anima/Effect_Anima_1_020.png",
	"data/Effect_Anima/Effect_Anima_1_021.png",
	"data/Effect_Anima/Effect_Anima_1_022.png",
	"data/Effect_Anima/Effect_Anima_1_023.png",
	"data/Effect_Anima/Effect_Anima_1_024.png",
	"data/Effect_Anima/Effect_Anima_1_025.png",
	"data/Effect_Anima/Effect_Anima_1_026.png",
	"data/Effect_Anima/Effect_Anima_1_027.png",
	"data/Effect_Anima/Effect_Anima_1_028.png",
	"data/Effect_Anima/Effect_Anima_1_029.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEffect(void)
{
	MakeVertexEffect();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = g_TexAnim = 0;

	// �G�t�F�N�g���[�N�̏�����
	for(int i = 0; i < EFFECT_MAX; i++)
	{
		ZeroMemory(&g_aEffect[i].material, sizeof(g_aEffect[i].material));
		g_aEffect[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_aEffect[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aEffect[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_aEffect[i].width = EFFECT_WIDTH;
		g_aEffect[i].height = EFFECT_HEIGHT;
		g_aEffect[i].use = FALSE;
		g_aEffect[i].texNum = 0;
	}

	g_bAlpaTest = TRUE;
	//g_nAlpha = 0x0;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEffect(void)
{
	for(int i = 0; i < TEXTURE_MAX; i++)
	{
		if(g_Texture[i] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	if(g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEffect(void)
{
	for(int i = 0; i < EFFECT_MAX; i++)
	{
		if(g_aEffect[i].use)
		{
			if (g_aEffect[i].texNum < EFFECT_SPONE * EFFECT_FRAME)
			{
				g_aEffect[i].texNum++;
				if (g_aEffect[i].texNum % EFFECT_FRAME == 0)
				{
					g_aEffect[i].use = FALSE;
				}
			}

			else
			{
				g_aEffect[i].texNum++;
				if (g_aEffect[i].texNum % EFFECT_FRAME == 0)
				{
					g_aEffect[i].texNum -= EFFECT_FRAME;
				}

				if (GetBossSponeCnt() >= BOSS_SPONE_TIME)
				{
					g_aEffect[i].use = FALSE;
				}
			}
		}
	}

#ifdef _DEBUG
	// �A���t�@�e�X�gON/OFF
	if(GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? FALSE: TRUE;
	}

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEffect(void)
{
	// ���e�X�g�ݒ�
	if (g_bAlpaTest == TRUE)
	{
		// ���e�X�g��L����
		SetAlphaTestEnable(TRUE);
	}

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(int i = 0; i < EFFECT_MAX; i++)
	{
		if(g_aEffect[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);


			// �֐��g������
			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;


			// �����s��i�����s��j��]�u�s�񂳂��ċt�s�������Ă��(����)
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];


			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_aEffect[i].scl.x, g_aEffect[i].scl.y, g_aEffect[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_aEffect[i].pos.x, g_aEffect[i].pos.y, g_aEffect[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_aEffect[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_aEffect[i].texNum]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexEffect(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = EFFECT_WIDTH;
	float fHeight = EFFECT_HEIGHT;

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// �@���̐ݒ�
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// �؂̃p�����[�^���Z�b�g
//=============================================================================
int SetEffect(XMFLOAT3 pos, float fWidth, float fHeight, int effectNum)
{
	int nIdxEffect = -1;

	for(int nCntEffect = 0; nCntEffect < EFFECT_MAX; nCntEffect++)
	{
		if(!g_aEffect[nCntEffect].use)
		{
			g_aEffect[nCntEffect].pos = pos;
			g_aEffect[nCntEffect].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_aEffect[nCntEffect].width = fWidth;
			g_aEffect[nCntEffect].height = fHeight;
			g_aEffect[nCntEffect].use = TRUE;
			g_aEffect[nCntEffect].texNum = effectNum * EFFECT_FRAME;

			// �e�̐ݒ�

			nIdxEffect = nCntEffect;

			break;
		}
	}

	return nIdxEffect;
}
