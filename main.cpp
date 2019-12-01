// 
// INF390 - CG : Submarino
// Caio O H Henrici - 92558
// Rodrigo E O B Chichorro - 92535


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
double Rot = 0.0;
double radius = 7.5;

//vertices do cubo
GLfloat vertices[8][3] = { {-1.0,-1.0,1.0},{-1.0,1.0,1.0},{1.0,1.0,1.0},{1.0,-1.0,1.0},
{-1.0,-1.0,-1.0},{-1.0,1.0,-1.0},{1.0,1.0,-1.0},{1.0,-1.0,-1.0} };

//limitantes
double chao = -72;
double paredeFrente = 36;
double paredeTras = -paredeFrente; 

//cor de cada face do cubo  , 6=marrom, 0=preto,4=azul, 7=amarelo, 3=laranja
GLfloat colors[8][4] = { {0.0,0.0,0.0,1.0} , {1.0,0.0,0.0,1.0}, {1.0,1.0,0.0,1.0}, {1,0.5,0.0,1.0},
{0.0,0.0,1.0,1.0} , {0.215,0.165,0.0,1.0},{0.198,0.089,0.007,1.0},{128,128,0,1.0}};

//Vars de "animacao"
double animaNavio,animaCardume,animaJetski,animaTorpedo = 0;
bool animaNavioAux, animaCardumeAux, inverteCardume, torpedo = false;

//posicao do observador (camera)
double POV = 1;
double CamX,CamZ=0;double CamY=1;
double FocoX,FocoY,FocoZ = 0;
bool trocaCam,initCam = false;
GLdouble center[] = {0.0, 0.0, 0.0}; //Posicao do submarino
GLdouble viewer[] = {-1*radius, 0.0, 0.0}; //Posicao da camera
GLdouble viewVector[] = {1.0, 0.0, 0.0}; //Vetor unitario que aponta de viewer para center
//Iluminação
bool Luz,ModoLuz = false; bool Luz1,Luz2 = false;

bool Parser(const char * nome, int cor) {  
    //fstream f;
    //f.open (nome);
    FILE * f = fopen(nome, "r");
    if( f == NULL ){
        printf("Nao da pra abrir\n");
        return false;
    }
	vector<vector<GLdouble> > Vertices;
    vector<vector<GLdouble> > TextureVertices;
    vector<vector<GLdouble> > NormalVertices;
    while( 1 ){
        char lineHeader[128];
        int res = fscanf(f, "%s", lineHeader); // le primeira palavra da linha
        if (res == EOF)
            break; // terminou
        if ( strcmp( lineHeader, "v" ) == 0 ){ //VERTICE
            double x,y,z;
            fscanf(f, "%lf %lf %lf\n", &x, &y, &z ); //ler vertice
            vector<GLdouble> vertice; vertice.push_back(x);vertice.push_back(y);vertice.push_back(z);
            Vertices.push_back(vertice);
        }else if ( strcmp( lineHeader, "vt" ) == 0 ){  //VERTICE TEXTURA
            double x,y;
            fscanf(f, "%lf %lf\n", &x, &y ); //ler textura
            vector<GLdouble> Texvertice; Texvertice.push_back(x);Texvertice.push_back(y);
            TextureVertices.push_back(Texvertice);
        } else if ( strcmp( lineHeader, "vn" ) == 0 ){   //VERTICE ENORMAL
            double x,y,z;
            fscanf(f, "%lf %lf %lf\n", &x, &y, &z ); //ler normal
            vector<GLdouble> Normalvertice; Normalvertice.push_back(x);Normalvertice.push_back(y);Normalvertice.push_back(z);
            NormalVertices.push_back(Normalvertice);
        } else if ( strcmp( lineHeader, "f" ) == 0 ){
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            // ler o indice do vertice "/" indice do vetor uv "/" indice da normal
            int matches = fscanf(f, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], 
            &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            if (matches != 9){
                printf("Erro no formato\n");
                return false;
            }
            // o q faz com uvIndex[0]uvIndex[1]uvIndex[2] e normalIndex[0]normalIndex[1]normalIndex[2] ???
            if (cor != 0)
            glColor4fv(colors[cor]);
            else
                glColor4f(0,0,0,0.9);
            glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT);
            glBegin(GL_TRIANGLES);
                glNormal3f (NormalVertices[normalIndex[0]-1][0],NormalVertices[normalIndex[0]-1][1],NormalVertices[normalIndex[0]-1][2]);
                glVertex3f (Vertices[vertexIndex[0]-1][0], Vertices[vertexIndex[0]-1][1], Vertices[vertexIndex[0]-1][2]);
                glNormal3f (NormalVertices[normalIndex[1]-1][0],NormalVertices[normalIndex[1]-1][1],NormalVertices[normalIndex[1]-1][2]);
                glVertex3f (Vertices[vertexIndex[1]-1][0], Vertices[vertexIndex[1]-1][1], Vertices[vertexIndex[1]-1][2]);
                glNormal3f (NormalVertices[normalIndex[2]-1][0],NormalVertices[normalIndex[2]-1][1],NormalVertices[normalIndex[2]-1][2]);
                glVertex3f (Vertices[vertexIndex[2]-1][0], Vertices[vertexIndex[2]-1][1], Vertices[vertexIndex[2]-1][2]);
            glEnd();
        }
    }
/*	PARSER ANTIGO
    GLdouble V;
	char carac;
	int counter = 0;
	double i = 0;
    string lixo;
    for(int i=0;i<4;i++)
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
    if (carac == 's') { //remover o smoth
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
        if(carac == 's') f>>lixo2; //remover smoth entre faces
		conta++;
    }
    */
	fclose (f);

}

void texto(string s,float x,float y,float z) //func de texto usando bitmap
{ 
  glColor3f(1.0,0.0,0.0);   //cor vermelha para contrastar com o resto
  glRasterPos3f( x, y , z);
  void * font = GLUT_BITMAP_9_BY_15;
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
	  char c = *i;
	  glutBitmapCharacter(font, c);
	}
}

void desenha_menu () {
    double d = 13.0*viewVector[2]/3.73, e = -13.0*viewVector[0]/3.73;
	texto("                    Menu de Comandos                   "                 , center[0] + 3.73*viewVector[0] + d, 4.7 + center[1], center[2] + 3.73*viewVector[2] + e);
	texto("Up (tecla direcional)     | Mover (verticalmente) para cima "            , center[0] + 3.73*viewVector[0] + d, 4.4 + center[1], center[2] + 3.73*viewVector[2] + e);
	texto("Down (tecla direcional)   | Mover (verticalmente) para baixo"            , center[0] + 3.73*viewVector[0] + d, 4.1 + center[1], center[2] + 3.73*viewVector[2] + e);
    texto("Left (tecla direcional)   | Virar (aproximadamente) 5o para a direita"   , center[0] + 3.73*viewVector[0] + d, 3.8 + center[1], center[2] + 3.73*viewVector[2] + e);
    texto("Right (tecla direcional)  | Virar (aproximadamente) 5o para a esquerda"  , center[0] + 3.73*viewVector[0] + d, 3.5 + center[1], center[2] + 3.73*viewVector[2] + e);
    texto("W ou w                    | Ir para a frente"                            , center[0] + 3.73*viewVector[0] + d, 3.2 + center[1], center[2] + 3.73*viewVector[2] + e);
	texto("S ou s                    | Re"                                          , center[0] + 3.73*viewVector[0] + d, 2.9 + center[1], center[2] + 3.73*viewVector[2] + e);
	texto("F ou f                    | Ponto de vista de fora do submarino"         , center[0] + 3.73*viewVector[0] + d, 2.6 + center[1], center[2] + 3.73*viewVector[2] + e);
	texto("I ou i                    | Ponto de vista de dentro do submarino"       , center[0] + 3.73*viewVector[0] + d, 2.3 + center[1], center[2] + 3.73*viewVector[2] + e);
	texto("H ou h                    | Apresentar/Ocultar um menu de ajuda"         , center[0] + 3.73*viewVector[0] + d, 2.0 + center[1], center[2] + 3.73*viewVector[2] + e); 
}

void desenha_menu2 () {  
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

//desenha o cubo com faces no sentido anti-horario/externa
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


// gluPerspective(viewAngle, aspectRatio, near, far) -> projecao perspectiva
void init(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0); // cor para limpeza do buffer
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();    
    gluPerspective(60.0,1.0,0.1,160.0); // projecao perspectiva
    glMatrixMode(GL_MODELVIEW);
    GLfloat mat_ambient[ ] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat mat_diffuse[ ] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat mat_specular[ ] = { 0.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[ ] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[ ] = { 0.0, 20.0, 0.0, 0.0 };
    GLfloat white_light[ ] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat red_light[ ] = { 1.0, 0.0, 0.0, 0.0 };
    glClearColor (1.0, 1.0, 1.0, 1.0);
    // glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    // glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    // glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    // glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_AMBIENT, white_light);
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
}

void Temporizador (int tempo) { //funcao para dar mais fluides aos movimentos, incrementa todas
    bool a=0;                   //as variaveis relativas ao movimento

	if (baixo) {  //descer
        a=1;
        center[1] = max(center[1]-1.0,chao); //tratar limite inferior
        viewer[1] = center[1];
	}

	if (cima) {  //subir
        a=1;
        center[1] = min(center[1]+1.0,0.0); //tratar limite superior
        viewer[1] = center[1];
	}

	if (frente) { //andar para frente
        a=1;
        viewer[0] += viewVector[0];
        center[0] += viewVector[0];
        viewer[2] += viewVector[2];
        center[2] += viewVector[2];
        //Tratamento dos limites do cubo a partir da movimentacao dianteira
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
	if (tras) { //andar para tras
        a=1;
        viewer[0] -= viewVector[0];
        center[0] -= viewVector[0];
        viewer[2] -= viewVector[2];
        center[2] -= viewVector[2];
        //Tratamento dos limites do cubo a partir da movimentacao traseira
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
	if (esq){      //rotacao para esquerda
        a=1;
		Rot-=5;
        viewVector[0] = cos(Rot*M_PI/180);
        viewer[0] = center[0]-radius*viewVector[0];
        viewVector[2] = sin(Rot*M_PI/180);
        viewer[2] = center[2]-radius*viewVector[2];
	}

	if (dir) {   //rotacao para direita
        a=1;
		Rot+=5;
        viewVector[0] = cos(Rot*M_PI/180);
        viewer[0] = center[0]-radius*viewVector[0];
        viewVector[2] = sin(Rot*M_PI/180);
        viewer[2] = center[2]-radius*viewVector[2];
	}
    // animacao do navio
	if (animaNavio < 1 && !animaNavioAux){ 
	 	animaNavio +=0.01; }
	if (animaNavio > -1 && animaNavioAux) 
	 	animaNavio -=0.01; 
	if (abs(animaNavio)>=0.4)  
	animaNavioAux = !animaNavioAux;
	// animacao do cardume	
	if (animaCardume < 8 && !animaCardumeAux)
	    animaCardume +=0.07;
	if (animaCardume > -8 && animaCardumeAux)
	 	animaCardume -= 0.07;
	if (abs(animaCardume)>=7) { 
        animaCardumeAux = !animaCardumeAux; 
        inverteCardume = !inverteCardume;
    }
    /*Suavisacao da transicao da camera
    if (POV == 0 && CamX<abs((radius+2.23)*viewVector[0]) && trocaCam){
		if (viewVector[2] < 0)
            CamX-=0.01;else CamX+=0.01; } */
	if (POV == 0 && CamY<1.5 && trocaCam){
		CamY+=0.05;} /*
    if (POV == 0 && CamZ<abs((radius+2.23)*viewVector[2]) && trocaCam){
		if (viewVector[2] < 0)
            CamZ-=0.01; else CamZ+=0.01; }
    if (POV == 0 && FocoX<abs(2.73*viewVector[0]) && trocaCam){
		if (viewVector[0] < 0)
            FocoX-=0.01;else FocoX+=0.01;}
    if (POV == 0 && FocoY<0.5 && trocaCam){
		FocoY+=0.01;}
    if (POV == 0 && FocoZ<abs(2.73*viewVector[2]) && trocaCam){
		if (viewVector[2] < 0)
            FocoZ-=0.01; else FocoZ+=0.01;}
	if (POV == 1 && CamX>0 && !trocaCam && initCam){
		if (CamX<0)
            CamX+=0.01;else if(CamX>0)CamX-=0.01; } */
	if (POV == 1 && CamY>1 && !trocaCam && initCam){
		CamY-=0.05; } /*
    if (POV == 1 && abs(CamZ)>0 && !trocaCam && initCam){
		if (CamZ<0)
            CamZ+=0.01;else if(CamZ>0) CamZ-=0.01; }
	if (POV == 1 && abs(FocoX)>0 && !trocaCam && initCam){
		if(FocoX<0)
            FocoX+=0.01;else if(FocoX>0)FocoX-=0.01;}
	if (POV == 1 && FocoY>0 && !trocaCam && initCam){
		FocoY-=0.01;}
    if (POV == 1 && abs(FocoZ)>0 && !trocaCam && initCam){
		if(FocoZ<0)
            FocoZ+=0.01;else if(FocoZ>0)FocoZ-=0.01;}
    */
	glutPostRedisplay();
	glutTimerFunc(1,Temporizador,0);

    // cerr para debug, caso seja necessario analisar os valores das variaveis durante a execucao
    // if(a) {
        //cerr << center[0] << "," << center[1] << "," << center[2] << " = " << viewer[0] << "," << viewer[1] << "," << viewer[2] << " = " << viewVector[0] << "," << viewVector[1] << "," << viewVector[2] << " " << Rot << endl;
        // a=0;
    // }
}

void display(void) {
    double seno = sin (Rot*PI/180);  
    double coseno = cos (Rot*PI/180);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //limpa a janela
    glLoadIdentity();
    //versao suave  { gluLookAt(CamX+viewer[0],CamY+viewer[1],CamZ+viewer[2], // define posicao do observador
	//FocoX+center[0], FocoY+center[1]+1, FocoZ+center[2],                                   // ponto de interesse (foco)
	//0.0, 1.0, 0.0);} 
    if (POV) //POV outside  
        gluLookAt(viewer[0],viewer[1]+CamY, viewer[2], // define posicao do observador
		center[0], center[1], center[2],               // ponto de interesse (foco)
		0.0, 1.0, 0.0);                                                    
	else //POV inside
        gluLookAt(viewer[0]+(radius+2.23)*viewVector[0],viewer[1]+CamY, viewer[2]+(radius+2.23)*viewVector[2], // define posicao do observador
		center[0]+2.73*viewVector[0], center[1]+CamY, center[2]+2.73*viewVector[2],                            // ponto de interesse (foco)
		0.0, 1.0, 0.0);
    glPushMatrix();

    glTranslated(center[0], center[1], center[2]);
	glRotatef(-1*Rot,0,1,0);  //fazer translacao pra rodar direito
    glRotatef(90,0,1,0);
    glTranslated(-1*center[0], -1*center[1], -1*center[2]);
    glTranslated(center[0], center[1], center[2]);

    Parser("models/submarino2.obj", 0); //0 = sem cor
    glPopMatrix();	

    glPushMatrix();
    
        glTranslatef(0,-63,0);
        glScalef(6,1,6);
    
    Parser("models/areia2.obj",7); //7 = amarelo
    glPopMatrix();
    
    glPushMatrix();{
        glTranslatef(-6,-73.7,-3);
        glScalef(1.5,1.5,1.5);
    }
    Parser("models/tesouro2.obj",6); //marrom 
    glPopMatrix(); 
    glPushMatrix();{
        glTranslatef(12,0,0);
        glScalef(2,2,2);
        glTranslatef(0,animaNavio,0);
    }
    Parser("models/navio2.obj",6);
    glPopMatrix(); 
    glPushMatrix();{
        glTranslatef(-45,-50,6);
        glScalef(2,2,2);
        glTranslatef(0,0,animaCardume);
        if (inverteCardume) {
            glTranslatef(15,10,-6);
            glScalef(1,1,-1);
            glTranslatef(-15,-10,6);
        }
    }
    Parser("models/cardumao2.obj",0); 
    glPopMatrix();
    glPushMatrix();{
        glTranslatef(28,-75,-3);
        glScalef(6,8,6);
    }
    Parser("models/abacaxi2.obj",3); //laranja
    glPopMatrix();
    
    glPushMatrix();{
		glTranslatef(-15,0,-7);
        glScalef(2,2,2);
		glScalef(0.2,0.2,0.2);
        glTranslatef(0,animaNavio,0);
    }
    Parser("models/jetski2.obj",5); //cinza
    glPopMatrix();

    // Funcao extra, nao finalizada
  /*  if (torpedo) { 
        glPushMatrix();{
            double posTorpedo[3] = {center[0],center[1],center[2]};
            //glTranslatef(0,PosY_torp,PosX_torp);
            // for(int i=0;)
            glTranslatef(animaTorpedo*viewVector[0], 0, animaTorpedo*viewVector[2]);
        }
        Parser("models/torpedo.obj",0); //cinza
        glPopMatrix();
    }
	*/
    
    colorcube(); //desenha o cubo
    if(showmenu && POV==1)         //caso 'h' seja pressionado chama o menu do ponto de vista atua
		desenha_menu();
	else if(showmenu && POV==0) 
		desenha_menu2();
    glFlush();
    glutSwapBuffers(); //usando double buffer (para evitar 'flicker')
}

void keyboard(unsigned char key, int x, int y) { //callback teclado asc
    switch(key) {
		case 27: exit(0); //ESC
        case 32: torpedo=true; break;//space
		case 'w': case 'W': frente=true; break;
		case 's': case 'S': tras=true; break;
		case 'h': case 'H': showmenu = !showmenu; break;
        case 'i': case 'I': if (POV ==0) break;POV = 0; trocaCam = !trocaCam; initCam=true;break;
		case 'f': case 'F': if (POV ==1) break;POV = 1; trocaCam = !trocaCam; initCam=true;break;
	}
}

void Upkeyboard(unsigned char key, int x, int y) { 
    switch(key) {
		case 27: exit(0); //ESC
		case 'w': case 'W': frente=false; break;
		case 's': case 'S': tras=false;  break;
        case 'l': case 'L': Luz=!Luz; if(Luz)glEnable(GL_LIGHTING); else glDisable(GL_LIGHTING); break;
        case 'g': case 'G': 
                ModoLuz=!ModoLuz; if(ModoLuz){ glDisable(GL_FLAT);glEnable(GL_SMOOTH);}
                else {glDisable(GL_SMOOTH);glEnable(GL_FLAT);} break;
		case '1':  Luz1 = !Luz1; if(Luz1) glEnable(GL_LIGHT1); else glDisable(GL_LIGHT1); break;
        case '2':  Luz2 = !Luz2; if(Luz2) glEnable(GL_LIGHT2); else glDisable(GL_LIGHT2); break;
        //case 'h': showmenu = !showmenu; break; menu nao aplica manter pressionado
	}
}

void UpSpecialKeys(int key, int x, int y)    //soltar os direcionais para o movimento
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
}


int main(int argc, char **argv) {
    glutInit(&argc,argv); //inicializa a glut
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //tipo de buffer/cores
    glutInitWindowSize(largurajanela, alturajanela); //dimensoes da janela
    glutInitWindowPosition(100, 100); //posicao da janela
    glutCreateWindow("CG - SubMarinho"); //cria a janela
    init();
    glutDisplayFunc(display); //determina funcaoo callback de desenho
    glutKeyboardFunc(keyboard); //determina funcao callback de teclado
    glutKeyboardUpFunc(Upkeyboard); //determina funcao release do teclado
    glutSpecialUpFunc(UpSpecialKeys); //determina funcao release do teclado
    glutSpecialFunc(SpecialKeys); //determina funcao callback das seta 
    glEnable(GL_DEPTH_TEST); //habilita remocao de superficies ocultas
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //determina o alpha
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D); //habilitar textura
    glEnable(GL_BLEND); //habilita o alpha nas superficies
    glutTimerFunc(100, Temporizador,0); //determina funcao de Timer
    glutMainLoop(); //loop
    return 0;
}
