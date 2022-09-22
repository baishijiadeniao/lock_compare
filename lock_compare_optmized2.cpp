#include<stdio.h>
#include<sys/time.h>
#include<atomic>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
// #include <asm/atomic.h>

//原子操作
#define ATOM_INC(ptr) __sync_add_and_fetch(ptr, 1)

#define num 8
typedef struct 
{   
    int32_t pd[15];  //用来填充
    int32_t value=0;   //真正用来计数
}counter_t;

//加入volatile防止编译器优化
volatile counter_t counter[num];
pthread_spinlock_t spinlock;
pthread_mutex_t mutexlock;

double gettime(struct timespec* begin,struct timespec* end){
    long sec=end->tv_sec-begin->tv_sec;
    long nsec=end->tv_nsec-begin->tv_nsec;
    return sec*1000+nsec/1000000;
}

void* increase_count_without_lock(int idx){
    counter[idx].value++;
}

void* increase_count_atomicly(int idx){
    ATOM_INC(&counter[idx].value);
}

void* increase_count_with_spin_lock(int idx){
    counter[idx].value++;
}

void* increase_count_with_mutex_lock(int idx){
    counter[idx].value++;
}

void* run(void* data){
    int idx = (intptr_t)data;
    for(int j=0;j<10000000;j++){
        //只需要将此函数改为想要测试的函数就可以了
        increase_count_without_lock(idx);
    }
}

int sum_count(){
    int sum=0;
    for(int i=0;i<num;i++){
        sum+=counter[i].value;
    }
    return sum;
}

int main(){
    int i;
    double t;
    pthread_t threads[num];
    pthread_spin_init(&spinlock,0);
    pthread_mutex_init(&mutexlock,0);
    struct timespec begin,end;

    clock_gettime(CLOCK_MONOTONIC,&begin);
    for(i=0;i<num;i++){
                //只需要将线程执行函数改为想要测试的函数就可以了
        pthread_create(&threads[i],NULL,run,(void*)(intptr_t)i);
        // pthread_create(&threads[i],NULL,run,(void*)&i);  //不知道为甚么传递指针结果会出错
    }
    for(i=0;i<num;i++){
        pthread_join(threads[i],NULL);
    }
    // increase_count_without_lock(0);
    // increase_count_without_lock(0);
    // increase_count_without_lock(0);
    // increase_count_without_lock(0);
    // increase_count_without_lock(0);
    // increase_count_without_lock(0);
    // increase_count_without_lock(0);
    // increase_count_without_lock(0);
    clock_gettime(CLOCK_MONOTONIC,&end);
    t=gettime(&begin,&end);
    pthread_spin_destroy(&spinlock);
    pthread_mutex_destroy(&mutexlock);
    printf("count=%ld time=%.0fms\n",sum_count(),gettime(&begin,&end));
}