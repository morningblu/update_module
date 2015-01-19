/** �� �� ��: im_download.c
** Copyright (c), imove(SHENTHEN) Co., Ltd.
** ��    ��: 2014-12-8
** ��    ��:
** ��    ��:
** �޸���ʷ:
** 2014-12-8   �ű󴴽����ļ���
##############################################################################*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "curl/curl.h"
#include "im_download.h"

#define IM_DOWNLOAD_FILE   "/tmp/tmp_update.bin"

size_t im_write_data(void *ptr, size_t size, size_t nmemb, void *stream)  
{
	int written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}

int im_curl_init(CURL **curlhandle)
{
	CURLcode res;

	if (curlhandle == NULL)
	{
		IM_DEBUG("PARAM ERROR");
		return -1;
	}
	
	res = curl_global_init(CURL_GLOBAL_ALL);
	if (res != CURLE_OK)
	{
		IM_DEBUG("curl_global_init fail:%s", curl_easy_strerror(res));	
		return -1;
	}
	
	*curlhandle = curl_easy_init(); 
	if (*curlhandle == NULL)
	{
		IM_DEBUG("curl_easy_init error");
		return -1;
	}

	return res;
}


int im_curl_clean(CURL *curlhandle)
{
	if (curlhandle == NULL)
	{
		IM_DEBUG("PARAM ERROR");
		return -1;
	}
	
	curl_easy_cleanup(curlhandle);
	curl_global_cleanup();
}

int im_progress_func(char *progress_data,
                     double t, /* dltotal */
                     double d, /* dlnow */
                     double ultotal,
                     double ulnow)
{
	int per = (int)(d*100.0/t);
	long dlen = (long) (d);
	long tlen = (long) (t);
	
	S_IM_MSG_FIRMWARE *firmware = (S_IM_MSG_FIRMWARE *)progress_data;
	
	*firmware->dlen = dlen;
	*firmware->tlen = tlen;
  return 0;
}


/*********************************************
function:   ����ָ���ļ�
params:
[IN]firmware:    ���ص��ļ���Ϣ
[IN] sw_name  :    ָ�������ļ�������
return: -1��0
***********************************************/

int im_download_curl(S_IM_MSG_FIRMWARE *firmware, char *sw_name)
{
	CURL *curlhandle = NULL;             //����CURL���͵�ָ��  
	CURLcode res;           //����CURLcode���͵ı��������淵��״̬��
	FILE *file = NULL;
	long retcode = 0;
	int err = 0;
	struct stat file_info;
	curl_off_t local_file_len = -1 ;
	int use_resume = 0;

	if ((NULL == firmware)||(NULL == sw_name))
	{
	    IM_DEBUG("Bad argument");
	    return -1;
	}
	
	err = im_curl_init(&curlhandle);
	if (err < 0)
	{
		IM_DEBUG("im_curl_init");
		return err;
	}

	if(stat(sw_name, &file_info) == 0) 
	{
		local_file_len =  file_info.st_size;
		use_resume  = 1;
	}

	file = fopen(sw_name, "ab+"); 
	if (file == NULL) 
	{
		IM_DEBUG("open file %s fail", sw_name);
		err = -1;
		goto openfile_err;
	}

	curl_easy_setopt(curlhandle, CURLOPT_URL, firmware->im_url);
	curl_easy_setopt(curlhandle, CURLOPT_CONNECTTIMEOUT, 5);  // �������ӳ�ʱ����λ��

	// �����ļ�������λ�ø�libcurl
	curl_easy_setopt(curlhandle, CURLOPT_RESUME_FROM_LARGE, use_resume?local_file_len:0);
	curl_easy_setopt(curlhandle, CURLOPT_NOPROGRESS, 1L);

	curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, file);
	curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, im_write_data);
	//�������ؽ���
	curl_easy_setopt(curlhandle, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(curlhandle, CURLOPT_PROGRESSFUNCTION, im_progress_func);
	curl_easy_setopt(curlhandle, CURLOPT_PROGRESSDATA, firmware);

	curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 0L);

	res = curl_easy_perform(curlhandle);
	if (res != CURLE_OK)
	{
		  IM_DEBUG("%s", curl_easy_strerror(res));
		  err = -1;
		  goto out;
	}

	res = curl_easy_getinfo(curlhandle, CURLINFO_RESPONSE_CODE , &retcode);
	if ((res == CURLE_OK)&& ((retcode == 200) || (206 == retcode)))
	{
		IM_DEBUG("download ok");
	}
	else if((res == CURLE_OK)&& (retcode == 416)) //�ϵ���������
	{
		IM_DEBUG("%s retcode:%d ", curl_easy_strerror(res), retcode);
	}
	else 
	{
		IM_DEBUG("fail %s retcode:%d ", curl_easy_strerror(res), retcode);
		err = -1;
	}


out:
		fclose(file);
openfile_err:
		im_curl_clean(curlhandle);
	
	return err;
}



/*********************************************
function:   ��ȡִ������ķ��ؽ��
params:
[IN]name:    ִ�е�������
[OUT] value  :    ���ص��ַ���
return: -1��0
***********************************************/
int im_get_cmd_value(char *name, char *value)
{
    char cmd[MAX_URL_LEN] = {0};
    FILE* fp = NULL;

	if (!value ||!name)
	{
		IM_DEBUG("param error");
		return -1;
	}
	
    snprintf(cmd, sizeof(cmd) - 1, "%s", name);
    fp = popen(cmd, "r");
    if (!fp)
    {
    	IM_DEBUG("popen fail");
    	return -1;
    }

	fgets(value, MAX_STAS_LEN - 1, fp);
	if (value[strlen(value)-1] == 0x0a)
		value[strlen(value)-1] = '\0';
    pclose(fp);
    
    return 0;
}


/*********************************************
function:   �Ƚ�MD5�Ƿ�ƥ��
params:
[IN]MD5:    ��ȷ��MD5ֵ
[IN] sw_name  :    Ҫ�Ƚ�MD5���ļ���
return: -1��0
***********************************************/
int im_check_md5(char *md5, char *sw_name)
{
	char cmd[MAX_URL_LEN] = {0};
    char md5_str[64] = {0};
	int ret = -1;

	if (!md5 ||!sw_name)
	{
		IM_DEBUG("param error");
		return -1;
	}
	
	snprintf(cmd, MAX_URL_LEN-1, "md5sum %s | cut -d ' ' -f 1", sw_name);
	ret = im_get_cmd_value(cmd, md5_str);
	if (ret != 0)
	{
		IM_DEBUG("(%s) failed!", cmd);
		return -1;
	}

	if (strcmp(md5, md5_str) != 0)
	{
		IM_DEBUG("MD5 not match");
		return -1;
	}


	return 0;
}


int im_upgrade_firmware(S_IM_MSG_FIRMWARE *firmware, pfunc_upgrade im_upgrade)
{
	int ret = -1;
	char cmd[MAX_URL_LEN] = {0};

	if (!firmware || !im_upgrade)
	{
		IM_DEBUG("param error");
		return -1;
	}
	
	//������Ҫ���µ��ļ�
	ret = im_download_curl(firmware, IM_DOWNLOAD_FILE);
	if (ret != 0)
	{
		IM_DEBUG("download file fail");
		return -1;
	}
	//�Ƚ�MD5�Ƿ���ȷ
	ret = im_check_md5(firmware->im_md5, IM_DOWNLOAD_FILE);
	if (ret != 0)
	{
		unlink(IM_DOWNLOAD_FILE);	//MD5����ɾ���ļ�
		return -1;
	}

	rename(IM_DOWNLOAD_FILE, FIRMWARE_NAME);
	ret = im_upgrade();
	
	return ret;
}

int im_upgrade(void)
{
	IM_DEBUG("start upgrade!!!!");
}


void im_test(void)
{
	S_IM_MSG_FIRMWARE firmware;
	long dlen = 0;
	long tlen = 0;
	int ret = -1;
	
	strcpy(firmware.im_url, "http://172.16.2.79/test/openwrt-rtkmips-rtl8198c-AP-fw.bin");
	strcpy(firmware.im_md5, "75be7279654bd145c6ea1b0a71f532fb");
	firmware.dlen = &dlen;
	firmware.tlen = &tlen;
	
	ret = im_upgrade_firmware(&firmware, im_upgrade);
	if (ret < 0)
	{
		IM_DEBUG("UPDATE FAIL!!!!");
	}
	else
	{
		IM_DEBUG("UPDATE ok, tlen:%d, dlen:%d!!!!", dlen, tlen);
	}

}

#if 0
int main (void)
{
	im_test();

	return 0;
}

#endif

