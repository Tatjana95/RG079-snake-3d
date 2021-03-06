#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#define TIMER_ID 1
#define TIMER_INTERVAL 300

#define EPS (0.000001)

//Struktira za zmiju
typedef struct{
	float snake_x;
	float snake_y;
	float snake_z ;
	struct SNAKE* next;
	struct SNAKE* preview;
}SNAKE;

//Struktura za prepreke
typedef struct{
	float barrier_x;
	float barrier_y;
}BARRIER;

static void on_display(void);
static void on_reshape(int width, int height);
static void on_timer(int);
static void on_keyboard(unsigned char key, int x, int y);

static void print_text(char* text, float x, float y, float z); //Funkcija za ispisivanje teksta na ekranu
static void print_instruction_text(char* text, float x, float y, float z); //Pomocna funkcija za ispis instrukcija
static void print_instruction(); //Funkcija koja ispisuje instrukcije

static void key_indicator(int w, int a, int s, int d); //Funkcija koja odredjuje koja je poslednja stisnuta komanda

static void draw_area(); //Funkcija koja iscrtava teren za igru
static void draw_frame(); //Funkcija koja iscrtava zid
static void draw_grass(); //Funkcija koja iscrtava zelenu podlogu po kojoj se zmija krece
static float barrier_coor(); //Funkcija koja odredjuje jednu koordinatu prepreke
static void draw_barrier(); //Funkcija koja iscrtava prepreke

static void draw_snake(SNAKE* head); //Funkcija koja iscrtava zmiju
static void initial_snake(); //Funkcija koja crta pocetnu zmiju

static void draw_food(float r, float g, float b); //Funkcija koja crta hranu

static void snake_move(); //Funkcija koja odredjuje u kom smeru ce se zmija kretati zavisno od poslednje komande
static int hit_wall(float x, float y); //Funkcija koja proverava da li je udareno u zod
static int hit_barrier(float x, float y); //Funkcija koja proverava da li je udareno u pepreku
static int snake_eat(float s_x, float s_y, float f_x, float x_y); //Funkcija koja proverava da li je zmija pojela hranu

static void end_game(); //Kraj igre

static float food_coor(); //Koordinate hrane
static float color_rand(); //Funkcija koja odredjuje vrednosti za boju

int animation_ongoing = 0;
int game_over= 0; //Indikator da li je igra zavrsena

time_t t;

float frame_r, frame_g, frame_b; //Boje okvira
float snake_r, snake_g, snake_b; //Boje zmije
float food_x, food_y; //Koordinate hrane
float food_r, food_g, food_b; //Boje hrane

float correct_x, correct_y; //Za koliko treba da transliramo zmiju

//Koja je poslednja zadata komanda, inicijalno je 'a', jer nam zmija na pocetku igre ide na levo
int if_w = 0;
int if_a = 1;
int if_s = 0;
int if_d = 0;

BARRIER barrier[3]; //Prepreke
SNAKE* snake; //Zmija

int snake_acceleration = 0; //Koliko treba ubrzati zmiju

int new_food = 1; //Da li je potrebno odrediti nove koordinate hrane

int score = 0; //Skor

int main(int argc, char** argv)
{
	GLfloat light_ambient[] = {0.1, 0.1, 0.1, 1};
	GLfloat light_diffuse[] = {0.7, 0.7, 0.7, 1};
	GLfloat light_specular[] = {0.9, 0.9, 0.9, 1};
	
	GLfloat ambient_coeffs[] = {0.3, 0.7, 0.3, 1};
	GLfloat diffuse_coeffs[] = {0.2, 1, 0.2, 1};
	GLfloat specular_coeffs[] = {1, 1, 1, 1};
	GLfloat shininess = 30;
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	// Kreira se prozor. 
	glutInitWindowSize(1100, 650);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Snake 3D");

	// Registruju se callback funkcije. 
	glutKeyboardFunc(on_keyboard);
	glutReshapeFunc(on_reshape);
	glutDisplayFunc(on_display);

	// Obavlja se OpenGL inicijalizacija. 
	glClearColor(0.8, 0.7, 1, 0);

	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_LIGHTING);
	
	glEnable(GL_LIGHT0);
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
	glEnable(GL_COLOR_MATERIAL);
	
	//inicijalizacija zmije
	initial_snake();
	
	srand((unsigned) time(&t));
	
	//Inicijalizuju se boje za okvir terena
	frame_r = color_rand();
	frame_g = color_rand();
	frame_b = color_rand();
	
	//Inicijalizuju se boje za zmiju
	snake_r = color_rand();
	snake_g = color_rand();
	snake_b = color_rand();

	//Inicijalizuju se koordinate za prepreke
	for(int i = 0; i < 2; i++)
	{
		float x, y;
		do{
			x = barrier_coor();
			y = barrier_coor();
			barrier[i].barrier_x = x;
			barrier[i].barrier_y = y;
		}while(y == 0 || (x < 0.15 && x > -0.25));
	}
	
	
	// Program ulazi u glavnu petlju.

	glutMainLoop();

	free(snake);

	return 0;

}

static void on_display()
{

	glClear(GL_COLOR_BUFFER_BIT);

	GLfloat light_position[] = {1, 3, 1, 0};
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(0, -1.5, 1.5, 0, 0, 0, 0, 0, 1);
        glShadeModel(GL_SMOOTH);
	
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	
	//Iscrtavanje terena za zmicu
	glPushMatrix();
		draw_area();
	glPopMatrix();
	
	glPushMatrix();
		draw_snake(snake);
	glPopMatrix();
	
	//Provera da li zmija udara u zid
	if(hit_wall(snake->snake_x, snake->snake_y))
		end_game();
	
	//Provera da li zmija udara u prepreku
	if(hit_barrier(snake->snake_x, snake->snake_y))
		end_game();
	
	//Hrana za zmiju
	if(new_food)
	{
		//x koordinata je slucajan broj izmedju -1 i 1 sa korakom 0.05
		do {
			food_x  = food_coor();
			//Hrana ne sme da se nadje na zidu
			if(food_x == -1)
				food_x = -0.95;
			if(food_x == 1)
				food_x = 0.95;
			//y koordinata je slucajan broj izmedju -1 i 1 sa korakom 0.05
			food_y  = food_coor();
			if(food_y == -1)
				food_y = -0.95;
			if(food_y == 1)
				food_y = 0.95;
		}while(hit_barrier(food_x, food_y)); //Hrana ne sme da se nadjen da prepreci
		//Odredjujemo boju nove hrane
		food_r = color_rand();
		food_g = color_rand();
		food_b = color_rand();
		
		new_food = 0;
	}

	//Iscrtavanje hrane
	glPushMatrix();
		glTranslatef(food_x, food_y, 0.025);
		draw_food(food_r, food_g, food_b);
	glPopMatrix();
	
	//Provera da li zmija jede hranu
	
	if(snake_eat(snake->snake_x, snake->snake_y, food_x, food_y))
	{
		score++;
		new_food = 1;
		printf("%d\n", score);
		
		//Zmija raste
		SNAKE* tmp = malloc(sizeof(SNAKE));
		if(tmp == NULL)
			exit(EXIT_FAILURE);
		
		tmp->snake_x = food_x;
		tmp->snake_y = food_y;
		
		tmp->next = snake;
		tmp->preview = NULL;
		
		snake->preview = tmp;
		snake = tmp;
		
		//Ubrzavamo zmiju kada pojede hranu
		if(snake_acceleration < 200)
			snake_acceleration+=2;
	}
	
	//Ispis skora
	char score_text[50] ;
	sprintf(score_text, "SCORE: %d", score);
	print_text(score_text, -4,3,0);
	
	//Ispis instrukcija
	print_instruction();
	
	glutSwapBuffers();
}

static void on_keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27:
			exit(0);
			break;
		case 'g':
		case 'G':
			//Pokrecemo zmiju
			if(!animation_ongoing)
			{
				animation_ongoing = 1;
				glutTimerFunc(TIMER_INTERVAL - snake_acceleration, on_timer, TIMER_ID);
			}
			if(game_over)
			{
				initial_snake();
				key_indicator(0, 1, 0, 0);
				game_over = 0;
				score = 0;
			}
			snake_move();
			break;
		case 'p':
		case 'P':
			//Pauza
			animation_ongoing = 0;
			break;
		case 'r':
		case 'R':
			//Restart
			glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
			animation_ongoing = 0;
			//Kada restartujemo igricu, vracamo zmiju u pocetni polozaj i resetujemo ubrzanje
			snake_acceleration = 0;			
			key_indicator(0, 1, 0, 0);
			initial_snake();
			score = 0;
			break;
		
		case 'w':
		case 'W':
			//Zmija skrece gore
			if(!if_s && animation_ongoing)
			{
				key_indicator(1, 0, 0, 0);
				snake_move();
			}
			break;
		case 'a':
		case 'A':
			//Zmija skrece levo
			if(!if_d && animation_ongoing)
			{
				key_indicator(0, 1, 0, 0);
				snake_move();
			}
			break;
		case 's':
		case 'S':
			//Zmija skrece dole
			if(!if_w && animation_ongoing)
			{
				key_indicator(0, 0, 1, 0);
				snake_move();
			}
			break;
		case 'd':
		case 'D':
			//Zmija skrece desno
			if(!if_a && animation_ongoing)
			{
				key_indicator(0, 0, 0, 1);
				snake_move();
			}
			break;
	}
}

static void on_timer(int id)
{
	if(id != TIMER_ID)
		return;
	
	if(animation_ongoing)
	{
		glutTimerFunc(TIMER_INTERVAL - snake_acceleration, on_timer, TIMER_ID);
	
	
		while(snake->next != NULL)
			snake = snake->next;
		
		while(snake->preview != NULL)
		{
			SNAKE* tmp = snake->preview;
			snake->snake_x = tmp->snake_x;
			snake->snake_y = tmp->snake_y;
			snake = snake->preview;
		}
		
		snake->snake_x += correct_x;
		snake->snake_y += correct_y;
	
	}
	
	glutPostRedisplay();
}

static void on_reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)width / height, 1, 100);
}

static void draw_area()
{
	draw_frame();	
	draw_grass();
	draw_barrier();
}

static void draw_frame()
{	
	//Boja okvira
	glColor3f(frame_r, frame_g, frame_b);
	
	//Gornja ivica
	for(int i=0; i < 22; i++)  
	{
		glPushMatrix();
			glTranslatef(-1.05 + i/10.0, 1.05, 0.05);
			glutSolidCube(0.1);
		glPopMatrix();
	}

	//Leva ivica
	for(int i=0; i < 22; i++)  
	{
		glPushMatrix();
			glTranslatef(-1.05, 1.05 - i/10.0, 0.05);
			glutSolidCube(0.1);
		glPopMatrix();
	}

	//Desna ivica
	for(int i=0; i < 22; i++)  
	{
		glPushMatrix();
			glTranslatef(1.05, 1.05 - i/10.0, 0.05);
			glutSolidCube(0.1);
		glPopMatrix();
	}

	//Donja ivica
	for(int i=0; i < 22; i++)  
	{
		glPushMatrix();
			glTranslatef(-1.05 + i/10.0, -1.05, 0.05);
			glutSolidCube(0.1);
		glPopMatrix();
	}

}

static void draw_grass()
{
	glColor3f(0, 0.5, 0);
	glBegin(GL_POLYGON);
		glVertex3f(-1, 1, 0);
		glVertex3f(1, 1, 0);
		glVertex3f(1, -1, 0);
		glVertex3f(-1, -1, 0);
	glEnd();	

}

static void draw_snake(SNAKE* head)
{
 	if(snake_r != 0 && snake_g != 0.5 && snake_b !=0) //Zmija ne sme biti iste boje kao podloga
 		glColor3f(snake_r, snake_g, snake_b);
 	else
 		glColor3f(snake_r + 0.5, snake_g, snake_b + 0.1);
	
	//Da li je zmija ujela samu sebe
	SNAKE* tmp = head->next;
	for(; tmp != NULL; tmp = tmp->next)
		if((fabs(tmp->snake_x - head->snake_x) < EPS) && (fabs(tmp->snake_y - head->snake_y) < EPS))
		{
			end_game();
			break;
		}
		
	//Iscrtavanje zmije
	while(head != NULL)
	{
		glPushMatrix();
		double clip_plane[] = {0, 0, 1, 0};
 		glClipPlane(GL_CLIP_PLANE0, clip_plane);
		glEnable(GL_CLIP_PLANE0);
			glTranslatef(head->snake_x, head->snake_y, head->snake_z);
			glutSolidSphere(0.05 , 32, 32);	
		glPopMatrix();
		
		head = head->next;
	}
	glDisable(GL_CLIP_PLANE0);
}

static void initial_snake()
{
	snake = malloc(sizeof(SNAKE));
	snake->snake_x = 0;
	snake->snake_y = 0;
	snake->snake_z = 0;
	snake->next = NULL;
	snake->preview = NULL;
	
	for(int i = 1; i < 3; i++)
	{
		SNAKE* tmp = malloc(sizeof(SNAKE));
		if(tmp == NULL)
			exit(EXIT_FAILURE);
		
		tmp->snake_x = -i*0.05;
		
		tmp->next = snake;
		tmp->preview = NULL;
		snake->preview = tmp;
		snake = tmp;
	}
}

static void draw_food(float r, float g, float b)
{
	if(food_r != 0 && food_g != 0.5 && food_b !=0) 	//Hrana ne sme biti iste boje kao podloga
 		glColor3f(food_r, food_g, food_b);
 	else
 		glColor3f(food_r + 0.5, food_g, food_b + 0.1);
	
	glPushMatrix();
		glutSolidSphere(0.05, 32, 32);
	glPopMatrix();
}

static void key_indicator(int w, int a, int s, int d)
{
	if_w = w;
	if_a = a;
	if_s = s;
	if_d = d;
}

static int hit_wall(float x, float y)
{
	if(x > 0.951 || x < -0.951)
		return 1;
	if(y > 0.951 || y < -0.951)
		return 1;
	return 0;
}

static void snake_move()
{
	if(if_w)
	{
		correct_x = 0;
 		correct_y = 0.05;
	}
	if(if_a)
	{
		correct_x = -0.05;
 		correct_y = 0;
	}
	if(if_s)
	{
		correct_x = 0;
		correct_y = -0.05;
	}
	if(if_d)
	{
		correct_x = 0.05;
 		correct_y = 0;
	}
}

static int snake_eat(float s_x, float s_y, float f_x, float f_y)
{
	if((fabs(s_x - f_x) < EPS) && (fabs(s_y - f_y) < EPS)) 
		return 1;
	
	return 0;
}

static void end_game()
{
	animation_ongoing = 0;
	game_over = 1;
	snake_acceleration = 0;
	printf("Kraj!!! :P\n");
	
	print_text("GAME OVER", -0.1, 0, 1);
	
	//Proveravamo da li je dostignut maksimalan broj poena
	int high_score = INT_MAX;
	FILE* high_score_txt;
	
	//Ucitavamo maksimum iz datoteke
	high_score_txt = fopen("high_score.txt", "r");
	if(high_score_txt != NULL)
	{
		fscanf(high_score_txt, "%d", &high_score);
		fclose(high_score_txt);
	}
	
	//Ako je trenutni skor veci od maksimuma upisujemo ga u datoteku i ispisujemo na ekran da je postignut novi maksimum
	if(score >= high_score)
	{
		high_score_txt = fopen("high_score.txt", "w");
		if(high_score_txt != NULL)
		{
			fprintf(high_score_txt, "%d", score);
			fclose(high_score_txt);
		}
 		char text[50];
 		sprintf(text, "NEW HIGH SCORE: %d", score);
 		print_text(text, -0.17, -0.3, 1);
	}
}

static void print_text(char* text, float x, float y, float z)
{
	char* tmp;
	glRasterPos3f(x, y, z);
	
	for(tmp = text ; *tmp != '\0'; tmp++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *tmp);
}

static void print_instruction_text(char* text, float x, float y, float z)
{
	char* tmp;
	glRasterPos3f(x, y, z);
	
	for(tmp = text ; *tmp != '\0'; tmp++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *tmp);
}

static void print_instruction()
{
 	print_instruction_text("INSTRUCTION:", 1.47, 1.5, 0.5);
  	print_instruction_text("COMMANDS:\tW A S D", 1.355, 1.2, 0.5);
  	print_instruction_text("STRAT: G", 1.28, 1, 0.5);
	print_instruction_text("PAUSE: P", 1.225, 0.84, 0.5);
	print_instruction_text("RESTART: R", 1.17, 0.69, 0.5);
}

static float food_coor()
{
	return ((rand() % 40) / 20.0) - 1;
}

static float color_rand()
{
	return (rand() % 10) / 10.0;
}

static float barrier_coor()
{
	float coor;
	
	do {
		coor= ((rand() % 40) / 20.0) - 1;
		
	}while(coor < -0.85 || coor > -0.1 || coor == 0);
	
	return coor;
}

static void draw_barrier()
{
	int i, j;
	
	glColor3f(frame_r, frame_g, frame_b);
	for(i = 0; i < 2; i++)
		for(j = 0; j < 20; j++)
		{
			if(i%2 == 0)
			{
				glPushMatrix();
					glTranslatef(j*0.05 + barrier[i].barrier_x, barrier[i].barrier_y, 0.025); 
					glutSolidCube(0.05);
				glPopMatrix();
			}
			else
			{
				glPushMatrix();
					glTranslatef( barrier[i].barrier_x, j*0.05 + barrier[i].barrier_y, 0.025); 
					glutSolidCube(0.05);
				glPopMatrix();
			}
		}
}

static int hit_barrier(float x, float y)
{
	if((x > barrier[0].barrier_x - 0.075) && (x < (barrier[0].barrier_x + 20*0.05)) && (fabs(barrier[0].barrier_y - y) < EPS))
		return 1;
	if((y > barrier[1].barrier_y - 0.025) && (y < (barrier[1].barrier_y + 20*0.05)) && (fabs(barrier[1].barrier_x - x) < EPS))
		return 1;
	
	return 0;
}
