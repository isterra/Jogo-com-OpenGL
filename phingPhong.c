#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define N_RETANGULOS 2
#define N_JOGADORES 2

//Declaraçao de variaveis globais
GLuint texturaBolinha;
GLuint texturaFundo;
GLuint texturaBarras;
GLuint texturaConfirmacao;
GLuint texturaMenu;
GLuint texturaInstrucoes;
GLuint texturaSobre;
GLuint texturaBotaoSair;
GLuint texturaPlayer1;
GLuint texturaPlayer2;
GLuint texturaBotaoPausar;
GLuint texturaRestart;

float controleAumentoVelocidade = 0;

float velocidade_bolinhaX;
float velocidade_bolinhaY;

int v0xPlayer1 = -90;
int v0yPlayer1 = 105;
int v1xPlayer1 = -60;
int v1yPlayer1 = 105;
int v2xPlayer1 = -60;
int v2yPlayer1 = 125;
int v3xPlayer1 = -90;
int v3yPlayer1 = 125;

int v0xPlayer2 = 10;
int v0yPlayer2 = 105;
int v1xPlayer2 = 40;
int v1yPlayer2 = 105;
int v2xPlayer2 = 40;
int v2yPlayer2 = 125;
int v3xPlayer2 = 10;
int v3yPlayer2 = 125;

int xPontuacaoPlayer1 = -54;
int yPontuacaoPlayer1 = 110;
int xPontuacaoPlayer2 = 45;
int yPontuacaoPlayer2 = 110;

bool mostraDebug = false;
bool vaiAdois = false;
bool pausar = true;
bool keyW = false;
bool keyS = false;
bool keyO = false;
bool keyL = false;
bool aparecerESC = false;
bool colisaoBarraEsquerda = false;
bool colisaoBarraDireita = false;
bool invertePlayer = false;
bool comecar = false;
bool reinicia = false;

int controleTeclas = 1;

typedef struct {
  int pontuacao ;
  int set;
} Jogador;

typedef struct {
    float posicaoX;
    float posicaoY;
    float largura;
    float altura;
    Jogador jogadores;
} Sprite;

typedef struct {
  float posicaoX;
  float posicaoY;
  int clique;
  int botao;
} Mouse;

enum telasDeJogo {MENU, JOGO, INSTRUCOES};
enum jogadores {NINGUEM, PLAYER1, PLAYER2};
enum confirmacaoUsuario {NAO, SIM};
enum telas{TELAINICIAL, TELAJOGO};

int controleTelas = MENU;


Sprite barrasLaterais[N_RETANGULOS];
Sprite bolinha;
Sprite pontuacao;
Sprite esc;
Sprite reset;
Mouse mouse;
Jogador player[N_JOGADORES];

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
int velocidadeAleatoriaBolinhaY(){
  int velocidade=0;
  srand(time(0));
  velocidade= rand()%100;
  if(velocidade %2 == 0)
    return velocidade;
  return -velocidade;
}

void inicializa_variaveis() {
    glClearColor(1, 1, 1, 1); //Branco

    // Pontuaçao dos jogadores
    player[0].pontuacao = 0;
    player[1].pontuacao = 0;
    player[0].set = 0;
    player[1].set = 0;

    //Retângulo esquerdo
    barrasLaterais[0].posicaoX = -90;
    barrasLaterais[0].posicaoY = 0;
    barrasLaterais[0].largura = 10;
    barrasLaterais[0].altura = 50;
    barrasLaterais[0].jogadores = player[0];

    //Retângulo direito
    barrasLaterais[1].posicaoX = 90;
    barrasLaterais[1].posicaoY = 0;
    barrasLaterais[1].largura = 10;
    barrasLaterais[1].altura = 50;
    barrasLaterais[1].jogadores = player[1];

    //Bolinha
    bolinha.posicaoX = 0;
    bolinha.posicaoY = 0;
    bolinha.largura = 10;
    bolinha.altura = 20;
    velocidade_bolinhaX = 3.0;
    velocidade_bolinhaY = -1.5 ;

    //esc retangulo
    esc.posicaoX = 0;
    esc.posicaoY = 0;
    esc.largura = 50;
    esc.altura = 50;

    glEnable(GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    texturaBolinha = carregaTextura("bola.png");
    texturaFundo = carregaTextura("fundo.png");
    texturaBarras = carregaTextura("alien.png");
    texturaConfirmacao = carregaTextura("sair.png");
    texturaMenu = carregaTextura("mainmenu.png");
    texturaInstrucoes = carregaTextura("menu.png");
    texturaSobre = carregaTextura("sobre.png");
    texturaBotaoSair = carregaTextura("botaoSair.png");
    texturaPlayer1 = carregaTextura("players.png");
    texturaPlayer2 = carregaTextura("players.png");
    texturaBotaoPausar = carregaTextura("pause.png");
    texturaRestart = carregaTextura("restart.png");
}

void escreveTexto(void * font, char *s, float x, float y, float z) {
    int i;
    glRasterPos3f(x, y, z);

    for (i = 0; i < strlen(s); i++) {
        glutBitmapCharacter(font, s[i]);
    }
}

void desenhaPontuacaoPlayer1() {
    glColor3f(1.0, 0.3, 0.6);
    char textoPadrao[11] = "Pontuacao: ";
    char textoSaida[15];
    snprintf(textoSaida, 15, "%s %d", textoPadrao, player[0].pontuacao);
    escreveTexto(GLUT_BITMAP_HELVETICA_18, textoSaida , xPontuacaoPlayer1, yPontuacaoPlayer1, 0);
}

void desenhaPontuacaoPlayer2() {
    glColor3f(0.2, 0.4, 0.8);
    char textoPadrao[11] = "Pontuacao: ";
    char textoSaida[15];
    snprintf(textoSaida, 15, "%s %d", textoPadrao, player[1].pontuacao);
    escreveTexto(GLUT_BITMAP_HELVETICA_18, textoSaida , xPontuacaoPlayer2, yPontuacaoPlayer2, 0);
}

void trocaCoordenadasPontuacao() {
  int aux;

  //Troca as coordenadas de desenho no texto
  aux = xPontuacaoPlayer1;
  xPontuacaoPlayer1 = xPontuacaoPlayer2;
  xPontuacaoPlayer2 = aux;

  aux = yPontuacaoPlayer1;
  yPontuacaoPlayer1 = yPontuacaoPlayer2;
  yPontuacaoPlayer2 = aux;
}

void reinicia_round() {
  barrasLaterais[0].posicaoX = -90;
  barrasLaterais[0].posicaoY = 0;

  //Retângulo direito
  barrasLaterais[1].posicaoX = 90;
  barrasLaterais[1].posicaoY = 0;

  //Bolinha
  bolinha.posicaoX = 0;
  bolinha.posicaoY = 0;
  velocidade_bolinhaX = 3.0;
  velocidade_bolinhaY = -3.0;
  comecar = false;
}

void zeraPontos(){
  player[0].pontuacao = 0;
  player[1].pontuacao = 0;
}

int venceu_jogo() {
  if(player[0].set == 2)
    exit(0);

  if(player[1].set == 2)
    return PLAYER1;

  return NINGUEM;
}

void trocaCoordenadasPlayers() {
  int aux;

  //Troca das variáveis dos vértices
  aux = v0xPlayer1;
  v0xPlayer1 = v0xPlayer2;
  v0xPlayer2 = aux;

  aux = v0yPlayer1;
  v0yPlayer1 = v0yPlayer2;
  v0yPlayer2 = aux;

  aux = v1xPlayer1;
  v1xPlayer1 = v1xPlayer2;
  v1xPlayer2 = aux;

  aux = v1yPlayer1;
  v1yPlayer1 = v1yPlayer2;
  v1yPlayer2 = aux;

  aux = v2xPlayer1;
  v2xPlayer1 = v2xPlayer2;
  v2xPlayer2 = aux;

  aux = v2yPlayer1;
  v2yPlayer1 = v2yPlayer2;
  v2yPlayer2 = aux;

  aux = v3xPlayer1;
  v3xPlayer1 = v3xPlayer2;
  v3xPlayer2 = aux;

  aux = v3yPlayer1;
  v3yPlayer1 = v3yPlayer2;
  v3yPlayer2 = aux;
}

void setNormal() {
  if(vaiAdois == false) {

    if(player[0].pontuacao == 11){
      player[0].set++;
      zeraPontos();
      reinicia_round();
      invertePlayer = !invertePlayer;
      trocaCoordenadasPlayers();
      trocaCoordenadasPontuacao();
    }
    if(player[1].pontuacao == 11) {
      player[1].set++;
      zeraPontos();
      reinicia_round();
      invertePlayer = !invertePlayer;
      trocaCoordenadasPlayers();
      trocaCoordenadasPontuacao();
    }
  }
}

void adicionaPonto(){

  if(bolinha.posicaoX + bolinha.largura > 100 && !invertePlayer) {
    player[0].pontuacao += 1;
    controleAumentoVelocidade = 0;
    setNormal();
    printf("player 0 %i\n",player[0].pontuacao);
    printf("player 0 %i\n",player[0].set);
    reinicia_round();
  }

  else if(bolinha.posicaoX + bolinha.largura > 100 && invertePlayer) {
    player[1].pontuacao += 1;
    controleAumentoVelocidade = 0;
    setNormal();
    printf("player 0 %i\n",player[0].pontuacao);
    printf("player 0 %i\n",player[0].set);
    reinicia_round();
  }

  if(bolinha.posicaoX - bolinha.largura < -100 && !invertePlayer) {
    player[1].pontuacao+=1;
    setNormal();
    printf("player 1 %i\n",player[1].pontuacao);
    printf("player 1 %i\n",player[1].set);
    reinicia_round();
  }

  else if(bolinha.posicaoX - bolinha.largura < -100 && invertePlayer) {
    player[0].pontuacao+=1;
    setNormal();
    printf("player 1 %i\n",player[1].pontuacao);
    printf("player 1 %i\n",player[1].set);
    reinicia_round();
  }
}
void verificaSet(){
  if(((player[0].set == player[1].set )&&player[0].set!=0) && (player[0].pontuacao == 5 || player[1].pontuacao == 5)){
    invertePlayer = !invertePlayer;
  }
}

void vai_A_dois() {

  int diferenca = 2;
  if(abs(player[0].pontuacao - player[1].pontuacao) == diferenca){
    if(player[0].pontuacao > player[1].pontuacao) {
      player[0].set++;
      zeraPontos();
      reinicia_round();
      invertePlayer = !invertePlayer;
      vaiAdois = !vaiAdois;
    }
    else {
      player[1].set++;
      zeraPontos();
      reinicia_round();
      invertePlayer = !invertePlayer;
      vaiAdois = !vaiAdois;
    }
  }
  adicionaPonto();
}

void pontuacao_jogo() {

  verificaSet();

  if(player[0].pontuacao == 10 && player[1].pontuacao == 10)
    vaiAdois = !vaiAdois;

    if(vaiAdois == true)
        vai_A_dois();
      else
        adicionaPonto();

  venceu_jogo();
}
//******************************************************************************
//***************************** MOVIMENTOS DA BOLINHA **************************
void limite_bolinha_pontuacao (Sprite* posicoessBolinha) {
    if(posicoessBolinha->posicaoY + posicoessBolinha->largura > 100)
        velocidade_bolinhaY = -velocidade_bolinhaY;
    if(posicoessBolinha->posicaoX + posicoessBolinha->largura > 100){
        velocidade_bolinhaX = -velocidade_bolinhaX;
          pontuacao_jogo();
    }
    if (posicoessBolinha->posicaoY - posicoessBolinha->largura < -100)
        velocidade_bolinhaY = -velocidade_bolinhaY;
    if(posicoessBolinha->posicaoX - posicoessBolinha->largura < -100){
        velocidade_bolinhaX = -velocidade_bolinhaX;
        pontuacao_jogo();
    }
}


bool colisao_bolinha(Sprite* bola, Sprite* barra) {
    if(bola->posicaoX - bola->largura/2 > barra->posicaoX + barra->largura/2)
        return false;

    if(bola->posicaoY - bola->largura/2 > barra->posicaoY + barra->altura/2 &&
        bola->posicaoX - bola->largura/2 < barra->posicaoX + barra->largura/2)
        return false;

    if(bola->posicaoX + bola-> largura/2 < barra->posicaoX - barra->largura/2)
        return false;

    if(bola->posicaoY + bola->altura/2 < barra->posicaoY - barra-> altura/2 &&
        bola->posicaoX - bola->largura < barra->posicaoX + bola->largura)
        return false;

    controleAumentoVelocidade += 0.5;

    return true;
}

void aumenta_velocidade_bolinha() {

  colisaoBarraEsquerda = colisao_bolinha(&bolinha, barrasLaterais);
  colisaoBarraDireita = colisao_bolinha(&bolinha, &barrasLaterais[1]);

  if((colisaoBarraEsquerda || colisaoBarraDireita) && controleAumentoVelocidade >= 4) {
    if(velocidade_bolinhaX >= 5.0) {
      velocidade_bolinhaX = 5.0;
      velocidade_bolinhaY = 5.0;
    }
    else {
      velocidade_bolinhaX = 1.2 * velocidade_bolinhaX;
      velocidade_bolinhaY = 1.2 * velocidade_bolinhaY;
    }
  }
    bolinha.posicaoX += velocidade_bolinhaX;
    bolinha.posicaoY += velocidade_bolinhaY;
}

int movimenta_bolinha(Sprite* posicoesBolinha) {
    if(colisao_bolinha(posicoesBolinha,&barrasLaterais[0]) == true){
        velocidade_bolinhaX = -velocidade_bolinhaX;
    }
    if(colisao_bolinha(posicoesBolinha,&barrasLaterais[1]) == true){
       velocidade_bolinhaX = -velocidade_bolinhaX ;
    }

    limite_bolinha_pontuacao(posicoesBolinha);
    aumenta_velocidade_bolinha();
}

void reiniciar() {
    inicializa_variaveis();
}
//Converte as posicoes para qualquer tamanho de tela
int cpx(int x) {
  return (glutGet(GLUT_WINDOW_WIDTH) * x) / 1000;
}

int cpy(int y) {
  return (glutGet(GLUT_WINDOW_HEIGHT) * y) / 500;
}
//******************************************************************************
//************************ DETECÇAO MOVIMENTO MOUSE ****************************
void movimento_mouse(int button, int state, int x, int y) {
  mouse.posicaoX = x;
  mouse.posicaoY = y;
  mouse.clique = state;
  mouse.botao = button;

  switch (controleTelas) {
    case MENU:
      if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
          if(x >= cpx(440) && x <= cpx(560) && y >= cpy(290)  && y <= cpy(340)) {
            controleTelas = JOGO;
            pausar = false;
            controleTeclas = false;
          }
          else if(x >= cpx(415) && x <= cpx(600) && y >= cpy(380)  && y <= cpy(430)) controleTelas = INSTRUCOES;
          else if(x >= cpx(820) && x <= cpx(980) && y >= cpy(410)  && y <= cpy(450)) exit(0);
      }
    break;

    case JOGO:
      if(aparecerESC) {
        if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
          if(x >= cpx(526) && x <= cpx(626) && y >= cpy(313)  && y <= cpy(338)) exit(0);
          else if(x >= cpx(377) && x <= cpx(526) && y >= cpy(313)  && y <= cpy(338)) {
            pausar = false;
            aparecerESC = false;
          }
        }
      }

      else if(reinicia) {
        if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
          if(x >= cpx(526) && x <= cpx(626) && y >= cpy(313)  && y <= cpy(338)) reiniciar();
          else if(x >= cpx(377) && x <= cpx(526) && y >= cpy(313)  && y <= cpy(338)) {
            pausar = false;
            reinicia = false;
          }
        }
      }
    break;

    case INSTRUCOES:
      if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
          if(x >= cpx(820) && x <= cpx(980) && y >= cpy(410)  && y <= cpy(450)) controleTelas = MENU;
      }
    break;


  default:
    break;
  }


}
//******************************************************************************
//*************************** FUNCOES DE CONTROLE ******************************
void confirmaESC() {
  pausar = true;
  aparecerESC = true;
}

void confirmaReiniciar() {
  reinicia = true;
}

//******************************************************************************
//************************** CRIA VERTEX ***************************************
void cria_retanguloESC() {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texturaConfirmacao);
  glColor3f(1,1,1);
  glPushMatrix();
        glTranslatef(esc.posicaoX,esc.posicaoY,0);
        glBegin(GL_TRIANGLE_FAN);

          glTexCoord2f(0.1, 0.40); glVertex3f(-esc.largura/2,-esc.altura/2, 0);
          glTexCoord2f(0.9, 0.40); glVertex3f(esc.largura/2,-esc.altura/2, 0);
          glTexCoord2f(0.9, 0.8); glVertex3f(esc.largura/2,esc.altura/2, 0);
          glTexCoord2f(0.1, 0.8); glVertex3f(-esc.largura/2,esc.altura/2, 0);

        glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void cria_bolinha(Sprite* bolinha) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaBolinha);
    glColor3f(1,1,1);

    glPushMatrix();
        glTranslatef(bolinha->posicaoX,bolinha->posicaoY,0);
        glBegin(GL_TRIANGLE_FAN);

            glTexCoord2f(0, 0); glVertex2f(-bolinha->largura/2,-bolinha->altura/2);
            glTexCoord2f(0.2, 0); glVertex2f( bolinha->largura/2,-bolinha->altura/2);
            glTexCoord2f(0.2, 0.23); glVertex2f( bolinha->largura/2, bolinha->altura/2);
            glTexCoord2f(0.23, 0); glVertex2f(-bolinha->largura/2, bolinha->altura/2);

        glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void criaFundo() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaFundo);
    glColor3f(1,1,1);

    glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0, 0); glVertex2f(-100, -100);
        glTexCoord2f(1, 0); glVertex2f(100, -100);
        glTexCoord2f(1, 1); glVertex2f(100, 150);
        glTexCoord2f(0, 1); glVertex2f(-100, 150);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void desenhaMenuJogo() {

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texturaMenu);
  glColor3f(1, 1, 1);

  glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0, 0.2); glVertex2f(-100, -100);
    glTexCoord2f(1, 0.2); glVertex2f(100, -100);
    glTexCoord2f(1, 1); glVertex2f(100, 150);
    glTexCoord2f(0, 1); glVertex2f(-100, 150);
    glEnd();
  glDisable(GL_TEXTURE_2D);
}

void desenhaInstrucoes() {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texturaInstrucoes);
  glColor3f(1, 1, 1);

  glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0, 0.2); glVertex2f(-100, -100);
        glTexCoord2f(1, 0.2); glVertex2f(100, -100);
        glTexCoord2f(1, 1); glVertex2f(100, 150);
        glTexCoord2f(0, 1); glVertex2f(-100, 150);
    glEnd();
  glDisable(GL_TEXTURE_2D);
}

void desenhaTextoInstrucoes() {
    glColor3f(0.5, 1, 0.5);
    char primeiraLinha[100] = "Obrigado por entrar em nosso jogo! Primeiramente, vamos definir os controles: ";
    char segundaLinha[150] = "         As teclas W e S controlam a barra do Player 1 e as teclas O e L controlam a barra do Player 2.";
    char terceiraLinha[150] = "Quando algum jogador atingir 11 pontos, o lado sera trocado. Os controles permanecerao os mesmos.";
    char quartaLinha[100] = "Desenvolvedores: Israel Terra e Joao Paulo Souza";
    char quintaLinha[100] = "Trabalho de Computacao Grafica. Docente: Flavio Coutinho";
    escreveTexto(GLUT_BITMAP_HELVETICA_18, primeiraLinha , -80, 60, 0);
    escreveTexto(GLUT_BITMAP_HELVETICA_18, segundaLinha , -80, 50, 0);
    escreveTexto(GLUT_BITMAP_HELVETICA_18, terceiraLinha , -80, 30, 0);

    glColor3f(1, 0, 0);
      escreveTexto(GLUT_BITMAP_HELVETICA_18, quartaLinha , -80, -50, 0);
      escreveTexto(GLUT_BITMAP_HELVETICA_18, quintaLinha , -80, -70, 0);
}


void desenhaSobre() {

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texturaSobre);
  glColor3f(1, 1, 1);

  glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0, 0); glVertex2f(-100, -100);
        glTexCoord2f(1, 0); glVertex2f(100, -100);
        glTexCoord2f(1, 1); glVertex2f(100, 100);
        glTexCoord2f(0, 1); glVertex2f(-100, 100);
    glEnd();
  glDisable(GL_TEXTURE_2D);
}

void desenhaBotaoSair() {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texturaBotaoSair);
  glColor3f(1, 1, 1);

  glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0, 0); glVertex2f(65, -80);
        glTexCoord2f(1, 0); glVertex2f(95, -80);
        glTexCoord2f(1, 1); glVertex2f(95, -60);
        glTexCoord2f(0, 1); glVertex2f(65, -60);
    glEnd();
  glDisable(GL_TEXTURE_2D);
}

void desenhaBotaoPausar() {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texturaBotaoPausar);
  glColor3f(1, 1, 1);

  glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0, 0); glVertex2f(-10, 10);
        glTexCoord2f(1, 0); glVertex2f(10, 10);
        glTexCoord2f(1, 1); glVertex2f(10, 30);
        glTexCoord2f(0, 1); glVertex2f(-10, 30);
    glEnd();
  glDisable(GL_TEXTURE_2D);
}

void desenhaNomePlayer1() {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texturaPlayer1);
  glColor3f(1, 1, 1);

  glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0, 0.45); glVertex2f(v0xPlayer1, v0yPlayer1);
        glTexCoord2f(1, 0.45); glVertex2f(v1xPlayer1, v1yPlayer1);
        glTexCoord2f(1, 0.75); glVertex2f(v2xPlayer1, v2yPlayer1);
        glTexCoord2f(0, 0.75); glVertex2f(v3xPlayer1, v3yPlayer1);
    glEnd();
  glDisable(GL_TEXTURE_2D);
}

void desenhaNomePlayer2() {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texturaPlayer2);
  glColor3f(1, 1, 1);

  glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0, 0.13); glVertex2f(v0xPlayer2, v0yPlayer2);
        glTexCoord2f(1, 0.13); glVertex2f(v1xPlayer2, v1yPlayer2);
        glTexCoord2f(1, 0.45); glVertex2f(v2xPlayer2, v2yPlayer2);
        glTexCoord2f(0, 0.45); glVertex2f(v3xPlayer2, v3yPlayer2);
    glEnd();
  glDisable(GL_TEXTURE_2D);
}

void desenhaConfirmacaoRestart() {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texturaRestart);
  glColor3f(1, 1, 1);

  glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0.4, 0.43); glVertex2f(-20, -20);
        glTexCoord2f(0.6, 0.43); glVertex2f(20, -20);
        glTexCoord2f(0.6, 0.55); glVertex2f(20, 20);
        glTexCoord2f(0.4, 0.55); glVertex2f(-20, 20);
    glEnd();
  glDisable(GL_TEXTURE_2D);
}


void cria_barrasLaterais(Sprite *barras) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaBarras);
    glColor3f(1,1,1);

    glPushMatrix();
        glTranslatef(barras->posicaoX,barras->posicaoY,0);
        glBegin(GL_TRIANGLE_FAN);

            glTexCoord2f(0, 0); glVertex2f(-barras->largura/2,-barras->altura/2);
            glTexCoord2f(1, 0); glVertex2f( barras->largura/2,-barras->altura/2);
            glTexCoord2f(1, 1); glVertex2f( barras->largura/2, barras->altura/2);
            glTexCoord2f(0, 1); glVertex2f(-barras->largura/2, barras->altura/2);

        glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}
void redimensiona(int width, int height) {

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-100, 100, -100, 130, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}
//******************************************************************************
//***************************** DETECÇAO DAS TECLAS ****************************
void movimento_das_barras() {

  if(pausar == false){
    if(invertePlayer == false){
          if(keyW == true){
            if(barrasLaterais[0].posicaoY+barrasLaterais[0].altura/2<=100)
            barrasLaterais[0].posicaoY += 7;
            comecar = true;
          }

        if(keyS == true){
            if(barrasLaterais[0].posicaoY-barrasLaterais[0].altura/2>=-100)
            barrasLaterais[0].posicaoY -= 7;
            comecar = true;
        }

        if(keyO == true){
            if(barrasLaterais[1].posicaoY+barrasLaterais[1].altura/2<=100)
            barrasLaterais[1].posicaoY += 7;
            comecar = true;
        }

        if(keyL==true){
            if(barrasLaterais[1].posicaoY-barrasLaterais[1].altura/2>=-100)
            barrasLaterais[1].posicaoY -= 7;
            comecar = true;
        }
    }
    else{
        if(keyW == true){
          if(barrasLaterais[1].posicaoY+barrasLaterais[1].altura/2<=100)
          barrasLaterais[1].posicaoY += 5;
          comecar = true;
        }

      if(keyS == true){
          if(barrasLaterais[1].posicaoY-barrasLaterais[1].altura/2>=-100)
          barrasLaterais[1].posicaoY -= 5;
          comecar = true;
      }

      if(keyO == true){
          if(barrasLaterais[0].posicaoY+barrasLaterais[0].altura/2<=100)
          barrasLaterais[0].posicaoY += 5;
          comecar = true;
      }

      if(keyL==true){
          if(barrasLaterais[0].posicaoY-barrasLaterais[0].altura/2>=-100)
          barrasLaterais[0].posicaoY -= 5;
          comecar = true;
      }
    }
  }
}

void teclado(unsigned char key, int x, int y) {
  switch (controleTeclas) {

    case 1:
      break;

    case 0:
      switch (key) {

        case 27:
          confirmaESC();
        break;

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
          confirmaReiniciar();
        break;

        case 'y':
          exit(0);
        break;

        default:
            break;
    }

    default:
      break;
    }
}

void keyup(unsigned char key, int x, int y) {
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

void atualizaCena(int periodo) {

  if(pausar == false) {
    if(comecar == true){
      movimenta_bolinha(&bolinha);
    }
  }
    movimento_das_barras();
    glutPostRedisplay();
    glutTimerFunc(periodo, atualizaCena, periodo);
}

void desenhaCena() {
     glClear(GL_COLOR_BUFFER_BIT);
        switch (controleTelas) {
          case MENU:
            desenhaMenuJogo();
            desenhaBotaoSair();
          break;

          case JOGO:
            criaFundo();
              for(int i = 0; i < N_RETANGULOS; i++) {
                cria_barrasLaterais(&barrasLaterais[i]);
              }
              desenhaNomePlayer1();
              desenhaNomePlayer2();
              desenhaPontuacaoPlayer1();
              desenhaPontuacaoPlayer2();
              cria_bolinha(&bolinha);
              if(aparecerESC)
                cria_retanguloESC();
              if(pausar && !aparecerESC)
                desenhaBotaoPausar();
              if(reinicia)
                desenhaConfirmacaoRestart();
            break;

          case INSTRUCOES:
            desenhaInstrucoes();
            desenhaBotaoSair();
            desenhaTextoInstrucoes();
          break;

          default:
            break;
        }

    glutSwapBuffers();
}
void posicoesMouse(int x,int y){

  printf("largura %i altura %i x %i y %i\n",glutGet(GLUT_WINDOW_WIDTH),glutGet(GLUT_WINDOW_HEIGHT),x,y);
}


int main(int argc, char **argv) {

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1000,500);
    glutCreateWindow("PhingPhong");
    inicializa_variaveis();
    glutReshapeFunc(redimensiona);
    glutKeyboardUpFunc(keyup);
    glutKeyboardFunc(teclado);
    glutPassiveMotionFunc(posicoesMouse);
    glutMouseFunc(movimento_mouse);
    glutDisplayFunc(desenhaCena);
    glutTimerFunc(0, atualizaCena, 33);
    glutMainLoop();

    return 0;
}