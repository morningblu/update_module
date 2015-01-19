#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "im_update.h"

#if 0
#include "curl/curl.h"



CURLcode im_curl_init(void)
{
	CURL *curl = NULL;             //����CURL���͵�ָ��  
	CURLcode res;           //����CURLcode���͵ı��������淵��״̬��
	
	res = curl_global_init(CURL_GLOBAL_ALL);
	
	curl = curl_easy_init(); 
	if (curl == NULL)
	{
		printf("curl_easy_init error\n");
		return -1;
	}
}


int im_download(char *url, char *local_path)
{
	CURL *curl = NULL;             //����CURL���͵�ָ��  
	CURLcode res;           //����CURLcode���͵ı��������淵��״̬��
	FILE *file = NULL;
	
	res = curl_global_init(CURL_GLOBAL_ALL);
	
	curl = curl_easy_init(); 
	if (curl == NULL)
	{
		printf("curl_easy_init error\n");
		return -1;
	}
	
	file = fopen(local_path, "w");
	if(file == NULL)
	{
		printf("file open error\n");
		return -1;
	}

	
}

#else
#include <sys/time.h>
#include <sys/wait.h>
#include <signal.h>

void im_cmdline_to_argcv(char *cmd, int *argc, char *argv[])
{
    int     count = 0;
    char    *head;
    char    *end;

    head = cmd;
    do
    {
        if (count >= *argc)
        {
            break;
        }

        /* ȥ���ײ�����Ŀո����Ʊ�� */
        for (; (*head == ' ' || *head == '\t'); head++)
        {
        }

        if (*head == '\0')
        {
            break;
        }

        if (   (NULL != (end = strchr(head, ' ')))
            || (NULL != (end = strchr(head, '\t')))
           )
        {
            *end = '\0';
            argv[count++] = head;
            head = end + 1;
        }
        else
        {
            argv[count++] = head;
            break;
        }
    } while (1);

    argv[count] = NULL;
    *argc = count;
}

void im_bc_execvp(const char *fmt, ...)
{
    va_list     args;
    char        cmd[512] = {0};
    char        *argv[128] = {0};
    int         argc = sizeof(argv)/sizeof(argv[0]);

	va_start(args, fmt);
    vsnprintf(cmd, sizeof(cmd) - 1, fmt, args);
	va_end(args);

    im_cmdline_to_argcv(cmd, &argc, argv);

    execvp(argv[0], argv);
}

void sig_handler(int sig)
{
	int bakup = errno;

	switch (sig)
	{
		case SIGALRM:
		case SIGTERM:
			kill(pid, SIGTERM);
			break;
		default:
			break;
	}

	errno = bakup;
}


int im_download_wget(char *url, char *sw_name, int time_out)
{
    pid_t       pid;
    int         status;
    unsigned long msec = (unsigned long)time_out * 1000;
	char cmd[DOWNLOAD_PATH_LEN] = {0};
	
    struct sigaction    nsa;
    struct sigaction    osa;
    struct itimerval    nitv;
    struct itimerval    oitv;

    if ((NULL == url)(NULL == sw_name))
    {
        printf("Bad argument\n");
        return -1;
    }
	
	unlink(msg->im_sw_name);	//����֮ǰ��ɾ��֮ǰ���ļ�
	
	snprintf(cmd, DOWNLOAD_PATH_LEN-1,
				"/usr/bin/wget %s -O %s%s",
				url, FIRMWARE_DOWNLOAD_PATH, sw_name);

    pid = vfork();

    if (-1 == pid) /* �쳣��� */
    {
        return -1;
    }

    if (pid == 0) /* �ӽ��� */
    {
        im_bc_execvp(cmd);
    }

    if (0 != msec)
    {
    	nsa.sa_handler = sig_handler;
    	sigemptyset(&nsa.sa_mask);
    	nsa.sa_flags = 0;
    	sigaction(SIGALRM, &nsa, &osa);

    	nitv.it_value.tv_sec = msec / 1000;
    	nitv.it_value.tv_usec = (msec % 1000) * 1000;
    	nitv.it_interval.tv_sec = 0;
        nitv.it_interval.tv_usec = 0;
    	setitimer(ITIMER_REAL, &nitv, &oitv);
    }

    /* �����̵ȴ��ӽ��̷��� */
    do
    {
        if (waitpid(pid, &status, 0) == -1)
        {
            if (EINTR != errno)
            {
                status = -1;
                break;
            }
        }
        else
        {
            break;
        }
    } while (1);

    /*-------- �����ӽ����˳�״̬������Ӧ���� --------*/
    if (0 != msec)
    {
        /* �ָ��ɵ�SIGALRM�Ĵ��� */
    	sigaction(SIGALRM, &osa, NULL);
        setitimer(ITIMER_REAL, &oitv, NULL);
    }

    if (status != 0)
    {
       return -1;
    }

    return 0;
}

#endif
