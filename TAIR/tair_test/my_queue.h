#ifndef __MY_Queue__
#define __MY_Queue__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <unistd.h>
template <class T>
class MY_Queue 
{
public:
    MY_Queue() {}
    MY_Queue(int size);
    //explicit MY_Queue(const MY_Queue& queue);
    ~MY_Queue();
    int isFull();
    int isEmpty();
    T pop();
    void push(T data); 
private:
    int _start;
    int _end;
    int _size;
    T* _buff;
};
template <class T>
MY_Queue<T>::MY_Queue(int size) 
{
    _start = 0;
    _end = 0;
    _size = size + 1;
    _buff = new T[_size];
}

template <class T>
MY_Queue<T>::~MY_Queue() 
{
    _start = -1;
    _end = -1;
    _size = -1;
    delete []_buff;
}

template <class T>
int MY_Queue<T>::isFull() 
{
    return (_end + 1)%_size == _start;
}

template <class T>
int MY_Queue<T>::isEmpty() 
{
    return _end%_size == _start;
}

template <class T>
T MY_Queue<T>::pop() 
{
    T data = _buff[_start];
    _start = (_start + 1) % _size;
    return data;
}

template <class T>
void MY_Queue<T>::push(T job) 
{
    _buff[_end] = job;
    _end = (_end + 1) % _size;
}

#endif