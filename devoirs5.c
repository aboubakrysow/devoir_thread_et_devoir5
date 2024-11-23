#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


#define max 100

int n;                           
sem_t semaphores[max];   
int iteration = 0;               


void *executer_thread(void *arg) {
    int id = *(int *)arg; 
    while (1) 
    {
        sem_wait(&semaphores[id]);
        printf("Thread %d s'exécute (Itération %d)\n", id + 1, iteration + 1);
        sem_post(&semaphores[(id + 1) % n]);
        if (iteration >= 10 && id == n - 1) 
        {
            break;
        }
        if (id == n - 1) 
        {
            iteration++;
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage : %s <nombre_de_threads>\n", argv[0]);
        return 1;
    }
    n = atoi(argv[1]);

    if (n <= 0 || n > max) {
        printf("Erreur : le nombre de threads doit être entre 1 et %d.\n", max);
        return 1;
    }

    pthread_t threads[n];      // Tableau pour stocker les threads
    int thread_ids[n];         // Identifiants des threads

    // Initialiser les sémaphores
    for (int i = 0; i < n; i++) {
        sem_init(&semaphores[i], 0, 0);
    }

    // Le premier thread peut commencer
    sem_post(&semaphores[0]);

    // Créer les threads
    for (int i = 0; i < n; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, executer_thread, &thread_ids[i]) != 0) {
            perror("Erreur lors de la création d'un thread");
            return 1;
        }
    }

    // Attendre la fin de tous les threads
    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    // Détruire les sémaphores
    for (int i = 0; i < n; i++) {
        sem_destroy(&semaphores[i]);
    }

    printf("Exécution terminée après %d itérations.\n", iteration);
    return 0;
}
