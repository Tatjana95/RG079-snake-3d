#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#define TIMER_ID 1
#define TIMER_INTERVAL 10

static void on_display(void);
static void on_reshape(int width, int height);
static void on_timer(int);
static void on_keyboard(unsigned char key, int x, int y);

static void draw_area();
static void draw_frame();
static void draw_grass();
static void draw_snake();

int animation_ongoing = 0;

time_t t;
int num_tail = 2;

float frame_r, frame_g, frame_b;
float snake_r, snake_g, snake_b;

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

	/* Kreira se prozor. */
	glutInitWindowSize(1100, 650);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	/* Registruju se callback funkcije. */
	glutKeyboardFunc(on_keyboard);
	glutReshapeFunc(on_reshape);
	glutDisplayFunc(on_display);

	/* Obavlja se OpenGL inicijalizacija. */
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
	
	/* Program ulazi u glavnu petlju. */
	
	srand((unsigned) time(&t));
	
	//Inicijalizuju se bole za okvir terena
	frame_r = (rand() % 10) / 10.0;
	frame_g = (rand() % 10) / 10.0;
	frame_b = (rand() % 10) / 10.0;
	
	snake_r = (rand() % 10) / 10.0;
	snake_g = (rand() % 10) / 10.0;
	snake_b = (rand() % 10) / 10.0;
	
	glutMainLoop();

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
		glTranslatef(0, 0, 0);
		draw_snake();
	glPopMatrix();
	
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
			if(!animation_ongoing)
			{
				animation_ongoing = 1;
				glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
			}
			break;
		case 'p':
		case 'P':
			animation_ongoing = 0;
			break;
		case 'r':
		case 'R':
			glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
			animation_ongoing = 0;
			break;
		
		case 'w':
		case 'W':
			break;
		case 'a':
		case 'A':
			break;
		case 's':
		case 'S':
			break;
		case 'd':
		case 'D':
			break;
	}
}

static void on_timer(int id)
{
	if(id != TIMER_ID)
		return;
	
	if(animation_ongoing)
	{
		glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
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
}

static void draw_frame()
{
	//printf("%f\n%f\n%f\n", frame_r, frame_g, frame_b);
	
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
	
	/*glBegin(GL_LINES);
		glColor3f(0, 0, 0);
		for(int i=0;i<21;i++)
		{	
			
			glVertex3f(-1+i/10.0,1,0);
			glVertex3f(-1+i/10.0,-1,0);
			glVertex3f(-1,-1+i/10.0,0);
			glVertex3f(1,-1+i/10.0,0);
		}
	glEnd();*/
}

static void draw_snake()
{
	glColor3f(snake_r, snake_g, snake_b);
	
	glPushMatrix();
		double clip_plane[] = {0, 0, 1, 0};
		glClipPlane(GL_CLIP_PLANE0, clip_plane);
		glEnable(GL_CLIP_PLANE0);
		
		for(int i = 0; i <= num_tail; i++)
		{
			glPushMatrix();
				glTranslatef(i * 0.07 + ((0.015/num_tail) *0.01*i), 0, 0);
				glutSolidSphere(0.05 - (0.015/num_tail) * i, 32, 32);
				printf("%f\n", 0.05 - (0.015/num_tail) * i);
			glPopMatrix();
		}
	glPopMatrix();
	
	glDisable(GL_CLIP_PLANE0);
}