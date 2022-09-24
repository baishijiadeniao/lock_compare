#include<stdio.h>
#include<sys/time.h>
#include<atomic>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
// #include <asm/atomic.h>

//原子操作
#define ATOM_INC(ptr) __sync_add_and_fetch(ptr, 1)

//加入volatile防止编译器优化
volatile uint64_t counter=0;
pthread_spinlock_t spinlock;
pthread_mutex_t mutexlock;

double gettime(struct timespec* begin,struct timespec* end){
    long sec=end->tv_sec-begin->tv_sec;
    long nsec=end->tv_nsec-begin->tv_nsec;
    return sec*1000+nsec/1000000;
}

void* increase_count_without_lock(void* data){
    for(int j=0;j<10000000;j++)
        counter++;
}

void* increase_count_atomicly(void* data){
    for(int j=0;j<10000000;j++)
        ATOM_INC(&counter);
}

void* increase_count_with_spin_lock(void* data){
    pthread_spin_lock(&spinlock);
    for(int j=0;j<10000000;j++)
        counter++;
    pthread_spin_unlock(&spinlock);
}

void* increase_count_with_mutex_lock(void* data){
    pthread_mutex_lock(&mutexlock);
    for(int j=0;j<10000000;j++)
        counter++;
    pthread_mutex_unlock(&mutexlock);
}

int main(){
    const int num=8;
    int i;
    double t;
    pthread_t threads[num];
    pthread_spin_init(&spinlock,0);
    pthread_mutex_init(&mutexlock,0);
    struct timespec begin,end;

    clock_gettime(CLOCK_MONOTONIC,&begin);
    for(i=0;i<num;i++){
            //只需要将线程执行函数改为想要测试的函数就可以了
        pthread_create(&threads[i],NULL,increase_count_without_lock,NULL);
    }
    for(i=0;i<num;i++){
        pthread_join(threads[i],NULL);
    }
    //单线程
    // increase_count_without_lock();
    // increase_count_without_lock();
    // increase_count_without_lock();
    // increase_count_without_lock();
    // increase_count_without_lock();
    // increase_count_without_lock();
    // increase_count_without_lock();
    // increase_count_without_lock();
    clock_gettime(CLOCK_MONOTONIC,&end);
    t=gettime(&begin,&end);
    pthread_spin_destroy(&spinlock);
    pthread_mutex_destroy(&mutexlock);
    printf("count=%ld time=%.0fms\n",counter,gettime(&begin,&end));
}