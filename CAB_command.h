#ifndef _CAB_COMMAND_
#define _CAB_COMMAND_

#define FIG_STAT_NORMAL		0	// ���̂܂�
#define FIG_STAT_FLIP_UD	1	// �㉺���]
#define FIG_STAT_FLIP_LR	2	// ���E���]
#define FIG_STAT_ERASE		4	// �}�`�̏���

struct CAB_command
{
	unsigned char cmd;		// ���߁DCMD_*
	unsigned char figure;	// �Ώۂ̐}�`�i�C���f�b�N�X�D0�`3�j
	bool canceled;			// �L�����Z�����ꂽ��true
};

struct CAB_figure
{
	unsigned char figure;	// ���̐}�`�i�C���f�b�N�X�D0�`3�j
	unsigned char stat;		// �}�`�̏�ԁDFIG_STAT_*
};

#endif
