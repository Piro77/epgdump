#ifndef	__TS_CONTROL_H__
#define	__TS_CONTROL_H__

#include	"util.h"

typedef	struct	_SVT_CONTROL	SVT_CONTROL;
typedef	struct	_EIT_CONTROL	EIT_CONTROL;
typedef struct	_CURRENTNEXT	CURRENTNEXT;
typedef struct  _EITEXTDESC     EITEXTDESC;
typedef struct	_AUDIODESC	AUDIODESC;

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
	char	start_time[5];
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
};

struct	_EIT_CONTROL{
	EIT_CONTROL	*next ;
	EIT_CONTROL	*prev ;
	int		table_id ;
	int		servid ;
	int		event_id ;			// イベントID
	int		content_type ;		// コンテントタイプ
    int		yy;
    int		mm;
    int		dd;
    int		hh;
    int		hm;
	int		ss;
	int		dhh;
	int		dhm;
	int		dss;
	int		ehh;
	int		emm;
	int		ess;
	char	*title ;			// タイトル
	char	*subtitle ;			// サブタイトル
	short   eitextcnt;
	EITEXTDESC  *eitextdesc;		// 拡張番組情報
	char	video;				// 映像情報
	AUDIODESC	audiodesc[2];		// 音声情報
	char	freeCA;				// スクランブル
};
#endif
