
PROGRAM := im_update                       # ���Ŀ�ִ���ļ�

SOURCES := $(wildcard *.c)                 #ͨ�䵱ǰĿ¼��˵�е�.c�ļ�
OBJS    := $(patsubst %.c,%.o,$(SOURCES))  #��$(SOURCES)�еı������Ϻ�׺��.c��ȫ���滻��.o��

.PHONY: clean 

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
%.o: %.c
	$(CC) -c $^ $(CFLAGS) $(INCLUDE)            # $@:��ǰ�����Ŀ���ļ����� $<:�����б��еĵ� һ�������ļ����� $^:�����������б����������������ظ����ļ�������

clean:
	rm *.o $(PROGRAM) -f

