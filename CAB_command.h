#ifndef _CAB_COMMAND_
#define _CAB_COMMAND_

#define FIG_STAT_NORMAL		0	// そのまま
#define FIG_STAT_FLIP_UD	1	// 上下反転
#define FIG_STAT_FLIP_LR	2	// 左右反転
#define FIG_STAT_ERASE		4	// 図形の消去

struct CAB_command
{
	unsigned char cmd;		// 命令．CMD_*
	unsigned char figure;	// 対象の図形（インデックス．0～3）
	bool canceled;			// キャンセルされたらtrue
};

struct CAB_figure
{
	unsigned char figure;	// 元の図形（インデックス．0～3）
	unsigned char stat;		// 図形の状態．FIG_STAT_*
};

#endif
