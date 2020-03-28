#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

int no_of_threads, size_of_barrier;
int count = 0;
int barrier = 0;
int invoke_barrier = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  end_condition  = PTHREAD_COND_INITIALIZER;

//Initialisation of the barrier
void init_barrier(int threads) {
    if( no_of_threads < size_of_barrier ){
        barrier = no_of_threads;
        return;
    }

    barrier = threads;
}




//decreases the count by 1

int decrease_counter()
{
    if (barrier == 0) { return 0; }

    if(pthread_mutex_lock(&lock) != 0){

        perror("Unable to take the lock");//prints error message
        return -1;
    }

    barrier--;

    if(pthread_mutex_unlock(&lock) != 0) {
        perror("Unable to unlock");//prints error message
        return -1;
    }

    return 0;
}




//The barrier_wait funtion will make other threads 
//wait until the last thread is reached

int barrier_wait()
{
    if(decrease_counter() < 0) {  return -1; }

    while (barrier)
    {
        if(pthread_mutex_lock(&lock) != 0){
            perror("  Cannot lock mutex");
            return -1;
        }

        if(pthread_cond_wait(&end_condition, &lock) != 0){
            perror("  Something went wrong in waiting codition");
            return -1;
        }
    }

   //This is excuted by the nth thread
    if(0 == barrier)
    {
        if(pthread_mutex_unlock(&lock) != 0) {
            perror("  Mutex is unable to be locked");
            return -1;
        }
        if(pthread_cond_signal(&end_condition) != 0) {
            perror("  Unable to signal");
            return -1;
        }
    }

    return 0;
}

void * at_barrier(void *nthreads)
{

     int rand_seconds = rand() % 5;

     printf("\n");
     printf("Thread number: %d",++count);
     printf("\n");
     printf("Ongoing init task having length %d seconds.\n",rand_seconds);

     sleep(rand_seconds);

     barrier_wait();
     if (size_of_barrier!=0)
      {
          int increment_invoke_barrier = invoke_barrier++;
          int total_threads = no_of_threads - increment_invoke_barrier;
          int cond_check = total_threads % size_of_barrier;

       if (cond_check == 0) {
         printf("\nThe barrier is opened and all the threads are released\n");
       }
       printf("\nThis task is performed after the barrier is opened\n");

     }
 return  NULL;
}




int main()
{

    int i, j;
    printf("Enter the size of barrier: ");
    scanf("%d", &size_of_barrier);

    printf("Enter the number of threads: ");
    scanf("%d", &no_of_threads);

    if (size_of_barrier>=0 && no_of_threads>=0) 
    {
        pthread_t tid[no_of_threads];

        init_barrier(size_of_barrier);

        for(i = 0; i < no_of_threads; i++) {
            pthread_create(&(tid[i]), NULL, &at_barrier, &no_of_threads);
        }


        for(j = 0; j < no_of_threads; j++) {
            pthread_join(tid[j], NULL);
        }
    }

    else
    {
      printf("Please check the entered input");//when wrong input is entered 
      printf("\n");
      main();//ask the same questions again
    }

    return 0;
}
