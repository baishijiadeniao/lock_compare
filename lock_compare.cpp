#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <functional>
// #include <asm/atomic.h>

//原子操作
#define ATOM_INC(ptr) __sync_add_and_fetch(ptr, 1)

//加入volatile防止编译器优化
volatile int counter=0;
// std::atomic_int a_counter(0);
pthread_spinlock_t spinlock;
pthread_mutex_t mutexlock;
//需要执行的函数

double gettime(struct timespec* begin,struct timespec* end){
    long sec=end->tv_sec-begin->tv_sec;
    long nsec=end->tv_nsec-begin->tv_nsec;
    return sec*1000+nsec/1000000;
}

void increase_count_without_lock(){
    counter++;
}

void increase_count_atomicly(){
    ATOM_INC(&counter);
}

void increase_count_with_spin_lock(){
    pthread_spin_lock(&spinlock);
    counter++;
    pthread_spin_unlock(&spinlock);
}

void increase_count_with_mutex_lock(){
    pthread_mutex_lock(&mutexlock);
    counter++;
    pthread_mutex_unlock(&mutexlock);
}


void* run(void* data){
    for(int j=0;j<10000000;j++){
        //只需要将此函数改为想要测试的函数就可以了
        increase_count_without_lock();
    }
}

int main(int argc,char* argv[]){
    const int num=8;
    int i;
    double t;
    pthread_t threads[num];
    pthread_spin_init(&spinlock,0);
    pthread_mutex_init(&mutexlock,0);
    struct timespec begin,end;

    clock_gettime(CLOCK_MONOTONIC,&begin);
    //多线程创建线程
    // for(i=0;i<num;i++){
    //     pthread_create(&threads[i],NULL,run,NULL);
    // }
    // for(i=0;i<num;i++){
    //     pthread_join(threads[i],NULL);
    // }
    //单线程
    for(int j=0;j<80000000;j++){
        increase_count_without_lock();
    }
    clock_gettime(CLOCK_MONOTONIC,&end);
    t=gettime(&begin,&end);
    pthread_spin_destroy(&spinlock);
    pthread_mutex_destroy(&mutexlock);
    printf("count=%d time=%.0fms\n",counter,gettime(&begin,&end));
}