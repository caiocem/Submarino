// 
// INF390 - Computa��o Gr�fica
//
// Visualiza�ao 3D
// Desenha um cubo centrado na origem com faces de -1 a 1
// coloridas com diferentes cores
//
// Permite que a posicao do observador seja alterada usando o teclado 
// X -> soma uma unidade na coordenada x do observador 
// x -> subtrai  uma unidade na coordenada x do observador 
// Y -> soma uma unidade na coordenada y do observador 
// y -> subtrai  uma unidade na coordenada y do observador 
// Z -> soma uma unidade na coordenada z do observador 
// z -> subtrai  uma unidade na coordenada z do observador 
//
// Marcus V. A. Andrade - 25/09/2008
//

#include <GL/glut.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <stdlib.h>
#include <cmath>

#define PI 3.14159265
using namespace std;
void init(void);
void display(void);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);

//funcao para desenhar um cubo colorido (desenha as suas 6 faces)
void colorcube();
//inicializacao do menu
bool showmenu = true;
//tamanho da janela em pixels
int largurajanela = 900, alturajanela = 600;
//teclas
bool baixo,cima,esq,dir,frente,tras = false;
//movimento
double esque,direi = 0;
double PosX, PosZ = 0;
double PosY = 0.0;
double Rot = 0.0;
double radius = 6.0;

//vertices do cubo
GLfloat vertices[8][3] = { {-1.0,-1.0,1.0},{-1.0,1.0,1.0},{1.0,1.0,1.0},{1.0,-1.0,1.0},
{-1.0,-1.0,-1.0},{-1.0,1.0,-1.0},{1.0,1.0,-1.0},{1.0,-1.0,-1.0} };
//limitantes
double chao = -72;
double paredeFrente = 36;
double paredeTras = -paredeFrente; 
//cor de cada face do cubo  , 6=marrom, 0=preto,4=azul, 7=amarelo, 3=laranja
GLfloat colors[8][4] = { {0.0,0.0,0.0,0.8} , {1.0,0.0,0.0,0.8}, {1.0,1.0,0.0,0.8}, {1,0.5,0.0,0.8},
{0.0,0.0,1.0,0.8} , {0.215,0.165,0.0,0.8},{0.198,0.089,0.007,0.8},{128,128,0,0.5}};

//Vars de "animacao"
double animaNavio,animaCardume,animaJetski,animaTorpedo = 0;
bool animaNavioAux, animaCardumeAux, inverteCardume, torpedo = false;

//posicao do observador (camera)
bool POV = 1;
GLdouble center[] = {0.0, 0.0, 0.0};
GLdouble viewer[] = {-1*radius, 0.0, 0.0};
GLdouble viewVector[] = {1.0, 0.0, 0.0};

// desenha uma face do cubo
// a, b, c e d sao indices no vetor de vertices
// ncolor eh um indice no vetor de cores
void Parser(string nome, int cor) {  
    fstream f;
    f.open (nome);
	vector<vector<GLdouble> > Vertices;
	GLdouble V;
	char carac;
	int counter = 0;
	double i = 0;
    string lixo;
    for(int i=0;i<3;i++)
        getline(f,lixo);
	while(true){
		f>>carac;
		if(carac != 'v')
			break;
		vector<GLdouble> vertice;
		f>>V;
		vertice.push_back(V);
		f>>V;
		vertice.push_back(V);
		f>>V;
		vertice.push_back(V);
		Vertices.push_back(vertice);
		counter++;
	}
	int num1,num2,num3;
	int conta = 0;
    int lixo2;
    if (carac == 's') {
        f>>lixo2;
        f>>carac;
    }
	while(!f.eof()){
		f>>num1;f>>num2;f>>num3;
        if (cor != 0)
            glColor4fv(colors[cor]);
        else
		    glColor4f(0,0,0,0.9);
		glBegin(GL_TRIANGLES);
			glVertex3f (Vertices[num1-1][0], Vertices[num1-1][1], Vertices[num1-1][2]);
			glVertex3f (Vertices[num2-1][0], Vertices[num2-1][1], Vertices[num2-1][2]);
			glVertex3f (Vertices[num3-1][0], Vertices[num3-1][1], Vertices[num3-1][2]);
		glEnd();
		f>>carac;
        if(carac == 's') f>>lixo2;
		conta++;
    }
	f.close();
}

void texto(string s,float x,float y,float z) //func de texto usando bitmap
{ 
  glColor3f(1.0,0.0,0.0); 
  glRasterPos3f( x, y , z);
  void * font = GLUT_BITMAP_9_BY_15;
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
	  char c = *i;
	  glutBitmapCharacter(font, c);
	}
}

void desenha_menu () {
	texto("                          Menu de Comandos                   ", center[0] + 4*viewVector[0], 24+center[1]/1.98, viewVector[2] + center[2]/2.01);
	texto("Up (tecla direcional)     | Mover (verticalmente) para cima ", center[0] + 4*viewVector[0], 23.9+center[1]/1.98, 2*viewVector[2] + center[2]/2.01);
	texto("Down (tecla direcional)   | Mover (verticalmente) para baixo", center[0] + 4*viewVector[0], 23.7+center[1]/1.98, 1.95*viewVector[2]+center[2]/2.01);
    texto("Left (tecla direcional)   | Virar (aproximadamente) 5o para a direita", center[0] + 4*viewVector[0], 23.5+center[1]/1.98, 1.93*viewVector[2]+center[2]/2.01);
    texto("Right (tecla direcional)  | Virar (aproximadamente) 5o para a esquerda", center[0] + 4*viewVector[0], 23.3+center[1]/1.98, 1.90*viewVector[2]+center[2]/2.01);
    texto("W ou w                    | Ir para a frente", center[0] + 4*viewVector[0], 23.1+center[1]/1.98, 1.85*viewVector[2]+center[2]/2.01);
	texto("S ou s                    | Re", center[0] + 4*viewVector[0], 22.9+center[1]/1.98, 1.82*viewVector[2]+center[2]/2.01);
	texto("F ou f                    | Ponto de vista de fora do submarino", center[0] + 4*viewVector[0], 22.7+center[1]/1.98, 1.79*viewVector[2]+center[2]/2.01);
	texto("I ou i                    | Ponto de vista de dentro do submarino", center[0] + 4*viewVector[0], 22.5+center[1]/1.98, 1.75*viewVector[2]+center[2]/2.01);
	texto("H ou h                    | Apresentar/Ocultar um menu de ajuda", center[0] + 4*viewVector[0], 22.3+center[1]/1.98, 1.72*viewVector[2]+center[2]/2.01); 
}

void desenha_menu2 () {
    cerr << "aaa\n";
    // center[0]+2.73*viewVector[0], center[1]+1.5, center[2]+2.73*viewVector[2],
    
    double d = 2.2*viewVector[2]/3.73, e = -2.2*viewVector[0]/3.73;
	texto("                     Menu de Comandos                   "                , center[0] + 3.73*viewVector[0] + d, 2.0 + center[1], center[2] + 3.73*viewVector[2] + e);
	texto("Up (tecla direcional)     | Mover (verticalmente) para cima "            , center[0] + 3.73*viewVector[0] + d, 1.9 + center[1], center[2] + 3.73*viewVector[2] + e);
	texto("Down (tecla direcional)   | Mover (verticalmente) para baixo"            , center[0] + 3.73*viewVector[0] + d, 1.8 + center[1], center[2] + 3.73*viewVector[2] + e);
    texto("Left (tecla direcional)   | Virar (aproximadamente) 5o para a direita"   , center[0] + 3.73*viewVector[0] + d, 1.7 + center[1], center[2] + 3.73*viewVector[2] + e);
    texto("Right (tecla direcional)  | Virar (aproximadamente) 5o para a esquerda"  , center[0] + 3.73*viewVector[0] + d, 1.6 + center[1], center[2] + 3.73*viewVector[2] + e);
    texto("W ou w                    | Ir para a frente"                            , center[0] + 3.73*viewVector[0] + d, 1.5 + center[1], center[2] + 3.73*viewVector[2] + e);
	texto("S ou s                    | Re"                                          , center[0] + 3.73*viewVector[0] + d, 1.4 + center[1], center[2] + 3.73*viewVector[2] + e);
	texto("F ou f                    | Ponto de vista de fora do submarino"         , center[0] + 3.73*viewVector[0] + d, 1.3 + center[1], center[2] + 3.73*viewVector[2] + e);
	texto("I ou i                    | Ponto de vista de dentro do submarino"       , center[0] + 3.73*viewVector[0] + d, 1.2 + center[1], center[2] + 3.73*viewVector[2] + e);
	texto("H ou h                    | Apresentar/Ocultar um menu de ajuda"         , center[0] + 3.73*viewVector[0] + d, 1.1 + center[1], center[2] + 3.73*viewVector[2] + e); 
}

void quad(int a, int b, int c, int d, int ncolor) {
    glColor4f(0,0,1,0.8);
    if(ncolor == 7)
        glColor4f(0.2,0.5,1.0,0.7);
    glBegin(GL_QUADS);
        glVertex3fv(vertices[a]);
        glVertex3fv(vertices[b]);
        glVertex3fv(vertices[c]);
        glVertex3fv(vertices[d]);
    glEnd();
}

//desenha o cubo (faces no sentido anti-horario - face externa)
void colorcube() {
    glPushMatrix();

    glTranslatef(0,-46.1,0);
    glScalef(46,46,46);
    quad(0,3,2,1,4); 
    quad(2,3,7,6,4);
    quad(0,4,7,3,4); //fundo
    quad(1,2,6,5,7); //topo
    quad(4,5,6,7,4); 
    quad(0,1,5,4,4);
    
    glPopMatrix();
}

// define o tipo de projecao 
// glOrtho(left, right, bottom, top, near, far) -> projecao paralela
// glFrustum(left, right, bottom, top, near, far) -> projecao perspectiva
// gluPerspective(viewAngle, aspectRatio, near, far) -> projecao perspectiva
//   ^^^^^ draw distance
void init(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0); // cor para limpeza do buffer
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    //glOrtho(-5.0, 5.0, -5.0, 5.0, 5.0, 10.0); // proje��o paralela
      
    //glFrustum(-2.0, 2.0, -2.0, 2.0, 2.0, 10.0); // proje��o perspectiva
    
    gluPerspective(60.0,1.0,0.1,160.0); // proje��o perspectiva
    glMatrixMode(GL_MODELVIEW);
}

void Temporizador (int tempo) {
    bool a=0;

	if (baixo) {
        a=1;
        center[1] = max(center[1]-1.0,chao);
        viewer[1] = center[1];
	}
	if (cima) {
        a=1;
        center[1] = min(center[1]+1.0,0.0);
        viewer[1] = center[1];
	}
	if (frente) {
        a=1;
        viewer[0] += viewVector[0];
        center[0] += viewVector[0];
        viewer[2] += viewVector[2];
        center[2] += viewVector[2];

        if(center[0] > paredeFrente) {
            center[0] = paredeFrente;
            viewer[0] = center[0] - radius*viewVector[0];
        }
        else if(center[0] < paredeTras) {
            center[0] = paredeTras;
            viewer[0] = center[0] - radius*viewVector[0];
        }

        if(center[2] > paredeFrente) {
            center[2] = paredeFrente;
            viewer[2] = center[2] - radius*viewVector[2];
        }
        else if(center[2] < paredeTras) {
            center[2] = paredeTras;
            viewer[2] = center[2] - radius*viewVector[2];
        }
	}
	if (tras) {
        a=1;
        viewer[0] -= viewVector[0];
        center[0] -= viewVector[0];
        viewer[2] -= viewVector[2];
        center[2] -= viewVector[2];

        if(center[0] > paredeFrente) {
            center[0] = paredeFrente;
            viewer[0] = center[0] - radius*viewVector[0];
        }
        else if(center[0] < paredeTras) {
            center[0] = paredeTras;
            viewer[0] = center[0] - radius*viewVector[0];
        }

        if(center[2] > paredeFrente) {
            center[2] = paredeFrente;
            viewer[2] = center[2] - radius*viewVector[2];
        }
        else if(center[2] < paredeTras) {
            center[2] = paredeTras;
            viewer[2] = center[2] - radius*viewVector[2];
        }
	}
	if (esq){
        a=1;
		Rot-=5;
        // if(Rot<-1e-8) {Rot+=360.0;}
        viewVector[0] = cos(Rot*M_PI/180);
        viewer[0] = center[0]-radius*viewVector[0];
        viewVector[2] = sin(Rot*M_PI/180);
        viewer[2] = center[2]-radius*viewVector[2];
	}
	if (dir) {
        a=1;
		Rot+=5;
        // if(Rot>1e-8) {Rot-=360.0;}
        viewVector[0] = cos(Rot*M_PI/180);
        viewer[0] = center[0]-radius*viewVector[0];
        viewVector[2] = sin(Rot*M_PI/180);
        viewer[2] = center[2]-radius*viewVector[2];
	}
	// if (animaNavio < 1 && !animaNavioAux){ 
	// 		animaNavio +=0.01; }
	// if (animaNavio > -1 && animaNavioAux) 
	// 		animaNavio -=0.01; 
	// if (abs(animaNavio)>=0.4)  
	// 	animaNavioAux = !animaNavioAux;
		
	// if (animaCardume < 8 && !animaCardumeAux)
	// 	animaCardume +=0.07;
	// if (animaCardume > -8 && animaCardumeAux)
	// 	animaCardume -= 0.07;
	// if (abs(animaCardume)>=7) { animaCardumeAux = !animaCardumeAux; inverteCardume = !inverteCardume;}
    // animaJetski += 2;
    // if (torpedo)
    //     animaTorpedo+=1;
    // if (animaTorpedo>20){
    //     animaTorpedo=0;
    //     torpedo = false;
    // }
	glutPostRedisplay();
	glutTimerFunc(1,Temporizador,0);

    if(a) {
        cerr << center[0] << "," << center[1] << "," << center[2] << " = " << viewer[0] << "," << viewer[1] << "," << viewer[2] << " = " << viewVector[0] << "," << viewVector[1] << "," << viewVector[2] << " " << Rot << endl;
        a=0;
    }
}

void display(void) {
    double seno = sin (Rot*PI/180);
    double coseno = cos (Rot*PI/180);
    //se tu for consertar o giro, usa as coordenadas certas pro torpedo
    //(movZ = 0, nao ta andando nessa dimension)
    //(inicialmente tem um translate de 20 pra ficar na posicao certa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //limpa a janela
    glLoadIdentity();

    if (POV) //POV outside
        // gluLookAt(0.0,0.0,8.0, // define posicao do observador
		// 0.0,20.0,0.0,                                   // ponto de interesse (foco)
		// 0.0, 1.0, 0.0);  
        gluLookAt(viewer[0],viewer[1], viewer[2], // define posicao do observador
		center[0], center[1], center[2],                                   // ponto de interesse (foco)
		0.0, 1.0, 0.0);                                                    
	else //POV inside
        gluLookAt(viewer[0]+(radius+2.23)*viewVector[0],viewer[1]+1.5, viewer[2]+(radius+2.23)*viewVector[2], // define posicao do observador
		center[0]+2.73*viewVector[0], center[1]+1.5, center[2]+2.73*viewVector[2],                                   // ponto de interesse (foco)
		0.0, 1.0, 0.0);
		// gluLookAt(-1.5+viewer[0]-10-PosX/6.7,viewer[1]-1.7+PosY/6.65,viewer[2]+PosZ, // define posicao do observador
		// -2.5-PosX/6.6, 6.5+PosY/6.7, 0.0+PosZ,                                 // ponto de interesse (foco)
		// 0.0, 1.0, 0.0);   		 // vetor de "view up"
	// glRotatef(-90,0,1,0);
    // glScalef(0.3,0.3,0.3);
    // glPushMatrix(); {
    // glScalef(0.5,0.5,0.5);
    // glTranslatef(0,40,0); 
	// glTranslatef(PosX,PosY,PosZ);
    //glTranslatef(coseno,0,-seno);
    // };
    
    glPushMatrix();

    glTranslated(center[0], center[1], center[2]);
	glRotatef(-1*Rot,0,1,0);  //fazer translacao pra rodar direito
    glRotatef(90,0,1,0);
    glTranslated(-1*center[0], -1*center[1], -1*center[2]);
    glTranslated(center[0], center[1], center[2]);

    Parser("models/submarino.obj", 0); //0 = sem cor
    glPopMatrix();	
    // glPushMatrix();{
	

    // }
    glPushMatrix();
    
        glTranslatef(0,-63,0);
        glScalef(6,1,6);
    
    Parser("models/areia.obj",7); //7 = amarelo
    glPopMatrix();
    /*
    glPushMatrix();{
        glTranslatef(-6,-16.7,-3);
        glScalef(0.4,0.4,0.4);
    }
    Parser("models/tesouro.obj",6); //marrom
    glPopMatrix(); 
    glPushMatrix();{
        glTranslatef(10,20,0);
        glTranslatef(0,animaNavio,0);
    }
    Parser("models/navio.obj",6);
    glPopMatrix();
    glPushMatrix();{
        glTranslatef(-15,-10,6);
        glTranslatef(0,0,animaCardume);
        if (inverteCardume) {
            glTranslatef(15,10,-6);
            glScalef(1,1,-1);
            glTranslatef(-15,-10,6);
            //inverte = 0;
        }
    }
    Parser("models/cardumao.obj",0);
    glPopMatrix();
    glPushMatrix();{
        glTranslatef(18,-19,-3);
        glScalef(4,4.5,4);
    }
    Parser("models/abacaxi.obj",3); //laranja
    glPopMatrix();
    
    glPushMatrix();{
		glTranslatef(-15,20,-7);
		//glRotatef(animaJetski,0,1,0);
		glScalef(0.2,0.2,0.2);
        glTranslatef(0,animaNavio,0);
    }
    Parser("models/jetski.obj",5); //cinza
    glPopMatrix(); 
    if (torpedo) { //se tu for consertar o giro usa as coordenadas certas pro torpedo
        glPushMatrix();{
            double PosY_torp = PosY;
            double PosX_torp = PosX;
            double cosenoTorp = coseno;double senoTorp = seno;
            glTranslatef(0,PosY_torp,PosX_torp);
            glTranslatef(animaTorpedo*cosenoTorp,0,animaTorpedo*senoTorp);
            //glTranslatef()
            double rotTorp = -Rot;
            glRotatef(rotTorp,0,1,0);
            //glTranslatef(-animaTorpedo*coseno,0,-animaTorpedo*seno);
        }
        Parser("models/torpedo.obj",0); //cinza
        glPopMatrix();
    }
    glScalef(2,2,2);
    */
    colorcube(); //desenha o cubo
    if(showmenu && POV==1) 
		desenha_menu();
	else if(showmenu && POV==0) 
		desenha_menu2();
    glFlush();
    glutSwapBuffers(); //usando double buffer (para evitar 'flicker')
}

void keyboard(unsigned char key, int x, int y) {
    switch(key) {
		case 27: exit(0); //ESC
        case 32: torpedo=true; break;//space
		case 'w': case 'W': frente=true; break;
		case 's': case 'S': tras=true; break;
		case 'h': case 'H': showmenu = !showmenu; break;
	    case 'i': case 'I': POV = 0;  break;
		case 'f': case 'F': POV = 1;  break;
	}
    //display();
}

void Upkeyboard(unsigned char key, int x, int y) {
    switch(key) {
		case 27: exit(0); //ESC
		case 'w': case 'W': frente=false; break;
		case 's': case 'S': tras=false;  break;
		//case 'h': showmenu = !showmenu; break; menu nao aplica manter pressionado
	}
    //display();
}

void UpSpecialKeys(int key, int x, int y) //callback das setas
{
  switch(key)
  {
    case GLUT_KEY_DOWN: baixo = false;break;
  
	case GLUT_KEY_UP: cima = false;break;
                    			
	case GLUT_KEY_LEFT: esq = false;break;

	case GLUT_KEY_RIGHT: dir = false;break;
  }
  //glutPostRedisplay();
}

void SpecialKeys(int key, int x, int y) //callback das setas
{
  switch(key)
  {
    case GLUT_KEY_DOWN: baixo = true; break;
  
	case GLUT_KEY_UP: cima = true; break;
                    			
	case GLUT_KEY_LEFT: esq = true;break;

	case GLUT_KEY_RIGHT: dir = true;break;
  }
  //glutPostRedisplay();
}


int main(int argc, char **argv) {
    glutInit(&argc,argv); //inicializa a glut
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //tipo de buffer/cores
    glutInitWindowSize(largurajanela, alturajanela); //dimens�es da janela
    glutInitWindowPosition(100, 100); //posicao da janela
    glutCreateWindow("Visualizacao 3D - Movimento de camera "); //cria a janela
    init();
    glutDisplayFunc(display); //determina fun��o callback de desenho
    glutKeyboardFunc(keyboard); //determina fun��o callback de teclado
    glutKeyboardUpFunc(Upkeyboard);
    glutSpecialUpFunc(UpSpecialKeys);
    glutSpecialFunc(SpecialKeys);
    glEnable(GL_DEPTH_TEST); //habilita remo��o de superf�cies ocultas
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glutTimerFunc(100, Temporizador,0);
    glutMainLoop();
    return 0;
}
