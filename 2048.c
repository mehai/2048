/*ZANFIR MIHAI-BOGDAN
 *GRUPA 312CC
 *TEMA PROIECT: 2048
 *TIMP EFECTIV DE LUCRU: 32 h
 *NUMAR CERINTE REZOLVATE: 5
 */

#include<ncurses.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<sys/select.h>

#define FOREVER  1
#define NR_OPT 	3
#define MENU_H 	6
#define MENU_W 	12
#define S_TO_WAIT  	2
#define MILIS_TO_WAIT 	0
#define KEYBOARD 	0
#define SELECT_ERROR 	-1
#define SELECT_COMMAND 	1
#define SELECT_NO_COMMAND 	0

void menu();
void print_menu();
void init_colors();
int **init_matrix();
void new_matrix();
void game();
WINDOW ***def_board();
WINDOW *def_panel();
int generate_coord();
int generate_number();
void generate_new();
void update_board();
void update_panel();
int slide();
int check_moves();

int main()
{
	/*initializarea ecranului de lucru*/
	initscr();
	/*se inhibă afișarea caracterelor introduse de la tastatură */
	noecho();
	/*caracterele introduse sunt citite imediat - fără 'buffering' */
	cbreak();
	/*se sterge ecranul*/
	clear();
	/*initializare culori*/
	init_colors();
	/*intrarea in meniu*/
	menu();
	return 0;
}

/*initializarea culorilor care vor fi folosite*/
void init_colors()
{
	if(has_colors() == FALSE)
	{
		endwin();
		printf("Terminalul tau nu suporta culori\n");
		exit(1);
	}
	/*inceperea modulului de culori*/
	start_color();
	/*se creeaza o pereche pentru fiecare numar obtinut in joc
	 *si pentru meniul principal
	 */
	init_pair(1, COLOR_RED, COLOR_WHITE);
	init_pair(2, COLOR_WHITE, COLOR_YELLOW);
	init_pair(4, COLOR_WHITE, COLOR_GREEN);
	init_pair(8, COLOR_WHITE, COLOR_RED);
	init_pair(16, COLOR_WHITE, COLOR_BLUE);
	init_pair(32, COLOR_WHITE, COLOR_MAGENTA);
	init_pair(64, COLOR_WHITE, COLOR_CYAN);
	init_pair(128, COLOR_WHITE, COLOR_RED);
	init_pair(256, COLOR_BLACK, COLOR_YELLOW);
	init_pair(512, COLOR_BLACK, COLOR_BLUE);
	init_pair(1024, COLOR_BLACK, COLOR_MAGENTA);
	init_pair(2048, COLOR_BLACK, COLOR_CYAN);

}

/*meniul principal al jocului*/
void menu()
{
	char *options[] = { 
						"New Game",
						"Resume",
						"Quit",
					  };
	/*matricea de 4x4 folosita pentru a reproduce
	 *rezultatele miscarilor posibile in joc. Aceasta
	 *este initializata in meniu pentru a putea fi
	 *accesata in momentul in care user-ul alege
	 *optiunea "Resume".
	 */
	int **matrix;
	WINDOW *menu_win;
	int highlight, selected;
	int quit, started;
	long score;
	int c, y_menu, x_menu;
	/*pozitionarea meniului*/
	y_menu = (24 - MENU_H) / 2;
	x_menu = (80 - MENU_W) / 2;
	/*alocarea memoriei pentru matrice*/
	matrix = init_matrix();

	reprint:;
	selected = 0;
	highlight = 1;
	quit = 0;
	/*stergerea ecranului*/
	erase();
	refresh();
	attron(A_BOLD);
	mvprintw(y_menu - 1, x_menu + 4, "2048");
	attroff(A_BOLD);
	/*se creeaza fereastra meniului*/
	menu_win = newwin(MENU_H, MENU_W, y_menu, x_menu);
	/*se foloseste keypad pentru utilizarea sagetilor*/
	keypad(menu_win, TRUE);
	/*instructiunile de folosire a meniului*/
	mvprintw(0, 0, "Foloseste ARROW UP si ARROW DOWN pentru a te misca");
	mvprintw(1, 0, "Foloseste ENTER pentu a selecta o optiune");
	refresh();
	/*se printeaza meniul initial*/
	print_menu(menu_win, highlight, options);
	/*se intra intr-un loop infinit in care se asteapta
	 *input din partea utilizatorului: acesta se poate 
	 *misca in sus si in jos in meniu si poate accesa
	 *una din optiunile acestuia cu tasta ENTER
	 *Observatie: highlight are rol de index;
	 */
	while(FOREVER)
	{
		c = wgetch(menu_win);
		switch(c)
		{
			case KEY_UP:
				if(highlight == 1)
					continue;
				else
					highlight--;
				break;
			case KEY_DOWN:
				if(highlight == NR_OPT)
					continue;
				else
					highlight ++;
				break;
			case '\n':
				selected = highlight;
				break; 
		}
		/*se reprinteaza meniul dupa fiecare alegere a utilizatorului*/
		print_menu(menu_win, highlight, options);
		if(selected != 0)
			{
				switch(selected)
				{
					/*daca s-a ales optiunea "New Game"*/
					case 1:
						/*se sterge ecranul*/
						erase();
						refresh();
						/*se initializeaza scorul si variabila
						 *started cu 0 pentru a se trimite ca
						 *parametrii functiei game astfel incat
						 *jocul sa se initializeze corect
						 */
						score = 0;
						started = 0;
						/*matricea jocului este populata cu 0*/
						new_matrix(matrix);
						/*se intra in jocul efectiv*/
						game(&score, &started, matrix);
						/*se reafiseaza meniul*/
						goto reprint;
						break;
					/*daca s-a ales optiunea "Resume"*/
					case 2:
						/*daca jocul este deja inceput*/
						if(started == 1)
						{

							erase();
							refresh();
							/*se intra in joc iar acesta porneste
							 *de unde a fost abandonat de jucator
							 *deoarece started este 1
							 */
							game(&score, &started, matrix);
							/*se reafiseaza meniul*/
							goto reprint;
						}
						else
						{
							mvprintw(23, 0, "You first need to start a new game");
							clrtoeol();
							mvprintw(24, 0, "Press any key to continue...");
							clrtoeol();
							refresh();
							/*se asteapta input de la user pentru a reafisa meniul*/
							c = getch();
							goto reprint;
						}
						break;
					/*daca s-a ales optiunea Quit*/
					case 3:
						/*BONUS: se afiseaza un mesaj pentru a fi
						 *sigur de decizia userului de a parasi jocul
						 */
						mvprintw(23, 0, "Do you really want to quit?(y/n)");
						while(FOREVER)
						{
							quit = getch();
							if(quit == 'y')
								break;
							else if(quit == 'n')
								goto reprint;
						}
						break;
				}
			}
		/*daca jucatorul vrea sa paraseasca jocul, 
		 *se iese si din acest loop infinit
		 */
		if(quit == 'y')
			break;
	}
	/*se incheie modul curses*/
	endwin();
}

/*printarea la ecran a meniului principal*/
void print_menu(WINDOW *menu_win, int highlight, char *options[])
{
	int i, x, y;
	y = 1;
	x = 1;
	/*se creeaza marginile meniului*/
	box(menu_win, 0, 0);
	for(i = 0; i < NR_OPT; i++)
		{
			/*daca optiunea la care se afla cursorul este cea curenta...*/
			if(highlight == i + 1)
			{
				/*se foloseste atributul de tip culoare
				 *pentru a afisa optiunea curenta
				 */
				wattron(menu_win, COLOR_PAIR(1));
				mvwprintw(menu_win, y, x, "%s", options[i]);
				wattroff(menu_win, COLOR_PAIR(1));
			}
			else
				mvwprintw(menu_win, y, x, "%s", options[i]);
			y++;
		}
	/*fereastra meniului este reprintata pentru a afisa modificarile*/
	wrefresh(menu_win);
}

/*jocul efectiv*/
void game(long *score, int *started, int **matrix)
{
	WINDOW ***board, *panel;
	int key, valid = 1;
	int quit = 0;
	int nfds = 1, sel, move;
	/*se va citi in variabila read_key*/
	fd_set read_key;
	struct timeval timeout;
	/*se curata multimea de lucru pentru select*/
	FD_ZERO(&read_key);
	/*se adauga tastatura la multime*/
	FD_SET(KEYBOARD, &read_key);
	/*se initializaeaza timpul de timeout pentru jucator
	 *adica cat are de asteptat pana jocul va muta in 
	 *locul sau conform TASK 4
	 */
	timeout.tv_sec = S_TO_WAIT;
	timeout.tv_usec = MILIS_TO_WAIT;
	/*daca jocul este inceput nou, se creeaza cele doua celule*/
	if(*started == 0)
	{
		generate_new(matrix);
		generate_new(matrix);
	}
	/*se definesc fereastra panel si matricea 4x4 de ferestre
	 *care vor alcatui tabela jocului
	 */
	panel = def_panel();
	board = def_board();
	/*cele mentionate sunt printate pe ecran*/
	update_board(matrix, board);
	update_panel(panel, *score);
	while(FOREVER)
	{
		sel = select(nfds, &read_key, NULL, NULL, &timeout);
		switch (sel)
		{
			/*daca s-a primit input de la user:*/
			case SELECT_COMMAND:
				/*input retinut in variabila key*/
				key = getch();
				/*in functie de tasta apasata, se fac miscarile necesare in
				 *matrice prin functia slide care returneaza:
				 *2 daca jocul a fost castigat
				 *1 daca miscarea e valida
				 *0 daca miscarea e invalida
				 */
				switch(key)
				{
					case 'q':
						quit = 1;
						break;
					case 'w':
						valid = slide(matrix, score, 'u');
						break;
					case 's':
						valid = slide(matrix, score, 'd');
						break;
					case 'a':
						valid = slide(matrix, score, 'l');
						break;
					case'd':
						valid = slide(matrix, score, 'r');
						break;
					default:
						valid = 0;
						break;

				}
				break;
			/*daca nu s-a primit input de la user*/
			case SELECT_NO_COMMAND:
				valid = 0;
				/*se afla miscarea care elibereaza cele mai multe celule
				 *prin functia check_moves care poate returna:
				 *u, d, l sau r in functie de miscarea favorabila
				 *0 daca nicio miscare nu elibereaza nicio celula
				 *-1 daca jocul este pierdut
				 */ 
				move = check_moves(matrix);
				/*daca nicio miscare nu e favorabila, se executa
				 *prima miscare valida in oridea up, down, left,
				 *right. Altfel, se executa miscarea favorabila
				 */
				if(move == 0)
				{
					if(!valid)
						valid = slide(matrix, score, 'u');
					if(!valid)
						valid = slide(matrix, score, 'd');
					if(!valid)
						valid = slide(matrix, score, 'l');
					if(!valid)
						valid = slide(matrix, score, 'r');
				}
				else if(move == 'u')
					valid = slide(matrix, score, 'u');
				else if(move == 'd')
					valid = slide(matrix, score, 'd');
				else if(move == 'l')
					valid = slide(matrix, score, 'l');
				else if(move == 'r')
					valid = slide(matrix, score, 'r');
				break;
			/*daca s-a produs o eroare*/
			case SELECT_ERROR:
				mvprintw(23, 0,"Select error; exiting...\n");
				break;
		}

		if(quit == 1)
		{
			/*la iesireaa din joc prin optiunea
			 *quit, se presupune ca jocul este inceput
			 *si oprit temporar astfel started devine 1
			 */
			*started = 1;
			/*se iese din loop-ul infinit*/
			break;
		}
		/*Daca jocul a fost castigat:*/
		if(valid == 2)
		{
			update_board(matrix, board);
			update_panel(panel, *score);
			mvprintw(21, 34, "YOU WIN!");
		}
		move = check_moves(matrix);
		/*daca jocul a fost pierdut*/
		if(move == -1)
		{
			mvprintw(23, 0, "You are out of moves!");
			mvprintw(24, 0, "Press any key to continue...");
			key = getch();
			*started = 0;
			/*se iese din joc iar variabila started ramane 0
			 *deoarece nu se mai poate reveni la acest joc,
			 *acesta fiind pierdut
			 */
			break;
		}
		/*daca miscarea selectata este invalida*/
		if(valid == 0)
			mvprintw(23, 0, "Invalid move");
		/*daca miscarea a fost valida*/
		else
		{
			/*se genereaza o noua celula*/
			generate_new(matrix);
			/*se sterg mesagele afisate precedent*/
			move(23, 0);
			clrtoeol();
			/*se updateaza ecranul cu modificarile aferente*/
			update_board(matrix, board);
			update_panel(panel, *score);
		}
		/*se reinitializeaza variabilele modificare de select*/
		FD_SET(KEYBOARD, &read_key);
		timeout.tv_sec = S_TO_WAIT;
		timeout.tv_usec = MILIS_TO_WAIT;
	}
}

/*functie de alocare dinamica a memoriei pentru matricea jocului*/
int **init_matrix()
{
	int i, **matrix;
	matrix = (int **)calloc(4, sizeof(int *));
	for(i = 0; i < 4; i++)
		matrix[i] = (int *)calloc(4, sizeof(int));
	return matrix;
}

/*functie pentru popularea matricei cu 0*/
void new_matrix(int **matrix)
{
	int i, j;
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			matrix[i][j] = 0;
}

/*functie pentru definirea panoului*/
WINDOW *def_panel()
{
	WINDOW *panel;
	/*se creeaza fereastra*/
	panel = newwin(11, 30, 4, 60);
	/*se creeaza marginile*/
	box(panel, 0, 0);
	/*se scrie textul schelet al panoului*/
	mvwprintw(panel, 1, 1, "DATE:");
	mvwprintw(panel, 3, 1, "TIME:");
	mvwprintw(panel, 5, 1, "SCORE:");
	mvwprintw(panel, 7, 1, "INSTRUCTIONS:");
	/*legenda miscarilor valide*/
	mvwprintw(panel, 8, 1, "-> Use w,a,s,d to move");
	mvwprintw(panel, 9, 1, "-> q - Quit");
	/*se afiseaza modificarile efectuate la ecran*/
	wrefresh(panel);
	/*returneaza adresa ferestrei*/
	return panel;
}

/*functie pentru definirea matricei de ferestre corespondente tabelei de joc*/
WINDOW ***def_board()
{
	int i, j, starty = 2, startx = 30;
	WINDOW ***board;
	/*se aloca memorie dinamic pentru matricea de ferestre*/
	board = (WINDOW ***)malloc(4 * sizeof(WINDOW **));
	for(i = 0; i < 4; i++)
		board[i] = (WINDOW **)malloc(4 * sizeof(WINDOW *));

	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			{
				/*se creeaza fiecare fereastra*/
				board[i][j] = newwin(5, 6, starty + i * 5, startx + j * 6);
				/*se creeaza marginile fiecarei ferestre*/
				box(board[i][j], 0, 0);
				/*se "salveaza" modificarile facute inainte de a fi aduse la ecran*/
				wnoutrefresh(board[i][j]);
			}
	/*sunt afisate le ecran toate modificarile facute ecranului*/
	doupdate();
	/*Observatie: am folosit functiile wnoutrefresh si doupdate
	  *in loc de wrefresh pentru a reimprospata ecranul o singura data
	  *pentru toate cele 16 modificati aduse acestuia
	  */
	/*returneaza adresa matricei de ferestre*/
	return board;
}

/*functia de generare a unei celule cu coordonate si valoare intamplatoare*/
void generate_new(int **matrix)
{
	int x, y;
	/*se genereaza coordonate pana cand se gaseste o celula libera*/
	do
	{
		x = generate_coord();
		y = generate_coord();
	}
	while(matrix[x][y] != 0);
	/*se genereaza un numar aleator 2 sau 4*/
	matrix[x][y] = generate_number();
}

/*functia pentru generarea unui numar aleator 0 sau 1*/
int generate_number()
{
	int random_number;
	random_number = rand() % 2;
	if(random_number == 0)
		return 2;
	else
		return 4;
}

/*functia pentru generarea unui numar aleator 0, 1, 2 sau 3*/
int generate_coord()
{
	int random_coordinate;
	random_coordinate = rand() % 4;
	return random_coordinate;
}

/*functia pentru afisare la ecran a tabelei de joc*/
void update_board(int **matrix, WINDOW ***board)
{
	int i, j, x = 2;
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			/*daca numarul din celula este diferit de 0
			 *background-ul este corespunzator acelei valori
			 */
			if(matrix[i][j])
			{
				if(matrix[i][j] > 2048)
					wbkgd(board[i][j], COLOR_PAIR(1));
				else
					wbkgd(board[i][j], COLOR_PAIR(matrix[i][j]));
				/*in functie de numarul de cifre, numarul este
				 *afisat la o anumita pozitie in fereastra
				 *pentru ca tabela sa aiba un aspect placut
				 */
				if(matrix[i][j] > 1000)
					x = 1;
				mvwprintw(board[i][j], 2, 1, "    ");
				mvwprintw(board[i][j], 2, x, "%d", matrix[i][j]);
				wnoutrefresh(board[i][j]);	
			}
			/*daca numarul din celula este 0:*/
			else
			{
				wbkgd(board[i][j], COLOR_PAIR(1));
				mvwprintw(board[i][j], 2, 1, "    ");
				wnoutrefresh(board[i][j]);
			}
	/*se afiseaza toate modificarile facute la ecran*/
	doupdate();
}

/*functia pentru updatarea valorilor din panou: data, ora si scorul*/
void update_panel(WINDOW *panel, long score)
{	
	/*se updateaza data si ora*/
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	mvwprintw(panel, 2, 1, "%d-%d-%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	mvwprintw(panel, 4, 1, "%d:%d", tm.tm_hour, tm.tm_min);
	/*se updateaza scorul*/
	mvwprintw(panel, 6, 1, "%li", score);
	/*se afiseaza la ecran modificarile efectuate*/
	wrefresh(panel);
}

/*functia folosita pentru a determina miscarea care elibereaza cele
 *mai multe celule si pentru a determina dac mai sunt miscari valide
 */
int check_moves(int **matrix)
{
	int i, j, move, freed, max = 0, value, full = 1;
	/*SE VERIFICA MISCAREA UP*/
	freed = 0;
	for(j = 0; j < 4; j++)
	{	
		/*valoarea cu care se face compararea pe fiecare
		 *coloana este initial 0
		 */
		value = 0;
		for(i = 0; i < 4; i++)
			if(matrix[i][j] != 0)
			{
				/*daca este egala cu valoarea precedenta*/
				if(matrix[i][j] == value)
				{
					freed++;
					/*value devine din nou 0*/
					value = 0;
				}
				/*value devine valoarea din pastul curent*/
				else
					value = matrix[i][j];
			}
	}
	if(freed > max)
	{
		max = freed;
		move = 'u';
	}
	/*Observatie: Algoritmul este acelasi pentru fiecare miscare,
	 *diferite fiind doar parcurgerile matricei
	 */
	/*SE VERIFICA MISCAREA DOWN*/
	freed = 0;
	for(j = 0; j < 4; j++)
	{
		/*valoarea cu care se face compararea pe fiecare
		 *coloana este initial 0
		 */
		value = 0;
		for(i = 3; i >= 0; i--)
			if(matrix[i][j] != 0)
			{
				if(matrix[i][j] == value)
				{
					freed++;
					value = 0;
				}
				else
					value = matrix[i][j];
			}
	}
	if(freed > max)
	{
		max = freed;
		move = 'd';
	}
	/*SE VERIFICA MISCAREA LEFT*/
	freed = 0;
	for(i = 0; i < 4; i++)
	{

		value = 0;
		for(j = 0; j < 4; j++)
			if(matrix[i][j] != 0)
			{
				if(matrix[i][j] == value)
				{
					freed++;
					value = 0;
				}
				else
					value = matrix[i][j];
			}
	}
	if(freed > max)
	{
		max = freed;
		move = 'l';
	}
	/*SE VERIFICA MISCAREA RIGHT*/
	freed = 0;
	for(i = 0; i < 4; i++)
	{
		value = 0;	
		for(j = 3; j >= 0; j--)
			if(matrix[i][j] != 0)
			{
				if(matrix[i][j] == value)
				{
					freed++;
					value = 0;
				}
				else
					value = matrix[i][j];
			}
	}
	if(freed > max)
	{
		max = freed;
		move = 'r';
	}
	/*se verifica daca tabela este plina in toate celulele*/
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			if(matrix[i][j] == 0)
				full = 0;
	/*nu mai exista miscari valide - lose*/	
	if(full && max == 0)
		return -1;
	/*daca niciuna din miscari nu elibereaza o celula*/
	else if(max == 0)
		return 0;
	else
		/*returneaza miscarea care elibereaza cele mai multe celule*/
		return move;
}

/*functia care realizeaza modificarile in matrice, in functie de miscare*/
int slide(int **matrix, long *score, int c)
{
	int i, j, k, free, valid = 0;
	switch(c)
	{
		/*PENTRU MISCAREA UP*/
		case 'u':
			for(j = 0; j < 4; j++)
			{
				/*prima pozitie parcursa de la inceputul fiecarei
				 *coloane este considerata initial libera
				 */
				free = 0;
				/*in acest prim for aduc toate valorile cat
				 *mai aproape de sensul indicat (up in acest caz)
				 */
				for(i = 0; i < 4; i++)
				{
					/*cand dau de o valoare diferita de 0*/
					if(matrix[i][j] != 0)
					{
						matrix[free][j] = matrix[i][j];
						/*daca pozitia libera nu este aceeasi cu pozitia curenta*/
						if(free != i)
						{
							/*se elibereaza pozitia curenta si
							 *se considera o miscare valida*/
							matrix[i][j] = 0;
							valid = 1;
						}
						/*urmatoarea pozitie se considera libera*/
						free++;
					}
				}
				/*in acest al doilea for fac alipirea valorilor adiacente
				 *si readaptez pozitiile valorilor nealipite din matrice*/
				for(i = 0; matrix[i][j] != 0 && i < 3; i++)
				{
					if(matrix[i][j] == matrix[i + 1][j])
					{
						valid = 1;
						/*se efectueaza alipirea prin operatia de inmultire cu 2*/
						matrix[i][j] *= 2;
						/*se aduna la scor valoarea obtinuta in urma alipirii*/
						*score += matrix[i][j];
						/*se elibereaza pozitia urmatoare*/
						matrix[i + 1][j] = 0;
						/*pozitia urmatoare este considerata libera*/
						free = i + 1;
						/*se efectueaza repozitionarea valorilor printr-un
						 *algoritm asemanator cu cel din primul for
						 */
						for(k = free + 1; k < 4; k++)
						{
							if(matrix[k][j] != 0)
							{
								matrix[free][j] = matrix[k][j];
								if(free != k)
								{
									matrix[k][j] = 0;
								}
								free++;
							}

						}
					}

				}
			}
			break;
		/*Observatie: Algoritmul este acelasi pentru toate miscarile.
		 *diferite fiind doar modurile de parcurgere a matricei
		 */
	    /*PENTRU MISCAREA DOWN*/
		case 'd':
			for(j = 0; j < 4; j++)
			{
				free = 3;
				for(i = 3; i >= 0; i--)
				{
					if(matrix[i][j] != 0)
					{
						matrix[free][j] = matrix[i][j];
						if(free != i)
						{
							matrix[i][j] = 0;
							valid = 1;
						}
						free--;
					}
				}
				for(i = 3; matrix[i][j] != 0 && i > 0; i--)
				{
					if(matrix[i][j] == matrix[i - 1][j])
					{
						valid = 1;
						matrix[i][j] *= 2;
						*score += matrix[i][j];
						matrix[i - 1][j] = 0;
						free = i - 1;
						for(k = free - 1; k >= 0; k--)
						{
							if(matrix[k][j] != 0)
							{
								matrix[free][j] = matrix[k][j];
								if(free != k)
								{
									matrix[k][j] = 0;
								}
								free--;
							}

						}
					}

				}
			}
			break;
		/*PENTRU MISCAREA LEFT*/
		case 'l':
			for(i = 0; i < 4; i++)
			{
				free = 0;
				for(j = 0; j < 4; j++)
				{
					if(matrix[i][j] != 0)
					{
						matrix[i][free] = matrix[i][j];
						if(free != j)
						{
							matrix[i][j] = 0;
							valid = 1;
						}
						free++;
					}
				}
				for(j = 0; matrix[i][j] != 0 && j < 3; j++)
				{
					if(matrix[i][j] == matrix[i][j + 1])
					{
						valid = 1;
						matrix[i][j] *= 2;
						*score += matrix[i][j];
						matrix[i][j + 1] = 0;
						free = j + 1;
						for(k = free + 1; k < 4; k++)
						{
							if(matrix[i][k] != 0)
							{
								matrix[i][free] = matrix[i][k];
								if(free != k)
								{
									matrix[i][k] = 0;
								}
								free++;
							}

						}
					}

				}
			}
			break;
		/*PENTRU MISCAREA RIGHT*/
		case 'r':
			for(i = 0; i < 4; i++)
			{
				free = 3;
				for(j = 3; j >= 0; j--)
				{
					if(matrix[i][j] != 0)
					{
						matrix[i][free] = matrix[i][j];
						if(free != j)
						{
							matrix[i][j] = 0;
							valid = 1;
						}
						free--;
					}
				}
				for(j = 3; matrix[i][j] != 0 && j > 0; j--)
				{
					if(matrix[i][j] == matrix[i][j - 1])
					{
						valid = 1;
						matrix[i][j] *= 2;
						*score += matrix[i][j];
						matrix[i][j - 1] = 0;
						free = j - 1;
						for(k = free - 1; k >= 0; k--)
						{
							if(matrix[i][k] != 0)
							{
								matrix[i][free] = matrix[i][k];
								if(free != k)
								{
									matrix[i][k] = 0;
								}
								free--;
							}

						}
					}

				}
			}
			break; 
	}
	/*returneaza valoarea de validitate a miscarii(0/1/2)*/
	return valid;
}
