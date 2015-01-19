/** �� �� ��: im_queue.c
** Copyright (c), imove(SHENTHEN) Co., Ltd.
** ��    ��: 2014-12-1
** ��    ��:
** ��    ��:
** �޸���ʷ:
** 2014-12-1   �ű󴴽����ļ���
##############################################################################*/


void IM_InitQueue(Queue *q) //��ʼ������  
{  
    q->front = q->rear = 0;  
} 

void IM_EnQueue(Queue *q, ElemType e) //��Ԫ��e����  
{  
    if((q->rear + 1) % MAXQUEUELEN == q->front) //��������  
        return;  
    q->data[q->rear] = e; //Ԫ��e����  
    q->rear = (q->rear + 1) % MAXQUEUELEN; //�α�rear��ǰ��һλ,����Ѵ����,���Ƶ�ǰ��  
}  

void IM_DeQueue(Queue *q, ElemType *e) //��ͷ��Ԫ�س��Ӵ���*e  
{  
    if(q->rear == q->front) //�������Ϊ�շ���  
        return;  
    *e = q->data[q->front]; //���ض�ͷ��Ԫ��  
    q->front = (q->front + 1) % MAXQUEUELEN; //�α�front��ǰ��һλ,����Ƕ��е�ĩβ�ƶ�����ǰ��  
} 

bool IM_IsEmpty(Queue *q) //�ж϶����Ƿ�Ϊ��  
{  
    return q->front == q->rear ? true : false;  
} 

int IM_GetQueueLength(Queue *q) //���ض��еĳ���  
{  
    return (q->rear - q->front + MAXQUEUELEN) % MAXQUEUELEN;  
} 

void IM_Clear(Queue *q) //��ն���  
{  
    q->front = q->rear = 0;  
} 

void IM_Print(Queue *q) //��ӡ����  
{  
#if 0
    if(q->front == q->rear)  
        return;  
    else if(q->rear < q->front)  
    {  
        for(int i = q->front;i < MAXQUEUELEN;++i)  
            printf("%d ",q->data[i]);  
        for(int i = 0;i < q->rear;++i)  
            printf("%d ",q->data[i]);  
        printf("\n");  
    }  
    else{  
        for(int i = q->front;i < q->rear;++i)  
            printf("%d ",q->data[i]);  
        printf("\n");  
    }  
#endif
}  


