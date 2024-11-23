#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define Taille 1000 
int valeur = 0;       
int n;                        
int tab_sortie[Taille];      
int index = 0;      

sem_t semaph_thre1;      
sem_t semaph_thre2;      


void *incrementer(void *arg) {
    for (int i = 1; i <= n; i++) {
        sem_wait(&semaph_thre1);    
        valeur++;               
        tab_sortie[index++] = valeur; 
        printf("Thread 1: %d\n", valeur);
        sem_post(&semaph_thre2);    
    }
    return NULL;
}

void *decrementer(void *arg) {
    for (int i = 1; i <= n; i++) {
        sem_wait(&semaph_thre2);    
        valeur--;               
        tab_sortie[index++] = valeur; 
        printf("Thread 2: %d\n", valeur);
        sem_post(&semaph_thre1);    
    }
    return NULL;
}

int main() {
    printf("Entrez la valeur de n : ");
    scanf("%d", &n);
    if (n * 2 > Taille) {
        printf("Erreur : n est trop grand pour le tableau (%d max).\n", Taille / 2);
        return 1;
    }

    pthread_t thread1, thread2;

    
    sem_init(&semaph_thre1, 0, 1);  
    sem_init(&semaph_thre2, 0, 0);  

    
    pthread_create(&thread1, NULL, incrementer, NULL);
    pthread_create(&thread2, NULL, decrementer, NULL);

    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    
    sem_destroy(&semaph_thre1);
    sem_destroy(&semaph_thre2);

    
    int fichier = open("tab_sortie.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fichier < 0) {
        perror("Erreur lors de l'ouverture du fichier");
        return 1;
    }

    
    for (int i = 0; i < index; i++) {
        char buffer[16];
        int longueur = sprintf(buffer, "%d\n", tab_sortie[i]);
        write(fichier, buffer, longueur);
    }

    close(fichier); 

    printf("Les résultats ont été enregistrés dans 'tab_sortie.txt'.\n");

    
    FILE *gnuplot = popen("gnuplot -persistent", "w");
    if (gnuplot == NULL) {
        perror("Erreur lors du lancement de GNUplot");
        return 1;
    }

    fprintf(gnuplot, "set title 'Variation de la valeur partagée'\n");
    fprintf(gnuplot, "set xlabel 'Étapes'\n");
    fprintf(gnuplot, "set ylabel 'Valeur partagée'\n");
    fprintf(gnuplot, "plot 'tab_sortie.txt' with linespoints title 'Valeurs'\n");
    pclose(gnuplot);

    printf("Courbe tracée avec GNUplot.\n");
    return 0;
}
