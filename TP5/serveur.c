/*
 **********************************************************
 *
 *  Programme : serveur.c
 *
 *  ecrit par : LP.
 *
 *  resume :    recoit une chaine de caracteres du programme client
 *                en mode connecte
 *
 *  date :      25 / 01 / 06
 *
 ***********************************************************
 */

/* include generaux */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* include fonctions TCP */
#include "fonctionsTCP.h"
#include "donnee.h"

/* taille du buffer de reception */
#define TAIL_BUF 100

main(int argc, char** argv) {
	Data a;
	Resultat res;
	pid_t pid;
  int sock_cont, 
      sock_trans,       /* descripteurs des sockets locales */
      err;	            /* code d'erreur */

  struct sockaddr_in nom_transmis;	/* adresse de la socket de */
					                     /* transmission */
  
  char            buffer[TAIL_BUF];	/* buffer de reception */
  
  socklen_t      size_addr_trans;	/* taille de l'adresse d'une socket */
  
  
  
  /*
   * verification des arguments
   */
  if (argc != 2) {
    printf ("usage : serveur no_port\n");
    exit(1);
  }
  
  size_addr_trans = sizeof(struct sockaddr_in);
  
  /* 
   * creation de la socket, protocole TCP 
   */
  printf("serveur : creation de la socket sur %d\n", atoi(argv[1]));
  sock_cont = socketServeur(atoi(argv[1]));
  if (sock_cont < 0) {
    printf("serveur : erreur socketServeur\n");
    exit(2);
  }
  
	//while
	//accept
	//fork
	//fils => traitement
	//close
	//pere close
  //FORK
  pid = fork();
  
  switch(pid) {
  	case 0:
  		// PER
	  	/*
		* attente de connexion
		*/
		sock_trans = accept(sock_cont, 
				  (struct sockaddr *)&nom_transmis, 
				  &size_addr_trans);
		if (sock_trans < 0) {
			perror("serveur :  erreur sur accept");
			close(sock_cont);
			exit(3); 
		}

	  	break;
  	case -1:
  		//error
  		break;
	default:
	 	//FILS = RECV + SEND
		/*
		* reception et affichage du message en provenance du client
		*/
		err = recv(sock_trans, &a, sizeof(Data), 0);
		if (err < 0) {
		perror("serveur : erreur dans la reception");
		shutdown(sock_trans, 2); close(sock_trans); close(sock_cont);
		exit(4);
		}

		res.error = 0;
		res.result = 0;
		switch(a.operateur) {
			case 0:
				res.result = a.op1 + a.op2;
				break;
			case 1:
				res.result = a.op1 - a.op2;
				break;
			case 2:
				res.result = a.op1 * a.op2;
				break;
			case 3:
				if(a.op2 == 0) {
					res.error = -2;
				} else {
					res.result = a.op1 / a.op2;
				}
				break;
			default:
				res.error = -1;
				break;
		}

		err = send(sock_trans, &res, sizeof(Data), 0);
		if (err < 0) {
		perror("client : erreur sur le send");
		shutdown(sock_trans, 2); close(sock_trans);
		exit(3);
		}

		shutdown(sock_trans, 2);
		break;
  }
  
 
  
  /* 
   * arret de la connexion et fermeture
   */
  close(sock_trans);
  close(sock_cont);
}
