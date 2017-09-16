
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "Opjob.h"

OpJob::OpJob(int keynum,int flag = -1)
{
    _keys = new char*[keynum];
    _keynum = keynum;
    _index = 0;
    _flag = flag;
    if(_flag == 0)
    {
        _vals = new char*[keynum];
    }
    else
    {
        _vals = NULL;
    }
}
OpJob::~OpJob() 
{
    if (_keys != NULL) 
    {
        for(int i = 0; i < _index; i++) 
        {
            if (_keys[i] != NULL) 
            {
                delete [](_keys[i]);
            }
        }
        delete []_keys;
    }
    if(_vals != NULL)
    {
        for(int i=0; i<_index; i++)
        {
            if(_vals[i] != NULL)
            {
                delete [](_vals[i]);
            }
        }
        delete []_vals;
    }
}
void OpJob::addKey(char* key, int size) 
{
    if (size <= 0 || key == NULL)
    {
        return;
    }
    _keys[_index] = new char[size];
    if (_keys[_index] == NULL) 
    {
        return;
    }
    memcpy(_keys[_index++], key, size);
}
void OpJob::add(char* key, int key_len, char* val, int val_len)
{
    if(key_len  <=0 || key == NULL)
    {
        return;
    }
    _keys[_index] = new char[key_len];
    memcpy(_keys[_index], key, key_len);
    if(val_len <= 0 || val == NULL)
    {
        _index++;
        return;
    }
    _vals[_index] = new char[val_len];
    memcpy(_vals[_index++], val, val_len);
}
char* OpJob::getKey(int index) 
{
    return _keys[index];
}
char** OpJob::getAllKey() 
{
    return _keys;
}
char** OpJob::getAllVal()
{
    return _vals;
}
int OpJob::getSize() 
{
    return _index;       
}