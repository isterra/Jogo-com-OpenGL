#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdbool.h>
//Declaraçao de variaveis globais
GLuint texturaBolinha;
float velocidade_bolinhaX;
float velocidade_bolinhaY;
int colidiu=0;
enum jogadores {NINGUEM,PLAYER1,PLAYER2};
#define N_RETANGULOS 2
bool pausar = false;
bool restart = false;
bool keyW = false;
bool keyS = false;
bool keyO = false;
bool keyL = false;

typedef struct {
    float posicaoX;
    float posicaoY;
    float largura;
    float altura;
} Sprite;
typedef struct{
  int pontuacao ;
}Jogador;


Sprite barrasLaterais[N_RETANGULOS];
Sprite bolinha;
Sprite pontuacao;
Sprite ESC;
Jogador player[2];


GLuint carregaTextura(const char* arquivo) {
    GLuint idTextura = SOIL_load_OGL_texture(
        arquivo,        // ⬅️ do parâmetro
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
    );

    if (idTextura == 0) {
        printf("Erro carregando a textura: '%s'\n", SOIL_last_result());
    }

    return idTextura;
}
void reinicia_round(){
  barrasLaterais[0].posicaoX = -90;
  barrasLaterais[0].posicaoY = 0;
  //Retângulo direito
  barrasLaterais[1].posicaoX = 90;
  barrasLaterais[1].posicaoY = 0;
  //Bolinha
  bolinha.posicaoX = 0;
  bolinha.posicaoY = 0;
  velocidade_bolinhaX = 1.5;
  velocidade_bolinhaY = -1.5;
}

void setJogadores(){
  player[0].pontuacao = 5;
  player[1].pontuacao = 5;
}
int venceu_jogo(){
  if(player[0].pontuacao==0)
    return PLAYER2;
  if(player[1].pontuacao==0)
    return PLAYER1;
  return NINGUEM;
}
void pontuacao_jogo(Jogador *jogador){

  jogador->pontuacao -=1;
  reinicia_round();

  if(venceu_jogo() == PLAYER1){
    printf("1 venceu\n" );
  }
  if(venceu_jogo() == PLAYER2){
    printf("2 venceu\n" );
  }
}

void limite_bolinha_pontuacao (Sprite* posicoessBolinha){

    if(posicoessBolinha->posicaoY + posicoessBolinha->largura > 100)
        velocidade_bolinhaY = -velocidade_bolinhaY;
    if(posicoessBolinha->posicaoX + posicoessBolinha->largura > 100){
        velocidade_bolinhaX = -velocidade_bolinhaX;
          pontuacao_jogo(&player[1]);
    }
    if (posicoessBolinha->posicaoY - posicoessBolinha->largura < -100)
        velocidade_bolinhaY = -velocidade_bolinhaY;
    if(posicoessBolinha->posicaoX - posicoessBolinha->largura < -100){
        velocidade_bolinhaX = -velocidade_bolinhaX;
        pontuacao_jogo(&player[0]);
    }
}
bool colisao_bolinha(Sprite* bola, Sprite *barra){

    if(bola->posicaoX - bola->largura/2 > barra->posicaoX + barra->largura/2)
        return false;

    if(bola->posicaoY - bola->largura/2 > barra->posicaoY + barra->altura/2 &&
        bola->posicaoX - bola->largura/2 < barra->posicaoX + barra->largura/2)
        return false;

    if(bola->posicaoX + bola-> largura/2 < barra->posicaoX - barra->largura/2)
        return false;

    if(bola->posicaoY + bola->altura/2 < barra->posicaoY - barra-> altura/2&&
        bola->posicaoX - bola->largura < barra->posicaoX + bola->largura)
        return false;

    return true;
}

int movimenta_bolinha(Sprite* posicoesBolinha) {

    if(colisao_bolinha(posicoesBolinha,&barrasLaterais[0])==true){
        velocidade_bolinhaX = -velocidade_bolinhaX;

    }
    if(colisao_bolinha(posicoesBolinha,&barrasLaterais[1])==true){
       velocidade_bolinhaX = -velocidade_bolinhaX ;

    }
    limite_bolinha_pontuacao(posicoesBolinha);

    posicoesBolinha->posicaoX += velocidade_bolinhaX;
    posicoesBolinha->posicaoY += velocidade_bolinhaY;

}

void inicializa_variaveis() {
    glClearColor(1, 1, 1, 1); //Branco

    //Retângulo esquerdo
    barrasLaterais[0].posicaoX = -90;
    barrasLaterais[0].posicaoY = 0;
    barrasLaterais[0].largura = 3;
    barrasLaterais[0].altura = 35;

    //Retângulo direito
    barrasLaterais[1].posicaoX = 90;
    barrasLaterais[1].posicaoY = 0;
    barrasLaterais[1].largura = 3;
    barrasLaterais[1].altura = 35;

    //Bolinha
    bolinha.posicaoX = 0;
    bolinha.posicaoY = 0;
    bolinha.largura = 5;
    bolinha.altura = 5;
    velocidade_bolinhaX = 1.5;
    velocidade_bolinhaY = -1.5;
    //Pontuaçao maxima dos jogadores
    player[0].pontuacao=5;
    player[1].pontuacao=5;

    glEnable(GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    texturaBolinha = carregaTextura("spriteSheetBalls.png");

}
void reiniciar(){

  if(restart == true){
    inicializa_variaveis();
    restart = !restart;
  }
}

void cria_bolinha(Sprite *barras){

    glEnable(GL_TEXTURE_2D);
    glColor3f(1,0,0);
    glBindTexture(GL_TEXTURE_2D, texturaBolinha);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    glPushMatrix();
        glTranslatef(barras->posicaoX,barras->posicaoY,0);
        glBegin(GL_TRIANGLE_FAN);

            glTexCoord2d(0, 0); glVertex3f(-barras->largura/2,-barras->altura/2, 0);
            glTexCoord2d(0.33, 0); glVertex3f( barras->largura/2,-barras->altura/2, 0);
            glTexCoord2d(0.33, 1); glVertex3f( barras->largura/2, barras->altura/2, 0);
            glTexCoord2d(0, 1); glVertex3f(-barras->largura/2, barras->altura/2, 0);

        glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();


}


void cria_barrasLaterais(Sprite *barras){

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(1,0,0);

    glPushMatrix();
        glTranslatef(barras->posicaoX,barras->posicaoY,0);
        glBegin(GL_TRIANGLE_FAN);

            glVertex2f(-barras->largura/2,-barras->altura/2);
            glVertex2f( barras->largura/2,-barras->altura/2);
            glVertex2f( barras->largura/2, barras->altura/2);
            glVertex2f(-barras->largura/2, barras->altura/2);

        glEnd();
    glPopMatrix();
}

void redimensiona(int width, int height) {

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-100, 100, -100, 100, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void movimento_das_barras(){
  if(pausar==false){

          if(keyW == true){
            if(barrasLaterais[0].posicaoY+barrasLaterais[0].altura/2<=97)
            barrasLaterais[0].posicaoY += 5;
          }

        if(keyS == true){
            if(barrasLaterais[0].posicaoY-barrasLaterais[0].altura/2>=-97)
            barrasLaterais[0].posicaoY -= 5;
        }

        if(keyO == true){
            if(barrasLaterais[1].posicaoY+barrasLaterais[1].altura/2<=97)
            barrasLaterais[1].posicaoY += 5;
        }

        if(keyL==true){
            if(barrasLaterais[1].posicaoY-barrasLaterais[1].altura/2>=-97)
            barrasLaterais[1].posicaoY -= 5;
        }


  }
}
void keyup(unsigned char key, int x, int y){
  switch (key) {
    case 'w':
      keyW = false;
      break;
    case 's':
      keyS = false;
      break;
    case 'o':
      keyO = false;
      break;
    case 'l':
      keyL = false;
      break;
    default:
      break;
  }
}
void teclado(unsigned char key, int x, int y) {

    switch (key) {
      case 'w':
        keyW = true;
        break;
      case 's':
        keyS = true;
        break;
      case 'o':
        keyO = true;
        break;
      case 'l':
        keyL = true;
        break;
      case 'p':
        pausar = !pausar;
        break;
      case 'r':
        restart = !restart;
        reiniciar();
        break;
      case 27:
          exit(0);
          break;
      default:
          break;
    }
}
void atualizaCena(int periodo) {
  if(pausar==false){
    movimenta_bolinha(&bolinha);
  }
  movimento_das_barras();
    glutPostRedisplay();
    glutTimerFunc(periodo, atualizaCena, periodo);
}

void desenhaCena() {
     glClear(GL_COLOR_BUFFER_BIT);

        for(int i = 0; i < N_RETANGULOS; i++) {
            cria_barrasLaterais(&barrasLaterais[i]);
        }
        cria_bolinha(&bolinha);

    glutSwapBuffers();
}

int main(int argc, char **argv) {

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1000,500);
    glutCreateWindow("PhingPhong");
    inicializa_variaveis();
    setJogadores();
    glutReshapeFunc(redimensiona);
    glutKeyboardUpFunc(keyup);
    glutKeyboardFunc(teclado);
    glutDisplayFunc(desenhaCena);
    glutTimerFunc(0, atualizaCena, 33);
    glutMainLoop();

    return 0;
}
