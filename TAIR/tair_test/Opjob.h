#ifndef __OpJOB_H__
#define __OpJOB_H__
class OpJob 
{
public:
    OpJob(int keynum,int flag);
    OpJob(const OpJob & job);
    void addKey(char* key, int size);
    void add(char* key, int key_len, char* val=NULL, int val_len=0);
    int getSize();
    char* getKey(int index);
    char** getAllKey();
    char** getAllVal();
    ~OpJob();
    int _flag;  // 0: put 1: get 2: delete
private:
    char** _keys;
    char** _vals;
    int _index;
    int _keynum;
};
#endif