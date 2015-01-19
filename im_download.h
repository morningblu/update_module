/** �� �� ��: im_download.h
** Copyright (c), imove(SHENTHEN) Co., Ltd.
** ��    ��: 2014-12-8
** ��    ��:
** ��    ��:
** �޸���ʷ:
** 2014-12-8   �ű󴴽����ļ���
##############################################################################*/
#ifndef __IM_DOWNLOAD_H__
#define __IM_DOWNLOAD_H__

#define MAX_STRING		33
#define MAX_URL_LEN		512
#define FIRMWARE_NAME	"/tmp/update.bin"	//Ĭ�ϱ����ļ���
#define MAX_STAS_LEN        (18*64)

#define DOWN_BREAK_CONTINUE 	1

typedef struct
{
	char	im_url[MAX_URL_LEN];
	char 	im_md5[MAX_STRING];
	long 	*dlen;			//�Ѿ��������ݵĳ���Byte
	long 	*tlen;			//�ļ��ܳ��ȵ�λByte
}S_IM_MSG_FIRMWARE;

typedef int (*pfunc_upgrade)(void);	//�����ص�����

#define IM_DEBUG_FLAG 1

#if IM_DEBUG_FLAG
#define IM_DEBUG(format, ...) \
    do { \
        fprintf(stderr, "[%s@%s,%d] " format "\n", \
             __func__, __FILE__, __LINE__, ##__VA_ARGS__ ); \
    } while (0)  
#else
#define IM_DEBUG(format, ...)
#endif


extern int im_upgrade_firmware(S_IM_MSG_FIRMWARE *firmware, pfunc_upgrade im_upgrade);


#endif

