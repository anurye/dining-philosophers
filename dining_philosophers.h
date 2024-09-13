#ifndef DINING_PHILOSOPHERS_H
#define DINING_PHILOSOPHERS_H

#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define NUM_PHILOSOPHERS 5
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define LEFT (i + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS
#define RIGHT (i + 1) % NUM_PHILOSOPHERS

/**
 * @brief Initialize shared memory for containing the critical section.
 */
void initialize_shared_memory();

/**
 * @brief Initialize semaphores for the dining philosophers problem by - 
 * create a group of NUM_PHILOSOPHERS + 1 semaphores
 */
void initialize_semaphores();

/**
 * @brief Simulate dining philosophers using fork().
 */
void simulate_philosophers();

/**
 * @brief Function to be executed by each philosopher process:
 * each philosopher think -> get hungry -> eat -> think 
 * 
 * @param i The index of the philosopher.
 */
void philosopher(int i);

/**
 * @brief Attempt to grab both the left and right forks - which is necessary for a
 * philosopher to eat.
 * 
 * @param i The index of the philosopher.
 */
void grab_forks(int i);

/**
 * @brief Release the grabed forks so that other philosopher can take their turn 
 * and eat.
 * 
 * @param i The index of the philosopher.
 */
void put_away_forks(int i);

/**
 * @brief Test whether a philosopher can eat or not (have two forks or not).
 * 
 * @param i The index of the philosopher.
 */
void test(int i);

/**
 * @brief Simulate the thinking activity of a philosopher, just by printing a message.
 * 
 * @param i The index of the philosopher.
 */
void think(int i);

/**
 * @brief Simulate the eating activity of a philosopher, just by printing a message.
 * 
 * @param i The index of the philosopher.
 */
void eat(int i);

/**
 * @brief Perform "up" operation on a semaphore.
 * 
 * @param sem_num semaphore number.
 */
void up(int sem_num);

/**
 * @brief Perform "down" operation on a semaphore.
 * 
 * @param sem_num semaphore number.
 */
void down(int sem_num);

/**
 * @brief Clean up resources, shared memory and semaphores.
 */
void cleanup_resources();

#endif /* DINING_PHILOSOPHERS_H */
