//SEC - PROJET MINI SHELL
//CYRIL FIGUIN
//1SN-E
//PROMOTION 2022

//importation des librairies natives
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
//importation des librairies locales
#include "readcmd.h"
#include "gestion_processus.h"

//Initialitation des variables
pid_t pid_actif; //pid Processus
Liste_Chainee_Processus processus; //liste des processus
sigset_t signaux; //signaux
char ***sequence; //Séquence de caractères qui sera utilisé dans le programme principal

void ctrlZHandler(int num_signal) {			// Gérer le Ctrl Z
	ajouterProcessus(sequence[0][0], pid_actif, Suspendu, &processus);
	kill(pid_actif, SIGSTOP);
	printf("\n");
}

void ctrlCHandler(int num_signal) {			// Gérer le Ctrl C
	if (existe(pid_actif, processus)) {
		retirerProcessus(pid_actif, &processus);
	}
	kill(pid_actif, SIGKILL);
	printf("\n");
}

void filsTermineHandler() 	// Gérer l'arrêt d'un processus fils
{	
    int status;
	int pid_fils;
	
	while((pid_fils = waitpid(-1, &status, WNOHANG))!= 0 && pid_fils != -1) {
		if(WIFEXITED(status)) {		
			retirerProcessus(pid_fils, &processus);
		}
	}
}

void handler(int sig)
{
    int status;
    pid_t pid;
	while((pid = waitpid(-1,&status,WCONTINUED | WUNTRACED | WNOHANG)) > 0) {
		if (WIFSIGNALED(status)|WIFEXITED(status)) {
			retirerProcessus(pid,&processus);
		}
		else if (WIFSTOPPED(status)) {
			suspendreProcessus(pid,&processus);
		}
		else if (WIFCONTINUED(status)) {
			reprendreProcessus(pid,&processus);
		}
	}
}

//Programme principal
int main(void) 
{
	processus = NULL; //Initialisation a NULL de la liste des processus
    struct cmdline* entree_utilisateur; //ENTREE DE L'UTILISATEUR

    sigemptyset(&signaux);
	sigaddset(&signaux , SIGTSTP);
	sigaddset(&signaux , SIGINT);

   	signal(SIGCHLD,handler);
    signal(SIGTSTP,&ctrlZHandler);
	signal(SIGINT,&ctrlCHandler);
	signal(SIGCHLD,&filsTermineHandler);

    printf("Bienvenue sur MiniShell")
    while(1)
    {
        printf("COMMANDE>> ");
		entree_utilisateur = readcmd();
        sequence = entree_utilisateur -> seq;
        if (sequence[0] != NULL) {	// L'utilisateur a rentré quelque chose
            
			if (strcmp(sequence[0][0], "cd") == 0) {		// COMMANDE cd
				if (sequence[0][1] == NULL) {				
					chdir(getenv("HOME"));
				}
				else {										
					chdir(sequence[0][1]);
				}
			}
			if (strcmp(sequence[0][0], "exit") == 0) {	// COMMANDE exit
				if (sequence[0][1] == NULL) {				
					exit(0);
				}
				else {										
					exit(atoi(sequence[0][1]));
				}
			}
            if (strcmp(sequence[0][0], "fg") == 0) {	// COMMANDE fg
				if (existeProcessus(pid_actif,processus)
                {
                    retirerProcessus(processus, pid_actif);
                    filsTermineHandler();
                }
			}
			if (strcmp(sequence[0][0], "cont") == 0) {
				pid_t pid_kill = IDtoPID(processus,atoi(sequence[0][1]));
				kill(pid_kill,SIGCONT);
			}

			if (strcmp(sequence[0][0], "jobs") == 0) { 					// Cas de la commande jobs
				afficher_processus(processus);						// On affiche la liste des processus actifs
			}

			if (strcmp(sequence[0][0], "stop") == 0) {
				pid_t pid_kill = IDtoPID(processus,atoi(sequence[0][1]));
				kill(pid_kill,SIGSTOP);
            }
        }
        else
        {
            int i = 0;
            int descripteur1 = 0;
            int descripteur2 = 1;

            /* Redirections entrée sorties si nécessaire*/
            if (entree_utilisateur -> in != NULL)  // Cas où l'entrée doit être redirigée
            { 		
                descripteur1 = open(entree_utilisateur -> in, O_RDONLY);
                if (descripteur1 == -1) {
                    perror("open(in)");
                }
			}
            if (entree_utilisateur -> out != NULL)  // Cas où l'entrée doit être redirigée
            { 		
                descripteur2 = open(entree_utilisateur -> out,  O_WRONLY | O_CREAT | O_TRUNC, 0644));
                if (descripteur2 == -1) {
                        perror("open(out)");

                }
			}
            int ancien_tube[2];
            int nouveau_tube[2];
            if (pipe(nouveau_tube) < 0)
            {
                perror("pipe");
            }

            pid_t pid_fils;
            do {
                ancien_tube[0]=nouveau_tube[0];
                ancien_tube[1]=nouveau_tube[1];

                if (pipe(current_pipe) < 0) // ERREUR
                {
                    perror("pipe");
                }

                //duplication processus
                pid_fils=fork();
                if (child_pid < 0)
                {
                    perror("fork");
                    return;
                }
                else if (pid_fils==0)
                {
                    sigprocmask(SIG_BLOCK, &signaux, NULL);

                    if (ancien_tube[1]) {	// Gestion des entrées et des sorties
                        close(ancien_tube[1]);
                    }	
                    if (ancien_tube[0]) {
                        dup2(ancien_tube[0], 0);
                    }
                    if (nouveau_tube[0]) {
                        close(nouveau_tube[0]);
                    }	
                    if (sortie[1]) {
                        dup2(sortie[1], 1);
                    }

                    execvp(sequence[i][0],sequence[i]);
                    printf("%s : commande inconnue\n", sequence[i][0]);
                    exit(1);
                }
                else // execution processus pere
                {
                    
                    setpgid(child_pid, child_pid);
                    close(prec_pipe[0]);
                    close(prec_pipe[1]);
                    close(current_pipe[1]);
                }
                ++i;
            } while (sequence[i] != NULL)

            //on s'assure que tous les tubes sont fermés
            close(prec_pipe[0]);
            close(prec_pipe[1]);
            close(current_pipe[1]);
            close(current_pipe[0]);
            
            
            if (ligne_cmd -> backgrounded == NULL) {
				filsTermineHandler();		
            }
            else {
                inserer_processus(sequence[0][0], pidFils, Actif, &list_process);
            }
        }
    }
    return EXIT_SUCCESS;
}
