#ifndef	__TS_CONTROL_H__
#define	__TS_CONTROL_H__

#include	"util.h"

#define	EIT_SDTNOTFOUND	0
#define	EIT_OK		1
#define	EIT_CHECKOK	2
#define	EIT_CHECKNG	3

typedef struct  _NIT_CONTROL    NIT_CONTROL;
typedef	struct	_SVT_CONTROL	SVT_CONTROL;
typedef	struct	_EIT_CONTROL	EIT_CONTROL;
typedef struct	_CURRENTNEXT	CURRENTNEXT;
typedef struct  _EITEXTDESC     EITEXTDESC;
typedef struct	_AUDIODESC	AUDIODESC;
typedef struct  _EITCHECK	EITCHECK;

struct _EITCHECK {          //イベントチェック用構造体
    int svid;               // 対象サービスID
    int evid;               // 対象イベントID
    time_t  starttime;      // 対象イベント開始時刻(check時のみ設定)
    time_t  tdttime;        // 現在のTDT(wait時の時刻チェック用)
    time_t  waitend;        // 最大待機時間(check時は11秒固定)
    int     maxcycle;       // BITにあるEIT送出周期で一番大きな値
};

struct _AUDIODESC {
    char audiotype;     // AudioType
    char langcode[8];   // 言語コード(jpn) 二ヶ国語の場合(jpn eng)
    char *audiodesc;    // text
};

struct _EITEXTDESC {
    char *item_description;
    char *item;
};
struct _CURRENTNEXT {
	short	event_id;
	unsigned char	start_time[5];
	char	duration[3];
	char	running_status;
};

struct	_SVT_CONTROL{
	SVT_CONTROL	*next ;
	SVT_CONTROL	*prev ;
	int		event_id ;				// イベントID
	int		original_network_id ;			// OriginalNetworkID
	int		transport_stream_id ;			// TransporrtStreamID
	char	servicename[MAXSECLEN] ;		// サービス名
	EIT_CONTROL	*eit;				// EIT テーブル
	CURRENTNEXT	cnev[2];
	char		haveeitschedule;		// EITスケジュールがあるか。
	int		frequency;
    int     remote_control_key_id;              // リモコンID（地デジ）
};

struct	_EIT_CONTROL{
	EIT_CONTROL	*next ;
	EIT_CONTROL	*prev ;
	int		table_id ;
	int		servid ;
	int		event_id ;			// イベントID
	unsigned char	numcontent;
	unsigned char 	content[7];     //コンテントタイプ
	unsigned char	usernibble[7];
	unsigned char	numattachinfo;
	unsigned char	attachinfo[7];  //番組付属情報
	char	*title ;			// タイトル
	char	*subtitle ;			// サブタイトル
	short  eitextcnt;
	EITEXTDESC *eitextdesc;
	char	video;				// 映像情報
	AUDIODESC	audiodesc[2];		// 音声情報
	char	freeCA;				// スクランブル
	int	duration;			// 時間
	time_t	start_time;			// 開始時刻
};
#endif
