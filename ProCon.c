#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>



#define tableSize 2
const int numOfData = 10;

int table[tableSize];
sem_t isEmpty;
sem_t isFull;
int in = 0;
int out = 0;
pthread_mutex_t mutex;


// Consumer to consume data. Consumer will not consume data if there is no data to consume
void *consumer() {
    for(int i = 0; i < numOfData; i++) {
        // Wait until table is full
        sem_wait(&isFull);

        // Lock mutex
        pthread_mutex_lock(&mutex);
        // Get data
        int data = table[out];
        printf("Consumer: Remove data %d from table slot %d\n", data, out + 1);
        // Get next slot in table
        out = (out + 1) % tableSize;
        // Unlock mutex
        pthread_mutex_unlock(&mutex);

        // Post that consumer is empty
        sem_post(&isEmpty);
    }
}


// Producer to produce data. Producer should not produce data if the table is filled.
void *producer() {
    for(int i = 0; i < numOfData; i++) {
        // Generate a random number for data
        int data = rand();

        // Wait until consumer is empty
        sem_wait(&isEmpty);

        // Lock mutex
        pthread_mutex_lock(&mutex);
        // Put data in empty spot in table
        table[in] = data;
        printf("Producer: Insert data %d in table slot %d of %d\n", table[in], in + 1, tableSize);
        // Get next slot in table
        in = (in + 1) % tableSize;
        // Unlock mutex
        pthread_mutex_unlock(&mutex);

        // Post that table is full
        sem_post(&isFull);
    }
}


int main() {
    // Initilize mutex
    pthread_mutex_init(&mutex, NULL);
    // isEmpty used to see if consumer is empty
    sem_init(&isEmpty, 0, tableSize);
    // isFull to see if table is full
    sem_init(&isFull, 0, 0);

    // Create producer and consumer threads
    pthread_t producerThread, consumerThread;
    pthread_create(&producerThread, NULL, producer, NULL);
    pthread_create(&consumerThread, NULL, consumer, NULL);

    // Join threads
    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    // Cleanup
    pthread_mutex_destroy(&mutex);
    sem_destroy(&isEmpty);
    sem_destroy(&isFull);

    return 0;
}