#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#define TIMER_TD 1
#define TIMER_INTERVAL 700

static void on_display(void);

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	/* Kreira se prozor. */
	glutInitWindowSize(1100, 650);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	/* Registruju se callback funkcije. */
	glutDisplayFunc(on_display);

	/* Obavlja se OpenGL inicijalizacija. */
	glClearColor(0, 0.6, 0, 0);

	/* Program ulazi u glavnu petlju. */
	glutMainLoop();

	return 0;

}

static void on_display()
{

	glClear(GL_COLOR_BUFFER_BIT);


	glutSwapBuffers();

}





