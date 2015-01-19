/** �� �� ��: im_queue.h
** Copyright (c), imove(SHENTHEN) Co., Ltd.
** ��    ��: 2014-12-1
** ��    ��:ѭ�����е�ʵ��
** ��    ��:
** �޸���ʷ:
** 2014-12-1   �ű󴴽����ļ���
##############################################################################*/

#ifndef IM_QUEUE_H  
#define IM_QUEUE_H 

/******************************************
������ʹ��ǰ�趨����������ֵ
#define MAXQUEUELEN 20  //���е���󳤶�  
typedef int ElemType; //���е���������

*******************************************/
#define MAXQUEUELEN 20 
typedef char *ElemType;

typedef struct{  
    ElemType data[MAXQUEUELEN]; //����  
    int front; //��ͷ���α�  
    int rear;  //��β���α�  
}Queue;  
  
void IM_InitQueue(Queue *q); //��ʼ������  
  
void IM_EnQueue(Queue *q,ElemType e); //Ԫ��e����  
  
void IM_DeQueue(Queue *q,ElemType *e); //��ͷ��Ԫ�س���  
  
int IM_IsEmpty(Queue *q); //�ж϶����Ƿ�Ϊ��  
  
int IM_GetQueueLength(Queue *q); //���ض��еĳ���  
  
void IM_Clear(Queue *q); //��ն���  
  
void IM_Print(Queue *q); //��ӡ����  
  
#endif //IM_QUEUE_H  


