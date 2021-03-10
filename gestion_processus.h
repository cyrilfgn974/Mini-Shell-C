#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#define MAX_L 100

typedef enum {Actif, Suspendu} Etat;    //Etats d'un processus

typedef struct processus *Liste_Chainee_Processus;

struct processus {
	int id;
	pid_t pid;
    char commande[MAX_L];
	Etat etat;
	Liste_Chainee_Processus Suivant;
};
	
int ajouterProcessus(pid_t pid, char *commande, Etat etat, Liste_Chainee_Processus *liste); 		// Ajouter un processus à une liste de processus

int retirerProcessus(pid_t pid, Liste_Chainee_Processus *liste);		// Supprimer un processus

int suspendreProcessus(pid_t pid, Liste_Chainee_Processus *liste); 	// Suspendre un processus

int reprendreProcessus(pid_t pid, Liste_Chainee_Processus *liste);

void miseaJourEtatProcessus(Liste_Chainee_Processus *liste, pid_t pid, Etat etat);  //mise a jour de l'état d'un processus

int existeProcessus(int pid, Liste_Chainee_Processus liste); 	// Vérifier si un processus existe

void afficher_processus(Liste_Chainee_Processus liste); 	// Afficher les processus d'une liste donnée 

pid_t id2pid(int id, Liste_Chainee_Processus liste); 		// Associer un pid à un id

bool contProcessus(Liste_Chainee_Processus liste, pid_t pid); 