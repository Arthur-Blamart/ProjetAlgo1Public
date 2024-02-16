// compiler header files
#include <stdbool.h> // bool, true, false
#include <stdlib.h> // rand
#include <stdio.h> // printf
#include <assert.h>// pour les assertions
#include <stdlib.h>//Pour malloc, free et realloc
#include <unistd.h>//Pour test avec sleep

// program header file
#include "bomberman.h"

// global declarations
extern const char BOMBERMAN; // ascii used for bomberman
extern const char WALL; // ascii used for the walls
extern const char BREAKABLE_WALL; // ascii used for the breakable walls
extern const char PATH; // ascii used for the pathes
extern const char EXIT; // ascii used for the EXIT
extern const char BOMB; // ascii used for a bomb
extern const char BOMB_BONUS; // ascii used for the bombs
extern const char FLAME_BONUS; // ascii used for the flames
extern const char FLAME_ENEMY; // ascii used for the flame ennemy
extern const char GHOST_ENEMY; // ascii used for the ghost ennemy

extern const int BOMB_DELAY; // time before a bomb explode
extern const int BREAKABLE_WALL_SCORE; // reward for bombing a breakable wall
extern const int FLAME_ENEMY_SCORE; // reward for bombing a flame enemy
extern const int GHOST_ENEMY_SCORE; // reward for bombing a ghost enemy 
extern const int BOMB_BONUS_SCORE; // reward for a bomb bonus, which increases by 1 the number of bombs that can be dropped at a time
extern const int FLAME_BONUS_SCORE; // reward for a flame bonus, which increses by 1 the explosion range of bombs

extern bool DEBUG; // indicates whether the game runs in DEBUG mode

char * binome="Arthur"; // student names here

// prototypes of the local functions/procedures
void printAction(action);
void printBoolean(bool);

//Type objet qui permet de sauvegarder la nature et la position d'un objet
typedef struct objet{
  char quoi;//Nature de l'objet (le caractère qui le représente)
  int x;
  int y;
}objet;
//Sommet de notre graohe d'objet
typedef struct sommet{
  int nb;//identifiant du sommet de 0 à N-1
  objet quoi;//l'objet designé par le sommet

}sommet;
//Graphe en d'objet avec une matrice d'adjacence
typedef struct graphe{
  int nmax;//c'est le nb de sommet que le graphe peut avoir actuellement
  int n;//C'est le nb de sommet du graphe actuellement
  int** mat;//Matrice d'adjacence
  sommet* sommetList;
}graphe;
//Liste chainée de sommet sert aussi pour des files
typedef struct maillon{
  sommet val;//valeur du maillon
  struct maillon* next;//maillon suivant, NULL par défaut
}maillon;


//Prototype de mes fonctions


/*Fonctions sur la recherche et fuite du danger*/

// la pos (x,y) est-elle safe d'une bombe?
bool isPosSafe(int x, int y, char** map, int mapxsize, int mapysize, int explosion_range, action last_action);
//Permet de savoir si une position suposée safe à un voisin direct safe (règle le problème de se faire coincer dans une case safe entouré par des dangers)
bool isAneigbhourAlsoSafe(int x, int y, char** map, int mapxsize, int mapysize, int explosion_range, action last_action);
//Es qu'on pourrais se cacher à cette case ? (ET logique de isPosSafe et IsANeigbhourAlsoSafe)
bool couldIHidThere(int x, int y, char** map, int mapxsize, int mapysize, int explosion_range, action last_action);
//Si le joueur risque de se faire exploser ou manger on envoit la position de la bombe/enemie sinon on renvoit {-1,-1}
objet whereIsDanger(char** map, int mapxsize, int mapysize, int playerX, int playerY, int explosion_range, action last_action);
//Permet de savoir vers où aller pour être safe
action whereToGoToBeSafe(graphe* g, maillon* listeNord, maillon* listeSud, maillon* listeOuest, maillon* listeEst, char** map,int mapxsize, int mapysize, int x, int y, int explosion_range, action last_action);



/*Fonctions de recherche de bonus*/

//On veut une liste de tous les bonus dans sur la map grace au graphe
maillon* getBonusList(graphe* g);
//Donne l'action pour aller au bonus le plus proches accessbiles et à une certaine distance, renvoit BOMBING si aucun ne correspond
action isBonusAccessibleInRadius(graphe* g, maillon* listeNord, maillon* listeSud, maillon* listeOuest, maillon* listeEst, char** map, int mapxsize, int mapysize, int explosion_range, action last_action, int radius, int x, int y);

/*Fonctions de recherche de sortie*/
action goToExit(graphe* g, maillon* listeNord, maillon* listeSud, maillon* listeOuest, maillon* listeEst, char** map, int mapxsize, int mapysize, action last_action, int explosion_range, int x, int y);

/*Fonctions pour savoir si on peut poser une bombe*/
bool isBombingUseful(char** map, int mapxsize, int mapysize, int x, int y, int explosion_range);

/*Fonctions pour trouver où se replacer*/
action replacementUtil(graphe* g, maillon* listeNord, maillon* listeSud, maillon* listeOuest, maillon* listeEst, char** map, int mapxsize, int mapysize, int x, int y, int explosion_range, action last_action);

//Es ce qu'un enemie est coller au perso
bool enemyColler(int x, int y, char** map);

/*Fonctions lancer quand aucune action n'a pu être trouvée*/
action perduPourPerdu(char** map, int x, int y);




/*Différentes fonctions sur les graphes*/

//Créer le graphe des chemins, inclus bien sur les cases des enemies, des bombes et du joueur
graphe* createPathGraph(char** map, int mapxsize, int mapysize);
/*Ajoute un path et ses liens dans le graphe g (ses liens sont d'ou on peut aller à partir de la en 1 fois)
attention à bien re-récupérer le nouveau graphe pour des soucis de realloc*/
void addPathInGraph(graphe* g, objet path);
//Initalise et renvoit un graphe vide avec une nmax=taille
graphe* initGraph(int taille);
//Augmente la taille du graphe g (la double)
graphe* extendGraph(graphe* g);
//Libere la mémoire allouée pour le graphe g
void libereGraphe(graphe* g);
//Fait un parcours en largeur dans g à partir de s, avec possibilité d'ignorer le joueur ou non
maillon* parcoursLargeur(graphe* g, sommet s, bool ignorePlayer);


/*Différentes fonctions sur les listes chainées et files*/

//Ajoute un maillon à la queue de la liste chainée
void addMaillonToTail(maillon* chaine, maillon* toAdd);
//Ajoute un maillon à la tête de la liste chainée
maillon* addSommetToHead(maillon* chaine, sommet toAdd);
//Ajoute un sommet à la tête de la liste chainée
maillon* addSommetToHead(maillon* chaine, sommet toAdd);
//enfile un sommet dans la file
void enfiler(maillon* file, sommet toAdd);
//Récupère la tête d'une file
sommet tete(maillon* file);
//Défile une pile (renvoit la nouvelle file à récupérer ABSOLUMENT)
maillon* defiler(maillon* file);
//Inverse une liste (en 0(n) temps et rien en espace on récupère les maillons)
maillon* reverseListe(maillon* liste);
//Libère récursivement une liste chainée
void libereList(maillon* chaine);


/*Différentes fonctions de récupérations et des test*/

//Permet de récupérer l'id d'un sommet en connaissant sa position
int getSommetIdByPosition(graphe* g, int x, int y);
//Permet d'obtenir la pos [x,y] d'un joueur
int* getPosjoueur(char** map, int mapxsize, int mapysize);
//donnent dans un tableau les parcours Nord, Sud, Ouest et Est (dans cet ordre)autour de x,y 
maillon** getAll4Parcours(graphe* g, char** map, int x, int y);
//Permet de savoir si la case x y est un chemin en dessous (meme si il y a des choses au dessus)
bool isPath(char** map, int x, int y);
//Es ce que c'est la case ou est le bomberman
bool estJoueur(sommet isHeThere);
//Es ce que a==b (selon leurs positions)
bool isSameSommet(sommet a, sommet b);
//Es ce que le sommet s est celui de la sortie
bool isSortie(sommet s);
//Regarde si item est une bombe (prend en compte si la bombe est au pieds du joueur)
bool isBomb(char item, action last_action);
//Es ce que le char envoyé correspond à celui d'un enemie
bool isEnemy(char item);
//Es ce que le char correspond à un danger cad : enemie ou bombe
bool isDanger(char item, action last_action);

/*Fonctions d'affichages*/

//Affiche la matrice d'adjacence d'un graphe
void AfficheMat(graphe* g);
//Affiche le contenue du graphe
void afficheSommetGraphe(graphe* g);
//Affiche un parcours dans une liste chainée par ses positions
void afficheparcours(maillon* parcours);

/*
  bomberman function:
  This function randomly select a valid move for BOMBERMAN based on its current position on the game map.
 */
action bomberman(
		 char * * map, // array of char modeling the game map
		 int mapxsize, // x size of the map
		 int mapysize, // y size of the map
		 int x, // x position of BOMBERMAN
		 int y, // y position of BOMBERMAN
		 action last_action, // last action made, -1 in the be beginning 
		 int remaining_bombs, // number of bombs
		 int explosion_range // explosion range for the bombs 
		 ){
  action a; // action to choose and return
  
  //Si le radius est à 2, c'est le même que celui d'explosions donc on explose tout le temps
  int radiusBonus1 = 10;
  int radiusBonus2 = 20;
  graphe* g = createPathGraph(map, mapxsize, mapysize);

  maillon** listeParcours = getAll4Parcours(g, map, x, y);
  maillon* listeNord = listeParcours[0];
  maillon* listeSud = listeParcours[1];
  maillon* listeOuest = listeParcours[2];
  maillon* listeEst = listeParcours[3];
  free(listeParcours);

  action resBonus1 = isBonusAccessibleInRadius(g, listeNord, listeSud, listeOuest, listeEst, map, mapxsize, mapysize, explosion_range, last_action, radiusBonus1, x, y);
  action resBonus2 = isBonusAccessibleInRadius(g, listeNord, listeSud, listeOuest, listeEst, map, mapxsize, mapysize, explosion_range, last_action, radiusBonus2, x, y);
  action resSortie = goToExit(g, listeNord, listeSud, listeOuest, listeEst, map, mapxsize, mapysize, last_action, explosion_range, x, y);
  action resReplacement = replacementUtil(g, listeNord, listeSud, listeOuest, listeEst, map, mapxsize, mapysize, x, y, explosion_range, last_action);

  if(!isPosSafe(x, y, map, mapxsize, mapysize, explosion_range, last_action)){
    a = whereToGoToBeSafe(g, listeNord, listeSud, listeOuest, listeEst, map, mapxsize, mapysize, x, y, explosion_range, last_action);
  }
  else if(resBonus1 != BOMBING){
    a = resBonus1;
  }
  else if(resSortie != BOMBING){
    a = resSortie;
  }
  else if(resBonus2 != BOMBING){
    a = resBonus2;
  }
  else if(isBombingUseful(map, mapxsize, mapysize, x, y, explosion_range) && remaining_bombs>0){
    a = BOMBING;
  }
  else if(resReplacement !=BOMBING){
    a = resReplacement;
  }
  else{
    printAction(resReplacement);
    a = perduPourPerdu(map, x, y);
  }


  libereGraphe(g);
  libereList(listeNord);
  libereList(listeSud);
  libereList(listeOuest);
  libereList(listeEst);

  return a; // answer to the game engine
}

/*
  printAction procedure:
  This procedure prints the input action name on screen.
 */
void printAction(action a) {
  switch(a) {
  case BOMBING:
    printf("BOMBING");
    break;
  case NORTH:
    printf("NORTH");
    break;
  case EAST:
    printf("EAST");
    break;
  case SOUTH:
    printf("SOUTH");
    break;
  case WEST:
    printf("WEST");
    break;
  }
}


/*
  printBoolean procedure:
  This procedure prints the input boolan value on screen.
 */
void printBoolean(bool b) {
  if(b==true) {
    printf("true");
  }
  else {
    printf("false");
  }
}

bool isPosSafe(int x, int y, char** map, int mapxsize, int mapysize, int explosion_range, action last_action){
  bool res;
  objet test = whereIsDanger(map, mapxsize, mapysize, x, y, explosion_range, last_action);
  res = (test.x==-1 && test.y==-1) && isPath(map, x, y);
  return res;
}

/*Fonction qui sert à régler problème d'être bloqué dans un coin de 1*/
bool isAneigbhourAlsoSafe(int x, int y, char** map, int mapxsize, int mapysize, int explosion_range, action last_action){
  //On regarde si il y a un voisin safe en haut en bas à gauche ou encore à droite
  bool res = (isPosSafe(x, y-1, map, mapxsize, mapysize, explosion_range, last_action) | isPosSafe(x, y+1, map, mapxsize, mapysize, explosion_range, last_action)
  | isPosSafe(x-1, y, map, mapxsize, mapysize, explosion_range, last_action) | isPosSafe(x+1, y, map, mapxsize, mapysize, explosion_range, last_action));
  return res;
}

bool couldIHidThere(int x, int y, char** map, int mapxsize, int mapysize, int explosion_range, action last_action){
  bool res = isPosSafe(x, y, map, mapxsize, mapysize, explosion_range, last_action) && isAneigbhourAlsoSafe(x, y, map, mapxsize, mapysize, explosion_range, last_action);

  return res;
}

objet whereIsDanger(char** map, int mapxsize, int mapysize, int playerX, int playerY, int explosion_range, action last_action){
  objet bombeOBJ = {.quoi=BOMB, .x=-1, .y=-1};//Position de la bombe initialiser tel qu'on veut renvoyer si on est hors de danger
  int i=0; //itérateur
  int enemyDistance = 1;//La distance ou on va chercher des enemies au max
  bool blockedNord = false;
  bool blockedSud = false;
  bool blockedEst = false;
  bool blockedOuest = false;
  while(i<=explosion_range && (bombeOBJ.x==-1 && bombeOBJ.y==-1)){
    //On vérifie qu'on puisse regarder en haut (attention aux problèmes de dépassement de g->matleau)
    if(playerY-i > 0){
      //On regarde en haut du joueur
      if(!blockedNord){
        if(isBomb(map[playerY-i][playerX], last_action)){
          bombeOBJ.y = playerY-i;
          bombeOBJ.x = playerX;
        }
        else if(i<=enemyDistance && isEnemy(map[playerY-i][playerX])){
          bombeOBJ.y = playerY-i;
          bombeOBJ.x = playerX;
        }
        else if(map[playerY-i][playerX] == WALL ||map[playerY-i][playerX] == BREAKABLE_WALL)
            blockedNord = true;
      }
      //Si le chemin est bloqué on va juste chercher les ghost
      else if(i<=enemyDistance && (map[playerY-i][playerX]==GHOST_ENEMY)){
        bombeOBJ.y = playerY-i;
        bombeOBJ.x = playerX;
      }
    }

    //en Bas
    if(playerY+i < mapysize){
      if(!blockedSud){
        if(isBomb(map[playerY+i][playerX], last_action)){
          bombeOBJ.y = playerY+i;
          bombeOBJ.x = playerX;
        }
        else if(i<=enemyDistance && isEnemy(map[playerY+i][playerX])){
          bombeOBJ.y = playerY+i;
          bombeOBJ.x = playerX;
        }
        else if(map[playerY+i][playerX] == WALL ||map[playerY+i][playerX] == BREAKABLE_WALL)
          blockedSud = true;
      }
      else if((i<=enemyDistance) && (map[playerY+i][playerX]==GHOST_ENEMY)){
        bombeOBJ.y = playerY+i;
        bombeOBJ.x = playerX;
      }
    }

    //a Gauche
    if(playerX-i > 0){
      if(!blockedOuest){
        if(isBomb(map[playerY][playerX-i], last_action)){
          bombeOBJ.y = playerY;
          bombeOBJ.x = playerX-i;
        }
        else if(i<=enemyDistance && isEnemy(map[playerY][playerX-i])){
          bombeOBJ.y = playerY;
          bombeOBJ.x = playerX-i;
        }
        else if(map[playerY][playerX-i] == WALL ||map[playerY][playerX-i] == BREAKABLE_WALL)
          blockedOuest = true;
      }
      else if((i<=enemyDistance) && (map[playerY][playerX-i]==GHOST_ENEMY)){
          bombeOBJ.y = playerY;
          bombeOBJ.x = playerX-i;
      }
    }

    //à Droite
    if(playerX+i < mapxsize){
      if(!blockedEst){
        if(isBomb(map[playerY][playerX+i], last_action)){
          bombeOBJ.y = playerY;
          bombeOBJ.x = playerX+i;
        }
        else if(i<=enemyDistance && isEnemy(map[playerY][playerX+i])){
          bombeOBJ.y = playerY;
          bombeOBJ.x = playerX+i;
        }
        else if(map[playerY][playerX+i] == WALL ||map[playerY][playerX+i] == BREAKABLE_WALL)
          blockedEst = true;
      }
      else if((i<=enemyDistance) && (map[playerY][playerX+i]==GHOST_ENEMY)){
          bombeOBJ.y = playerY;
          bombeOBJ.x = playerX+i;
      }
    }
    i++;
  }
  return bombeOBJ;
}

action whereToGoToBeSafe(graphe* g, maillon* listeNord, maillon* listeSud, maillon* listeOuest, maillon* listeEst, char** map, int mapxsize, int mapysize, int x, int y, int explosion_range, action last_action){

  //L'action qu'on voudra faire, bombing par défaut
  action choosen = BOMBING;
  sommet caseNord;
  sommet caseSud;
  sommet caseEst;
  sommet caseOuest;

  do{
    if(listeNord != NULL){
      caseNord = listeNord->val;
      if(couldIHidThere(caseNord.quoi.x, caseNord.quoi.y, map, mapxsize, mapysize, explosion_range, last_action)){
          choosen = NORTH;
      }
      listeNord = listeNord->next;
    }
    if(listeSud != NULL){
      caseSud = listeSud->val;
      if(couldIHidThere(caseSud.quoi.x, caseSud.quoi.y, map, mapxsize, mapysize, explosion_range, last_action)){
          choosen = SOUTH;
      }
      listeSud = listeSud->next;
    }
    if(listeEst != NULL){
      caseEst = listeEst->val;
      if(couldIHidThere(caseEst.quoi.x, caseEst.quoi.y, map, mapxsize, mapysize, explosion_range, last_action)){
          choosen = EAST;
      }
      listeEst = listeEst->next;
    }
    if(listeOuest != NULL){
      caseOuest = listeOuest->val;
      if(couldIHidThere(caseOuest.quoi.x, caseOuest.quoi.y, map, mapxsize, mapysize, explosion_range, last_action)){
          choosen = WEST;
      }
      listeOuest = listeOuest->next;
    }
  }
  while(choosen==BOMBING && (listeNord != NULL || listeSud != NULL || listeEst != NULL || listeOuest != NULL));
  
  return choosen;
}

maillon* getBonusList(graphe* g){
  //Si le sommet existe il est forcement dans un chemin donc dans le graphe
  maillon* liste = NULL;
  sommet s;

  //On cherche parmis tous les sommets du graphe ceux qui sont des bonus et on les ajoutes
  for(int i=0 ; i<g->n ; i++){
    s = g->sommetList[i];
    if(s.quoi.quoi == BOMB_BONUS || s.quoi.quoi == FLAME_BONUS){
      liste = addSommetToHead(liste, s);
    }
  }

  return liste;
}

action isBonusAccessibleInRadius(graphe* g, maillon* listeNord, maillon* listeSud, maillon* listeOuest, maillon* listeEst, char** map, int mapxsize, int mapysize, int explosion_range, action last_action, int radius, int x, int y){
  //C'est l'action qu'on renverras initialiser à BOMBING, ce qu'on renvoit si aucun bonus ne correspond
  action actionRes = BOMBING;
  maillon* bonusList = getBonusList(g);

  //permetteras de se balader dans la liste de bonus sans en perdre la tête
  maillon* currentInBonusList;
  sommet currentBonus;
  int i = 0;

  sommet caseNord;
  sommet caseSud;
  sommet caseEst;
  sommet caseOuest;

//Nous permet de savoir si la case directement à au N,S,O,E est safe ou non
  bool isNordSafe = isPosSafe(x, y-1, map, mapxsize, mapysize, explosion_range, last_action);
  bool isSouthSafe = isPosSafe(x, y+1, map, mapxsize, mapysize, explosion_range, last_action);
  bool isEastSafe = isPosSafe(x+1, y, map, mapxsize, mapysize, explosion_range, last_action);
  bool isWestSafe = isPosSafe(x-1, y, map, mapxsize, mapysize, explosion_range, last_action);

  //La recherche doit-elle s'aretter
  bool end = false;

  //On boucle ça tant que on as pas trouvé d'action, qu'on est pas plus loin que le radius et qu'il reste des listes à explorer
  while(actionRes==BOMBING && i<radius && !end){
    //Et on renvoit current à la tête de la liste de bonus
    currentInBonusList = bonusList;
    if(listeNord != NULL){
      caseNord = listeNord->val;
      listeNord = listeNord->next;
    }
    if(listeSud != NULL){
      caseSud = listeSud->val;
      listeSud = listeSud->next;
    }
    if(listeOuest != NULL){
      caseOuest = listeOuest->val;
      listeOuest = listeOuest->next;
    }
    if(listeEst != NULL){
      caseEst = listeEst->val;
      listeEst = listeEst->next;
    }
    //On regarde pour les différents bonus
    while(currentInBonusList!=NULL && actionRes==BOMBING){
      currentBonus = currentInBonusList->val;
      if (isSameSommet(caseNord, currentBonus) && isNordSafe){
        actionRes = NORTH;
      }
      else if(isSameSommet(caseSud, currentBonus) && isSouthSafe){
        actionRes = SOUTH;
      }
      else if(isSameSommet(caseOuest, currentBonus) && isWestSafe){
        actionRes = WEST;
      }
      else if(isSameSommet(caseEst, currentBonus) && isEastSafe){
        actionRes = EAST;
      }
      currentInBonusList = currentInBonusList->next;
    }
    i++;
    end = ((listeNord==NULL || !isNordSafe) && (listeSud==NULL || !isSouthSafe) && (listeEst==NULL || !isEastSafe) && (listeOuest==NULL || !isWestSafe));
  }

  return actionRes;
}

action goToExit(graphe* g, maillon* listeNord, maillon* listeSud, maillon* listeOuest, maillon* listeEst, char** map, int mapxsize, int mapysize, action last_action, int explosion_range, int x, int y){
  //Comme d'habitude bombing est le résultat par défaut si on ne trouve pas de sortie
  action actionRes = BOMBING;

  sommet caseNord;
  sommet caseSud;
  sommet caseEst;
  sommet caseOuest;

  //Nous permet de savoir si la case directement à au N,S,O,E est safe ou non
  bool isNordSafe = isPosSafe(x, y-1, map, mapxsize, mapysize, explosion_range, last_action);
  bool isSouthSafe = isPosSafe(x, y+1, map, mapxsize, mapysize, explosion_range, last_action);
  bool isEastSafe = isPosSafe(x+1, y, map, mapxsize, mapysize, explosion_range, last_action);
  bool isWestSafe = isPosSafe(x-1, y, map, mapxsize, mapysize, explosion_range, last_action);

  //La recherche doit-elle s'aretter
  bool end = false;

  //On boucle ça tant que on as pas trouvé d'action, qu'on est pas plus loin que le radius et qu'il reste des listes à explorer
  while(actionRes==BOMBING && !end){
    //Et on renvoit current à la tête de la liste de bonus
    if(isNordSafe && listeNord != NULL){
      caseNord = listeNord->val;
      if(isSortie(caseNord))
        actionRes = NORTH;
      listeNord = listeNord->next;
    }
    if(isSouthSafe && listeSud != NULL){
      caseSud = listeSud->val;
      if(isSortie(caseSud))
        actionRes = SOUTH;
      listeSud = listeSud->next;
    }
    if(isWestSafe && listeOuest != NULL){
      caseOuest = listeOuest->val;
      if(isSortie(caseOuest))
        actionRes = WEST;
      listeOuest = listeOuest->next;
    }
    if(isEastSafe && listeEst != NULL){
      caseEst = listeEst->val;
      if(isSortie(caseEst))
        actionRes = EAST;
      listeEst = listeEst->next;
    }
    end = ((listeNord==NULL || !isNordSafe) && (listeSud==NULL || !isSouthSafe) && (listeEst==NULL || !isEastSafe) && (listeOuest==NULL || !isWestSafe));
  }

  return actionRes;
}

bool isBombingUseful(char** map, int mapxsize, int mapysize, int x, int y, int explosion_range){
  int i=1;
  bool res = false;

  bool blockedNord = false;
  bool blockedSud = false;
  bool blockedEst = false;
  bool blockedOuest = false;

  while(!res && i<=explosion_range){
    if(y-i >= 0 && !blockedNord){
      if(map[y-i][x] == BREAKABLE_WALL){
        res = true;
      }
      else if(map[y-i][x] == WALL){
        blockedNord = true;
      }
    }
    if(y+i <mapysize && !blockedSud){
      if(map[y+i][x] == BREAKABLE_WALL){
        res = true;
      }
      else if(map[y-1][x] == WALL){
        blockedSud = true;
      }
    }
    if(x-i >= 0 && !blockedOuest){
      if(map[y][x-i] == BREAKABLE_WALL){
        res = true;
      }
      else if(map[y][x-i] == WALL){
        blockedOuest = true;
      }
    }
    if(x+i <mapxsize && !blockedEst){
      if(map[y][x+i] == BREAKABLE_WALL){
        res = true;
      }
      else if(map[y][x+i] == WALL){
        blockedEst = true;
      }
    }
    i++;
  }
  return res;
}

action replacementUtil(graphe* g, maillon* listeNord, maillon* listeSud, maillon* listeOuest, maillon* listeEst, char** map, int mapxsize, int mapysize, int x, int y, int explosion_range, action last_action){
  //Comme d'habitude bombing est le résultat par défaut si on ne trouve pas de sortie
  action actionRes = BOMBING;

  sommet caseNord;
  sommet caseSud;
  sommet caseEst;
  sommet caseOuest;

  //Es ce que la case N,S,O,E est elle safe (pourrait on aller dessus)
  bool isNordSafe = isPosSafe(x, y-1, map, mapxsize, mapysize, explosion_range, last_action);
  bool isSouthSafe = isPosSafe(x, y+1, map, mapxsize, mapysize, explosion_range, last_action);
  bool isEastSafe = isPosSafe(x+1, y, map, mapxsize, mapysize, explosion_range, last_action);
  bool isWestSafe = isPosSafe(x-1, y, map, mapxsize, mapysize, explosion_range, last_action);

  //Es ce que la recherche doit se terminer
  bool end = false;


  while (actionRes==BOMBING && !end)
  {
    if(isNordSafe && listeNord!=NULL){
      caseNord = listeNord->val;
      if(isBombingUseful(map, mapxsize, mapysize, caseNord.quoi.x, caseNord.quoi.y, explosion_range)){
        actionRes = NORTH;
      }
      listeNord = listeNord->next;
    }
    if(isSouthSafe && listeSud!=NULL){
      caseSud = listeSud->val;
      if(isBombingUseful(map, mapxsize, mapysize, caseSud.quoi.x, caseSud.quoi.y, explosion_range)){
        actionRes = SOUTH;
      }
      listeSud = listeSud->next;
    }
    if(isEastSafe && listeEst!=NULL){
      caseEst = listeEst->val;
      if(isBombingUseful(map, mapxsize, mapysize, caseEst.quoi.x, caseEst.quoi.y, explosion_range)){
        actionRes = EAST;
      }
      listeEst = listeEst->next;
    }
    if(isWestSafe && listeOuest!=NULL){
      caseOuest = listeOuest->val;
      if(isBombingUseful(map, mapxsize, mapysize, caseOuest.quoi.x, caseOuest.quoi.y, explosion_range)){
        actionRes = WEST;
      }
      listeOuest = listeOuest->next;
    }

    end = ((listeNord==NULL || !isNordSafe) && (listeSud==NULL || !isSouthSafe) && (listeEst==NULL || !isEastSafe) && (listeOuest==NULL || !isWestSafe));
  }

  return actionRes;
}

bool enemyColler(int x, int y, char** map){
  bool res = isEnemy(map[y-1][x]) | isEnemy(map[y+1][x]) | isEnemy(map[y][x-1]) | isEnemy(map[y][x+1]);

  return res;
}

action perduPourPerdu(char** map, int x, int y){
  action res = BOMBING;

  char caseNord = map[y-1][x];
  char caseSud = map[y+1][x];
  char caseOuest = map[y][x-1];
  char caseEst = map[y][x+1];
  if(caseNord == PATH)
    res = NORTH;
  else if(caseSud == PATH)
    res = SOUTH;
  else if(caseOuest == PATH)
    res = WEST;
  else if(caseEst == PATH)
    res = EAST;
  else{
    //Dans ce cas on est foutu
    res = NORTH;
  }

  return res;
}

graphe* createPathGraph(char** map, int mapxsize, int mapysize){
  graphe* g = initGraph(5);
  int id_current;
  int id_neighbour;
  for(int y=0 ; y<mapysize ; y++){
    for(int x=0 ; x<mapxsize ; x++){
      //Attention à ne pas oublier que BOMBERMAN se trouve sur un path
      if(isPath(map, x, y)){

        objet temp = {.quoi=map[y][x], .x=x, .y=y};
        addPathInGraph(g, temp);
        /*
        les deux conditions suivantes permettent de construire la matrice d'adjacence
        En effet pour la construire il suffit de regarder : pour tout chemin dans map
        si la case au dessus et celle à droite sont des chemins
        Si c'est le cas on peut ajouter les deux liaison dans la matrice
        on est garantit de pas dépasser de la map grace aux bordures * qui ne passe pas le if path
        */
        if(isPath(map, x, y-1)){
          id_current = g->sommetList[g->n-1].nb;
          id_neighbour = getSommetIdByPosition(g, x, y-1);
          g->mat[id_current][id_neighbour] = 1;
          g->mat[id_neighbour][id_current] = 1;
        }
        if(isPath(map, x-1, y)){
          id_current = g->sommetList[g->n-1].nb;
          id_neighbour = getSommetIdByPosition(g, x-1, y);
          g->mat[id_current][id_neighbour] = 1;
          g->mat[id_neighbour][id_current] = 1;
        }
      }
    }
  }
  return g;
}

void addPathInGraph(graphe* g, objet path){
  //On vérifie si il ne faut pas étendre la taille du graphe
  if(g->n >= g->nmax){
    g = extendGraph(g);
  }
  sommet newEdge = {.nb=g->n, .quoi=path};
  g->sommetList[g->n] = newEdge;
  g->n = g->n +1;

}

graphe* initGraph(int taille){
  graphe* new = (graphe*)malloc(sizeof(graphe));
  //La liste de sommet est initialisée à NULL
  new->nmax = taille;
  new->n = 0;
  new->sommetList = malloc(sizeof(sommet)*taille);
  new->mat = (int**)malloc(sizeof(int*)*taille);
  for(int i=0 ; i<taille ; i++){
    new->mat[i] = (int*)malloc(sizeof(int)*taille);
    for(int j=0 ; j<taille ; j++){
      new->mat[i][j] = 0;
    }
  }
  return new;
}

graphe* extendGraph(graphe* g){
  //On définit la nouvelle taille
  int nouvelleTaille = g->nmax*2;
  //On redimmensionne la matrice d'adjacence
  for(int i=0 ; i<g->nmax ; i++){
        g->mat[i] = (int*)realloc(g->mat[i], sizeof(int)*nouvelleTaille);
        for(int j=g->nmax ; j<nouvelleTaille ; j++){
          g->mat[i][j] = 0;
        }
    }
    g->mat = (int**)realloc(g->mat, sizeof(int*)*nouvelleTaille);
    for(int i=g->nmax ; i<nouvelleTaille ; i++){
        g->mat[i] = (int*)malloc(sizeof(int)*nouvelleTaille);
        for(int j=0 ; j<nouvelleTaille ; j++){
            g->mat[i][j] = 0;
        }
    }

  g->sommetList = realloc(g->sommetList, sizeof(sommet)*nouvelleTaille);

  g->nmax = nouvelleTaille;
  return g;
}

void libereGraphe(graphe* g){
  for(int i=0 ; i<g->n ; i++){
    free(g->mat[i]);
  }
  free(g->mat);
  free(g->sommetList);
}

maillon* parcoursLargeur(graphe* g, sommet s, bool ignorePlayer){
  //Ce sera le parcours final à renvoyer
  maillon* parcours = NULL;

  bool* tableauVu = (bool*)malloc(sizeof(bool)*g->n);
  for(int i=0 ; i<g->n ; i++){
    tableauVu[i] = false;
  }

  //On init la file avec sa première valeur
  maillon* file = (maillon*)malloc(sizeof(maillon));
  file->val = s;
  file->next = NULL;

  sommet current;

  while(file != NULL){
    current = tete(file);
    parcours = addSommetToHead(parcours, current);
    tableauVu[current.nb] = true;
    for(int i=0 ; i<g->n ; i++){
      //On regarde si le sommet d'indice i est voisins avec le current, si c'est le cas on l'ajoute dans la file
      if((g->mat[current.nb][i])){
        if(!estJoueur(g->sommetList[i]) && ignorePlayer){
          if(!(tableauVu[g->sommetList[i].nb])){
            enfiler(file, g->sommetList[i]);
          }

        }
      }
    }
    //Très important de défiler à la fin parce que les fct d'enfilage ne marche pas sur une file vide
    file = defiler(file);
  }
  /*
  En pratique inverser la liste après l'avoir construite n'est pas une mauvaise chose
  car les ajout on tête était en 0(1) alors que ceux en queue sont tous en 0(n)
  au total on as donc du o(n) au lieu de o(n**2)
  */
  parcours = reverseListe(parcours);

  free(tableauVu);

  return parcours;
}

void addMaillonToTail(maillon* chaine, maillon* toAdd){
  if(chaine->next == NULL)
    chaine->next = toAdd;

  else
    addMaillonToTail(chaine->next, toAdd);

}

maillon* addMaillonToHead(maillon* chaine, maillon* toAdd){
  toAdd->next = chaine;
  return toAdd;
}

maillon* addSommetToHead(maillon* chaine, sommet toAdd){
  maillon* new = (maillon*)malloc(sizeof(maillon));
  new->val = toAdd;
  new->next = chaine;
  return new;
}

void enfiler(maillon* file, sommet toAdd){
  maillon* new = (maillon*)malloc(sizeof(maillon));
  new->val = toAdd;
  new->next = NULL;
  addMaillonToTail(file, new);
}

sommet tete(maillon* file){
  return file->val;
}

maillon* defiler(maillon* file){
  maillon* newFile = file->next;
  free(file);
  return newFile;
}

maillon* reverseListe(maillon* liste){
  /*
  Principe :
    temp stocke le maillon qu'on veut ajouter
    liste devient sa propre queue
    il ne reste plus qu'a séparé le maillon contenue dans temp qui deviendras un maillon indépendant qu'on peut ajouer
  */
  maillon* new = false;
  maillon* temp;
  while(liste != NULL){
    temp = liste;
    liste = liste->next;
    temp->next = NULL;
    new = addMaillonToHead(new, temp);
  }
  return new;
}

void libereList(maillon* chaine){
  if(chaine != NULL){
    libereList(chaine->next);
    free(chaine);
  }
}

int getSommetIdByPosition(graphe* g, int x, int y){
  int i=-1;
  do{
    i++;
  }
  while(!(g->sommetList[i].quoi.x == x && g->sommetList[i].quoi.y == y)&& i<g->n);
  if(i >= g->n)
    i = -1;
  return i;
}

int* getPosjoueur(char** map, int mapxsize, int mapysize){
  int* tab = (int*)malloc(sizeof(int)*2);
  for(int i=0 ; i<mapxsize ; i++){
    for(int j=0 ; j<mapysize ; j++){
      if(map[j][i] == BOMBERMAN){
        tab[1] = j;
        tab[0] = i;
      }
    }
  }

  return tab;
}

maillon** getAll4Parcours(graphe* g, char** map, int x, int y){
  maillon** tab = (maillon**)malloc(sizeof(maillon*)*4);

  //Bien obliger de faire 4 parcours pour savoir ce qu'on peut acc"éder à partir d'où
  tab[0] = NULL;
  tab[1] = NULL;
  tab[2] = NULL;
  tab[3] = NULL;

  //Des listes chainée qui contiendront tout les voisins en partant du nord ou autre
  int nord_id = getSommetIdByPosition(g, x, y-1);
  int sud_id = getSommetIdByPosition(g, x, y+1);
  int ouest_id = getSommetIdByPosition(g, x-1, y);
  int est_id = getSommetIdByPosition(g, x+1, y);

  //On vérifie que ces cases soient bien dans le graphe (car la fct renvoit -1 sinon)
  if(nord_id != -1)
    tab[0] = parcoursLargeur(g, g->sommetList[nord_id], true);
  if(sud_id != -1)
    tab[1] = parcoursLargeur(g, g->sommetList[sud_id], true);
  if(ouest_id != -1)
    tab[2] = parcoursLargeur(g, g->sommetList[ouest_id], true);
  if(est_id != -1)
    tab[3] = parcoursLargeur(g, g->sommetList[est_id], true);

  return tab;
}

bool isPath(char** map, int x, int y){
  bool res =  ((map[y][x]!=WALL) && (map[y][x]!=BREAKABLE_WALL) && (map[y][x]!=BOMB));
  return res;
}

bool estJoueur(sommet isHeThere){
  return (isHeThere.quoi.quoi == BOMBERMAN);
}

bool isSameSommet(sommet a, sommet b){
  bool res = (a.quoi.x==b.quoi.x) && (a.quoi.y==b.quoi.y);
  return res;
}

bool isSortie(sommet s){
  return s.quoi.quoi == EXIT;
}

bool isBomb(char item, action last_action){
  bool res = (item==BOMB) || ((last_action==BOMBING) && (item==BOMBERMAN));
  return res; 
}

bool isEnemy(char item){
  bool res = (item==FLAME_ENEMY) || (item==GHOST_ENEMY);
  return res;
}

bool isDanger(char item, action last_action){
  bool res = isEnemy(item) || isBomb(item, last_action);
  return res;
}

void AfficheMat(graphe* g){
    int taille = g->n;
    for(int i=0 ; i<taille ; i++){
        for(int j=0 ; j<taille ; j++){
            printf("%d ",g->mat[i][j]);
        }
        printf("\n");
    }
}

void afficheSommetGraphe(graphe* g){
  for(int i=0 ; i<g->n ; i++){
    objet temp = g->sommetList[i].quoi;
    printf("1 : %c à la position (%d,%d)\n",temp.quoi, temp.x, temp.y);
  }
}

void afficheparcours(maillon* parcours){
  if(parcours != NULL){
    printf("p : (%d, %d)\n", parcours->val.quoi.x, parcours->val.quoi.y);
    afficheparcours(parcours->next);
  }
}