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
{0.0,0.0,1.0,1.0} , {0.215,0.165,0.0,1.0},{0.198,0.145,0.087,1.0},{128,128,0,1.0}};

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
//Textura
GLuint texture[8];
struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
};
typedef struct Image Image;
#define checkImageWidth 64
#define checkImageHeight 64
GLubyte checkImage[checkImageWidth][checkImageHeight][3];
bool Parser(const char * nome, int cor, int numTexture) {  
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
            //  if (cor != 0)
            //  glColor4fv(colors[cor]);
            //  else
            //      glColor4f(0.1,0.1,0.1,1);
            glClearColor(1.0, 1.0, 1.0, 1.0);
            glEnable(GL_TEXTURE_2D); //habilitar textura
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glColor3ub(255, 255, 255);
            if (numTexture == 4) glColor3f(1,0.3,0.0);
            if (numTexture == 1) glColor3f(0.897,0.945,0.587);
            if(numTexture != 2) {
                glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
                glBindTexture ( GL_TEXTURE_2D, texture[numTexture]);
                //cerr<<TextureVertices[uvIndex[0]-1][0]<<" "<<TextureVertices[uvIndex[0]-1][1];
                glBegin(GL_TRIANGLES);
                    glNormal3f (NormalVertices[normalIndex[0]-1][0],NormalVertices[normalIndex[0]-1][1],NormalVertices[normalIndex[0]-1][2]);
                    glTexCoord2f(TextureVertices[uvIndex[0]-1][0],TextureVertices[uvIndex[0]-1][1]);
                    glVertex3f (Vertices[vertexIndex[0]-1][0], Vertices[vertexIndex[0]-1][1], Vertices[vertexIndex[0]-1][2]);
                    glNormal3f (NormalVertices[normalIndex[1]-1][0],NormalVertices[normalIndex[1]-1][1],NormalVertices[normalIndex[1]-1][2]);
                    glTexCoord2f(TextureVertices[uvIndex[1]-1][0],TextureVertices[uvIndex[1]-1][1]);
                    glVertex3f (Vertices[vertexIndex[1]-1][0], Vertices[vertexIndex[1]-1][1], Vertices[vertexIndex[1]-1][2]);
                    glNormal3f (NormalVertices[normalIndex[2]-1][0],NormalVertices[normalIndex[2]-1][1],NormalVertices[normalIndex[2]-1][2]);
                    glTexCoord2f(TextureVertices[uvIndex[2]-1][0],TextureVertices[uvIndex[2]-1][1]);
                    glVertex3f (Vertices[vertexIndex[2]-1][0], Vertices[vertexIndex[2]-1][1], Vertices[vertexIndex[2]-1][2]);
                glEnd();
                glDisable(GL_TEXTURE_2D); //habilitar textura
            }
            else {
                glColor4fv(colors[cor]);
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
    }
	fclose (f);
}

void makeCheckImage(void){
    int i, j, c;
    for (i = 0; i < checkImageWidth; i++) {
        for (j = 0; j < checkImageHeight; j++) {
            c = ((((i&0x8)==0)^((j&0x8)==0)))*255;            
            checkImage[i][j][0] = (GLubyte) c;
            checkImage[i][j][1] = (GLubyte) c;
            checkImage[i][j][2] = (GLubyte) c;
        }
    }
}

int ImageLoad(char *filename, Image *image) {
    FILE *file;
    unsigned size; // size of the image in bytes.
    unsigned long i; // standard counter.
    unsigned short int planes; // number of planes in image (must be 1)
    unsigned short int bpp; // number of bits per pixel (must be 24)
    char temp; // temporary color storage for bgr-rgb conversion.
    // make sure the file is there.
    if ((file = fopen(filename, "rb"))==NULL){
        printf("File Not Found : %s\n",filename);
        return 0;
    }
    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);
    // read the width
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
        printf("Error reading width from %s.\n", filename);
        return 0;
    }
    //printf("Width of %s: %lu\n", filename, image->sizeX);
    // read the height
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
        printf("Error reading height from %s.\n", filename);
        return 0;
    }
    //printf("Height of %s: %lu\n", filename, image->sizeY);
    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 3;
    // read the planes
    if ((fread(&planes, 2, 1, file)) != 1) {
        printf("Error reading planes from %s.\n", filename);
        return 0;
    }
    if (planes != 1) {
        printf("Planes from %s is not 1: %u\n", filename, planes);
        return 0;
    }
    // read the bitsperpixel
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
        printf("Error reading bpp from %s.\n", filename);
        return 0;
    }
    if (bpp != 24) {
        printf("Bpp from %s is not 24: %u\n", filename, bpp);
        return 0;
    }
    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);

    // read the data.
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
        printf("Error allocating memory for color-corrected image data");
        return 0;
    }
    if ((i = fread(image->data, size, 1, file)) != 1) {
        printf("Error reading image data from %s.\n", filename);
        return 0;
    }
    // for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
    //     temp = image->data[i];
    //     image->data[i] = image->data[i+2];
    //     image->data[i+2] = temp;
    // }
    // we're done.
    return 1;
}
Image * loadTexture(char *file_name){
    Image *image_aux;
    // allocate space for texture
    image_aux = (Image *) malloc(sizeof(Image));
    if (image_aux == NULL) {
        printf("Error allocating space for image");
        exit(0);
    }
    if (!ImageLoad(file_name, image_aux)) {
        exit(1);
    }
    return image_aux;
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
    //glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT);
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
    GLfloat light_position[ ] = { 0.0, 25.0, 0.0, 0.0 };
    GLfloat white_light[ ] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat red_light[ ] = { 1.0, 0.0, 0.0, 0.0 };
    glClearColor (1.0, 1.0, 1.0, 1.0);
    // glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    // glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    // glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    // glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_AMBIENT, white_light);
    // glLightfv(GL_LIGHT1, GL_DIFFUSE, white_light);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);

    GLfloat light2_position[ ] = {viewer[0], viewer[1], viewer[2], 1.0};
    GLfloat light2_direction[ ] = {viewVector[0], viewVector[1], viewVector[2], 1.0};
    glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
    cerr << light2_position[0] << " " << light2_direction[0] << endl;
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light2_direction);
    glLighti(GL_LIGHT2, GL_SPOT_EXPONENT, 2);
    // glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 4.0);
    // glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.001);
    glLighti(GL_LIGHT2, GL_SPOT_CUTOFF, 30);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, red_light);

    //8 Texturas
    Image *image1 = loadTexture("Textures/Metal.bmp");
    Image *image2 = loadTexture("Textures/Madeira.bmp"); 
    Image *image3 = loadTexture("Textures/Areia.bmp"); 
    Image *image4 = loadTexture("Textures/Escamas.bmp"); 
    Image *image5 = loadTexture("Textures/Abacaxi.bmp"); 
    if(image1 == NULL || image2 == NULL || image3 == NULL || image4 == NULL){
        printf("Image was not returned from loadTexture\n");
        exit(0);
    }
    makeCheckImage();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Create Texture
    //Primeira Textura - Metal
    glGenTextures(8, texture);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image1->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
 
    //Segunda textura - Madeira
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image2->sizeX, image2->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image2->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
    
    //Terceira textura - Areia
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image3->sizeX, image3->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image3->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    //Quarta textura - Escamas
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image4->sizeX, image4->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image4->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    //Quinta textura - Abacaxi
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image5->sizeX, image5->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image5->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
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

    GLfloat red_light[ ] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light2_position[ ] = {viewer[0], viewer[1], viewer[2], 1.0};
    GLfloat light2_direction[ ] = {viewVector[0], viewVector[1], viewVector[2], 1.0};
    glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
    // cerr << light2_position[0] << " " << light2_direction[0] << endl;
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light2_direction);
    glLighti(GL_LIGHT2, GL_SPOT_EXPONENT, 2);
    // glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 4.0);
    // glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.001);
    glLighti(GL_LIGHT2, GL_SPOT_CUTOFF, 30);
    
    glLightfv(GL_LIGHT2, GL_DIFFUSE, red_light);

    glTranslated(center[0], center[1], center[2]);
	glRotatef(-1*Rot,0,1,0);  //fazer translacao pra rodar direito
    glRotatef(90,0,1,0);
    glTranslated(-1*center[0], -1*center[1], -1*center[2]);
    glTranslated(center[0], center[1], center[2]);
    
    Parser("models/submarino3.obj", 0,0); //0 = sem cor 0 = metal
    glPopMatrix();	
    
    glPushMatrix();
        glTranslatef(0,-63,0);
        glScalef(6,1,6);
    Parser("models/areia2.obj",7,2); //7 = amarelo; 2 = areia
    glPopMatrix();

    glPushMatrix();{
        glTranslatef(-6,-73.7,-3);
        glScalef(1.5,1.5,1.5);
    }
    Parser("models/tesouro2.obj",6,1); //6 = marrom; 1 = madeira
    glPopMatrix(); 
    
    glPushMatrix();{
        glTranslatef(12,0,0);
        glScalef(2,2,2);
        glTranslatef(0,animaNavio,0);
    }
    Parser("models/navio2.obj",6,1);  //6 = marrom; 1 = madeira
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
    Parser("models/cardumao2.obj",0,3); //3 = escamas
    glPopMatrix();
    
    glPushMatrix();{
        glTranslatef(28,-75,-3);
        glScalef(6,8,6);
    }
    Parser("models/abacaxi2.obj",3,4); //laranja - 4 = Abacaxi
    glPopMatrix();
    
    glPushMatrix();{
		glTranslatef(-15,0,-7);
        glScalef(2,2,2);
		glScalef(0.2,0.2,0.2);
        glTranslatef(0,animaNavio,0);
    }
    Parser("models/jetski2.obj",5,0); //cinza
    glPopMatrix();
    
//     // Funcao extra, nao finalizada
//   /*  if (torpedo) { 
//         glPushMatrix();{
//             double posTorpedo[3] = {center[0],center[1],center[2]};
//             //glTranslatef(0,PosY_torp,PosX_torp);
//             // for(int i=0;)
//             glTranslatef(animaTorpedo*viewVector[0], 0, animaTorpedo*viewVector[2]);
//         }
//         Parser("models/torpedo.obj",0); //cinza
//         glPopMatrix();
//     }
// 	*/
    
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
        case 'g': case 'G': ModoLuz=!ModoLuz; if(ModoLuz) glShadeModel(GL_FLAT); else glShadeModel(GL_SMOOTH); break;

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
    glEnable(GL_FLAT | GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND); //habilita o alpha nas superficies
    glutTimerFunc(100, Temporizador,0); //determina funcao de Timer
    glutMainLoop(); //loop
    return 0;
}