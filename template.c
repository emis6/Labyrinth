//
// TEMPLATE 
//
//
// Permet de jouer un seul tour (en ne faisant rien s'il commence ou en 
// réceptionnant le coup de l'adversaire s'il ne commence pas) 
// et termine le jeu.
// Ce programme vous sert de base pour construire votre propre programme



#include <stdio.h>
#include <stdlib.h>
#include "labyrinthAPI.h"
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "my_heap.h"

extern int debug;	/* hack to enable debug messages */

/* Structure stockant une position dans le labyrinthe */ 
typedef struct position{
	int x; 
	int y;
}Pos;

/* Structure stockant les données du labyrinthe */
typedef struct donnees_lab{
	char *lab;
	int energy;
	Pos play;
	Pos adv;
	Pos Tresor;
}Lab;

int normal(int x, int y, int lenx, int leny);

int code(int x, int y);

void decode(int z, int* x, int* y);

int** dir = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};/*to move in: left, right, up, down directions*/ ; /*to move in: left, right, up, down directions*/
/*Fonction pour trouver le chemin avec Dijkstra will be used to count h*/
int Dijkstra(Lab* lab, int sizeX, int sizeY , t_return_code ret, int player, t_move move);

/*Fonction pour jouer avec A* simple*/
int A_Star(Lab* lab, int sizeX, int sizeY , t_return_code ret, int player, t_move move);

/*Takes x and y and number of rows of labyrinth then returns the index corresponding to x and y in laby which is a char**/
int ind(int x, int y, int xlen);

/*A function that plays a random move*/
int playRandom(Lab* lab, int sizeX, int sizeY , t_return_code ret, int player, t_move move);

/*Fonction renvoyant le reste de la division euclidienne de a par b:
ici, l'opérateur % ne convient pas car il peut renvoyer des entiers négatifs!
Or le reste d'une division euclidienne est nécessairement positif
 */
int reste(int a, int b);


/* Fonction mettant à jour le labyrinthe s'il y a eu rotation */
int lab_update(char* lab,int size_x, int size_y, t_move m);

/* Fonction mettant à jour une position dans le labyrinthe
(qu'il s'agisse de celle du trésor, de la notre ou celle de l'adversaire)
s'il y a eu rotation et qu'on se trouve sur la même ligne/colonne que celle qui est bougée  */
int pos_update_rotation(Pos* pos,int size_x, int size_y, t_move m);

/* Fonction mettant à jour une position dans le labyrinthe
(qu'il s'agisse de la notre ou celle de l'adversaire)
si on s'est déplacé sur une autre case  */
int pos_update_translation(Pos* pos,int size_x, int size_y, t_move m);

/* Fonction renvoyant un mouvement aléatoire autorisé*/
t_move make_move(Lab* laby, int size_x, int size_y);



int main()
{
	char labName[50];					/* name of the labyrinth */
	char* labData;						/* data of the labyrinth */
	t_return_code ret = MOVE_OK;		/* indicates the status of the previous move */
	t_move move;						/* a move */
	int player;
	int sizeX,sizeY;
	Lab *laby = (Lab*)malloc(sizeof(Lab));
	
	/* connection to the server */
	connectToServer( "pc4023.polytech.upmc.fr", 1234, "prog_mahsh_ani");
	
	/* wait for a game, and retrieve informations about it */
	//waitForLabyrinth( "DO_NOTHING timeout=400", labName, &sizeX, &sizeY);	
	waitForLabyrinth( "PLAY_RANDOM timeout=100 rotation=TRUE", labName, &sizeX, &sizeY);
	labData = (char*) malloc( sizeX * sizeY );
	player = getLabyrinth( labData);
	
	
	/* display the labyrinth */
	printf("sizex: %d \t sizey: %d\n", sizeX,sizeY);

	// Initialisation de laby
	laby -> lab = labData;
	laby -> energy = player;
	laby -> play.x = sizeY/2;
	laby -> adv.x = sizeY/2;
	laby -> Tresor.x = sizeY/2;
	laby -> Tresor.y = sizeX/2;
	

	if (player == 1)
	{
		laby -> play.y = sizeX - 1;
		laby -> adv.y = 0;
	}

	else
	{
		laby -> play.y = 0;
		laby -> adv.y = sizeX - 1;
	}
	
	playRandom(laby,  sizeX,  sizeY, ret, player, move);

	/* we do not forget to free the allocated array */
	free(labData);
	free(laby);
	
	
	/* end the connection, because we are polite */
	closeConnection();
	
	return EXIT_SUCCESS;
}

int Dijkstra(Lab* laby, int sizeX, int sizeY , t_return_code ret, int player, t_move move)
{
	int i, x, y,z;
	int total_places = sizeof(laby->lab)/sizeof('0');
	
	int* visit = (int*)calloc(0, total_places); /*todo: should be #FREE afterwards*/

	p_queue_t *h = (p_queue_t *)calloc(1, sizeof (p_queue_t)); /*priority queue of Dijkstra algo*/

	/*todo free this shit*/
	int* pred = (int *) calloc(-1, total_places*sizeof (int)); /*Holds the predecessor of each node in the path*/


	x = laby->play.x;
	y =laby->play.y;

	push(h, 0, code(x, y));

	while((z = pop(h))!= -1)
	{
		decode(z, &x, &y);
		if(laby->Tresor.x == x && laby->Tresor.y == y)
			break;

		if(normal(x, y, sizeX, sizeY))
		{
			for(i =0; i < 4; i++)
			{
				int newT = ind(x + dir[0][i], y + dir[1][i], sizeX);
				if(visit[newT]!= 1 && laby->lab[newT] != '1')
				{
					pred[newT] = ind(x , y , sizeX) ;
					visit[newT] = 1;
					push(h,0, code(x + dir[0][i], y + dir[1][i]));
				}
			}
		}
		else
		{
			if(x ==1 )
			{
				if(y == 1)/*in upper left corner, exception cases: left and up*/
				{

				}
				else if(y == sizeY)/*in the right up corner, exception cases: right and up*/
				{

				}
				else/*in first row but not in the corners, exception case: up direction*/
				{
					int newT;
					for( i = 0; i < 3; i++)
					{
						newT= ind(x + dir[0][i], y + dir[1][i], sizeX);
						if(visit[newT]!= 1 && laby->lab[newT] != '1')
						{
							pred[newT] = ind(x , y , sizeX) ;
							visit[newT] = 1;
							push(h,0, code(x + dir[0][i], y + dir[1][i]));
						}
					}

					newT = ind(x, sizeY, sizeX);
					if(visit[newT]!= 1 && laby->lab[newT] != '1')
					{
						pred[newT] = ind(x , y , sizeX) ;
						visit[newT] = 1;
						push(h,0, code(x , sizeY-1));
					}
				}
			}
			else if(x == sizeX)
			{
				if(y == 1) /*in the left down corner, exception cases: left, down*/
				{

				}
				else if(y == sizeY)/*in the down right corner, exception cases: right, down*/
				{

				}
				else/*in first row but not in the corners, exception case: down direction*/
				{
					
				}
			}
		}
	}


	return 0;
}

int normal(int x, int y, int lenx, int leny)
{
	if(x >=1 && x <=lenx-1 && y >= 1 && y <= leny-1 )
		return 1;
	return 0;
}

int code(int x, int y)
{
	return (x<<8)|y;
}
void decode(int z, int* x, int* y)
{
	*y = z&127;
	*x = z >>8;
}
int ind(int x, int y, int xlen)
{
	return y * xlen + x -1;
}
int A_Star(Lab* lab, int sizeX, int sizeY , t_return_code ret, int player, t_move move)
{
	return 0;
}

/*The function that plays a random move*/
int playRandom(Lab* laby, int sizeX, int sizeY , t_return_code ret, int player, t_move move)
{
	do
	{
		printLabyrinth();

	    if (player==1)	/* The opponent plays */
		{
			ret = getMove(&move);

			lab_update(laby -> lab, sizeX, sizeY, move);
			pos_update_translation(&(laby->adv), sizeX, sizeY, move);
			pos_update_rotation(&(laby->adv), sizeX, sizeY, move);

		/* On met a jour notre position dans le labyrinthe ainsi que le labyrinthe s'il a effectue une rotation */
			pos_update_rotation(&(laby->Tresor), sizeX, sizeY, move);
			pos_update_rotation(&(laby->play), sizeX, sizeY, move);

		/*C'est a nous de jouer */
			player = 0;
		}

		else
		{
		/*On génère un mouvement puis on met a jour notre structure laby */
			move =  make_move(laby, sizeX, sizeY);
			ret = sendMove(move);
			lab_update(laby -> lab, sizeX, sizeY, move);

			pos_update_translation(&(laby->play), sizeX, sizeY, move);
			pos_update_rotation(&(laby->play), sizeX, sizeY, move);
			pos_update_rotation(&(laby->Tresor), sizeX, sizeY, move);
			pos_update_rotation(&(laby->adv), sizeX, sizeY, move);

			player = 1;
		}

	} while(ret == MOVE_OK);	
	
	if ( (player == 0 && ret == MOVE_WIN) || (player == 1 && ret == MOVE_LOSE ) )
	{
		printf("I lose the game\n");
		return 0;
	}
	else
	{
		printf("I win the game\n");
		return 1;
	}
}

/*Fonction renvoyant le reste de la division euclidienne de a par b:
ici, l'opérateur % ne convient pas car il peut renvoyer des entiers négatifs!
Or le reste d'une division euclidienne est nécessairement positif
 */
int reste(int a, int b)
{
	return ( (a%b + b) % b);
}


/* Fonction mettant à jour le labyrinthe s'il y a eu rotation */
int lab_update(char* lab,int size_x, int size_y, t_move m)
{
  /* indice parcourant la ligne ou la colonne à bouger*/
	int i;

  int tmp; // variable temporaire contenant un élément du tableau

  /*on deplace tous les elements de la ligne m.value vers la gauche */ 
  if (m.type == 0)
  {
      tmp = lab[size_x * m.value]; //tmp contient le premier element de la ligne à bouger
      for (i=0; i< size_x - 1; i++)
      {
      	lab[ size_x * m.value + i] =  lab[ size_x * m.value + i +1 ];
      }
      lab[ size_x * (m.value+1) -1 ] = tmp;
  }

   /*on deplace tous les elements de la ligne m.value vers la droite */
  if (m.type == 1)
  {
      tmp = lab[size_x * (m.value+1) -1]; //tmp contient le dernier element de la ligne à bouger
      for (i=size_x-1; i> 0; i--)
      {
      	lab[ size_x * m.value + i] =  lab[ size_x * m.value + i -1 ];
      }
      lab[ size_x * m.value ] = tmp;
  }

     /*on deplace tous les elements de la colonne m.value vers le haut */
  if (m.type == 2)
  {
	tmp = lab[m.value]; //tmp contient le premier element de la colonne à bouger
	for (i=0; i< size_y - 1; i++)
	{
		lab[ size_x * i + m.value] =  lab [size_x * (i +1) + m.value];
	}
	lab[ size_x *(size_y -1) + m.value ] = tmp;
}

     /*on deplace tous les elements de la colonne m.value vers le bas */
if (m.type == 3)
{
	tmp = lab[size_x *(size_y -1) + m.value]; //tmp contient le dernier element de la colonne à bouger
	for (i=size_y-1; i>0; i--)
	{
		lab[ size_x * i + m.value] =  lab [size_x * (i -1) + m.value];
	}
	lab[m.value] = tmp;
}
return 0;

}

/* Fonction mettant à jour une position dans le labyrinthe
(qu'il s'agisse de celle du trésor, de la notre ou celle de l'adversaire)
s'il y a eu rotation et qu'on se trouve sur la même ligne/colonne que celle qui est bougée  */

int pos_update_rotation(Pos* pos,int size_x, int size_y, t_move m)
{

	if (m.type == 0 && m.value == pos->x)
		pos->y = reste(pos->y - 1, size_x);

	if (m.type == 1 && m.value == pos->x)
		pos->y = reste(pos->y + 1, size_x);

	if (m.type == 2 && m.value == pos->y)
		pos->x = reste(pos->x - 1,size_y);

	if (m.type == 3 && m.value == pos->y)
		pos->x = reste(pos->x + 1 , size_y);

	return 0;

}

/* Fonction mettant à jour une position dans le labyrinthe
(qu'il s'agisse de la notre ou celle de l'adversaire)
si on s'est déplacé sur une autre case  */

int pos_update_translation(Pos* pos,int size_x, int size_y, t_move m)
{
	if (m.type == 4)
		pos->x = reste(pos->x - 1,size_y); 

	if (m.type == 5) 
		pos->x = reste(pos->x + 1,size_y);

	if (m.type == 6)
		pos->y = reste(pos->y - 1,size_x);

	if (m.type == 7)
		pos->y = reste(pos->y + 1,size_x);

	return 0;  
}


/* Fonction renvoyant un mouvement aléatoire autorisé*/
t_move make_move(Lab* laby, int size_x, int size_y)
{
	srand(time(NULL));
  /* booleen verifiant la validite du mouvement tiré aleatoirement */
	int check = 0;

  /* mouvement */
	t_move m;

  /*Position de notre joueur */
	int x = laby -> play.x ;
	int y = laby -> play.y;

  /* tant que le mouvement tiré n'est pas valide, on tire un autre mouvement aleatoirement */
	while(check == 0)
	{
      /* On tire un mouvement au hasard */

		m.type = rand() % 9;

      /* Si le mouvement est une rotation, on tire au hasard la ligne ou la colonne à tourner */

		if(m.type >= 0 && m.type <= 3 && laby->energy >= 5)
		{
			laby -> energy -= 5;
			check = 1;

			if (m.type == 0 || m.type == 1)
				m.value = rand() % size_y;

			else
				m.value = rand() % size_x;

		}


      /*Sinon on vérifie que le joueur a le droit d'effectuer le mouvement tiré au sort: 
	il ne doit pas y avoir de murs à l'endroit où l'on se rend */
		else 
		{
			if ( (m.type == 4 && laby -> lab[ size_x * reste(x-1,size_y) + y ] == 0)
				|| (m.type == 5 && laby -> lab[ size_x * reste(x+1,size_y) + y ] == 0)
				|| (m.type == 6 && laby -> lab[ size_x * x + reste(y - 1,size_x) ] == 0)
				|| (m.type == 7 && laby -> lab[ size_x * x + reste(y + 1,size_x) ] == 0)  )

			{
				check = 1 ;
				(laby -> energy) ++;
			}
		}

	}

	return m;

} 


