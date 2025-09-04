#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <stdio.h>


#define MAX(a,b) (a>b)?a:b
#define MIN(a,b) (a<b)?a:b
#define MOD(a) (a < 0)?(-a):a

enum Movimento {
	PARADO,
	TRANSV,
	REV,
	ESQ_TRANSV,
	DIR_TRANSV,
	ESQ_REV,
	DIR_REV,
	ESQ,
	DIR	
};

struct cor {
	int r;
	int g;
	int b;
	int a;
};

struct ponto {
	double x;
	double y;
};

struct poligono {
	double angulo;
	cor borda;
	cor dentro;
	ponto centro;
	int n;
	ponto * vertices;
};

struct modelo {
	double angulo;
	ponto centro;
	int n;
	poligono * partes;
};

double positivar(double n, double limite) {
	if (n >= limite)
		return 0;
	else if (n < 0)
		return limite;
	else
		return n;
}
double graus(double radianos) {
	return radianos*180/M_PI;
}

double radianos(double angulo) {
	return angulo*M_PI/180;
}

ponto somar(ponto p1, ponto p2) {
	ponto soma = {p1.x+p2.x, p1.y+p2.y}; 
	return soma;
}

ponto escalar(ponto p1, double escala) {
	ponto produto = {p1.x*escala,p1.y*escala};
	return produto;
}

ponto mover(double velocidade, double angulo) {
	ponto newp = {velocidade*cos(radianos(angulo)), velocidade*sin(radianos(angulo))};
	return newp;
}

ponto rotacionar(ponto o1, ponto p1, double angulo) {
	ponto newp = {(p1.x-o1.x)*cos(radianos(angulo)) - (p1.y-o1.y)*sin(radianos(angulo)) + o1.x,
	              (p1.x-o1.x)*sin(radianos(angulo)) + (p1.y-o1.y)*cos(radianos(angulo)) + o1.y};
	return newp;
}

void desenhar(SDL_Renderer * ren, poligono poli, ponto centro_absoluto, double escala) {
	int i;
	cor dentro = poli.dentro, borda = poli.borda;
	Sint16 vx[poli.n], vy[poli.n];
	for (i = 0; i < poli.n; i++) {
		ponto ponto_absoluto = rotacionar(centro_absoluto, somar(escalar(poli.vertices[i],escala),somar(centro_absoluto,escalar(poli.centro,escala))), poli.angulo);
		vx[i] = ponto_absoluto.x;
		vy[i] = ponto_absoluto.y;
	}
	filledPolygonRGBA(ren,vx,vy,poli.n,dentro.r,dentro.g,dentro.b,dentro.a);
	polygonRGBA(ren,vx,vy,poli.n,borda.r,borda.g,borda.b,borda.a);
}

void renderizarModelo(SDL_Renderer * ren, modelo objeto, ponto centro_absoluto, double escala) {
	int i;
	for (i = 0; i < objeto.n; i++) {
		desenhar(ren, objeto.partes[i],centro_absoluto,escala);
	}
}

void reticula(SDL_Renderer * ren, int x, int y, int a) {
	circleRGBA(ren,x,y,15,255,0,0,a);
	lineRGBA(ren, x+10,y,x+20,y,255,0,0,a);
	lineRGBA(ren, x-10,y,x-20,y,255,0,0,a);
	lineRGBA(ren, x,y+10,x,y+20,255,0,0,a);
	lineRGBA(ren, x,y-10,x,y-20,255,0,0,a);
	filledCircleRGBA(ren,x,y,2,255,0,0,a);
}

int main(int argc, char* args[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window * win = SDL_CreateWindow("Cubo3D", 0, 0, 0, 0, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | 0x00001000);
	SDL_Renderer * ren = SDL_CreateRenderer(win, -1, 0);
	SDL_Event evt;
	SDL_ShowCursor(SDL_DISABLE);
	
	double angulo = 0, 
	  	   angulo_arma = 0, 
		   angulo_alvo = 0, 
		   posicao_lagarta_esq = 0,
		   posicao_lagarta_dir = 0;
	int WIDTH, HEIGHT;
	SDL_GetWindowSize(win, &WIDTH,&HEIGHT);
	
	cor preto0 = {  0,  0,  0,255},
		cinza0 = { 30, 30, 30,255},
		verde2 = { 80, 72, 37,255},
		verde1 = { 60, 55, 28,255},
		verde0 = { 50, 46, 23,255};
		
	ponto lagarta11 = {-94, -52},
		  lagarta12 = {+101, -52},
		  lagarta13 = {+101, -31},
		  lagarta14 = {-94, -31},
		  lagarta21 = {-94, +31},
		  lagarta22 = {+101, +31},
		  lagarta23 = {+101, +52},
		  lagarta24 = {-94, +52},
		  chassi1 = {-90, -50},
		  chassi2 = {+90, -50},
		  chassi3 = {+90, +50},
		  chassi4 = {-90, +50},
		  chassi5 = {-67, -32},
		  chassi6 = {+53, -32},
		  chassi7 = {+53, +32},
		  chassi8 = {-67, +32},
		  centro_chassi = {+0, +0},
		  torre1 = {-18, -30},
		  torre2 = {+15, -20},
 	   	  torre3 = {+15, +20},
		  torre4 = {-18, +30},
		  torre5 = {-53, +15},
		  torre6 = {-53, -15},
		  canhao1 = {+15, -3},
		  canhao2 = {+87, -3},
		  canhao3 = {+87, +3},
		  canhao4 = {+15, +3},
		  centro_torre = {+18, +0},
		  centro_tanque = {WIDTH/2, HEIGHT/2},
		  local = {WIDTH/2, HEIGHT/2};
	
	ponto retangulo1[] = {chassi1,chassi2,chassi3,chassi4},
		  retangulo2[] = {chassi5,chassi6,chassi7,chassi8},
		  retangulo3[] = {canhao1,canhao2,canhao3,canhao4},
		  retangulo4[] = {lagarta11,lagarta12,lagarta13,lagarta14},
		  retangulo5[] = {lagarta21,lagarta22,lagarta23,lagarta24},
		  trapezio1[] = {chassi1,chassi5,chassi8,chassi4},
		  trapezio2[] = {chassi6,chassi2,chassi3,chassi7},
		  hexagono1[] = {torre1,torre2,torre3,torre4,torre5,torre6};
	
	poligono lagarta1 = {0,preto0,cinza0,centro_chassi,4,retangulo4},
			 lagarta2 = {0,preto0,cinza0,centro_chassi,4,retangulo5},
			 blindagem = {0,preto0,verde0,centro_chassi,4,retangulo1},
			 teto = {0,preto0,verde1,centro_chassi,4,retangulo2},
			 peito = {0,preto0,verde0,centro_chassi,4,trapezio2},
			 tras = {0,preto0,verde0,centro_chassi,4,trapezio1},
			 canhao = {angulo_arma,preto0,verde1,centro_torre,4,retangulo3},
			 torre = {angulo_arma,preto0,verde2,centro_torre,6,hexagono1};
	
	poligono lagartas[] = {lagarta1,lagarta2},
			 partes_do_chassi[] = {blindagem,teto,peito,tras},
			 partes_da_torre[] = {canhao,torre};
	
	modelo tanque_lagarta = {angulo,centro_chassi,2,lagartas},
		   tanque_chassi = {angulo,centro_chassi,4,partes_do_chassi},
		   tanque_torre = {angulo,centro_torre,2,partes_da_torre};

	
	const Uint8 * tecP = SDL_GetKeyboardState(NULL);
	double velocidade = 0,
		   velocidade_angular = 0.9,
		   velocidade_torre = 0.5,
		   zoom = 1;
		   
	//informações dos paineis
	char x[30],y[30],grau[30],velocimetro[30],numero[30],state[30]={'E','s','t','a','d','o',':',' ','P','A','R','A','D','O','\0'},angulotorre[30],anguloalvo[30];
	
	enum Movimento estado = PARADO;
	int gamerunning = 1, mx=0, my=0;
	while (gamerunning) {
		//fundo verde
		SDL_SetRenderDrawColor(ren,0,100,0,255);
		SDL_RenderClear(ren);
		
		//grade chao
		int m,n;
		for (m = -100; m < WIDTH+100; m++) {
			if ((int)(m+local.x-WIDTH/2)%(int)(100*zoom)==0)
				lineRGBA(ren, m,-100,m,HEIGHT+100,0,50,0,255);
		}
		for (n = -100; n < HEIGHT+100; n++) {
			if ((int)(n+local.y-HEIGHT/2)%(int)(100*zoom)==0)
				lineRGBA(ren, -100,n,WIDTH+100,n,0,50,0,255);			
		}
		
		//lagartas
		renderizarModelo(ren,tanque_lagarta,rotacionar(centro_tanque,somar(escalar(centro_chassi,zoom),centro_tanque),angulo), zoom);
		
		//animacao das lagartas
		double aux;
		posicao_lagarta_esq = positivar(posicao_lagarta_esq, 195*zoom);
		posicao_lagarta_dir = positivar(posicao_lagarta_dir, 195*zoom);
		ponto ponto1,ponto2,ponto3,ponto4;
		//animacao da lagarta esquerda
		for (aux = posicao_lagarta_esq; aux < posicao_lagarta_esq+195*zoom; aux+=3*zoom) {
			ponto1 = rotacionar(centro_tanque,somar(centro_tanque,{lagarta11.x*zoom+(int)aux%(int)(195*zoom),lagarta11.y*zoom}),angulo);
			ponto2 = rotacionar(centro_tanque,somar(centro_tanque,{lagarta11.x*zoom+(int)aux%(int)(195*zoom),lagarta13.y*zoom}),angulo);
			lineRGBA(ren,ponto1.x,ponto1.y,ponto2.x,ponto2.y,0,0,0,255);
			if ((int)(aux/(3*zoom))%3 == 2) {
				ponto3 = rotacionar(centro_tanque,somar(centro_tanque,{lagarta11.x*zoom+(int)(aux+3*zoom)%(int)(195*zoom),lagarta11.y*zoom}),angulo);
				ponto4 = rotacionar(centro_tanque,somar(centro_tanque,{lagarta11.x*zoom+(int)(aux+3*zoom)%(int)(195*zoom),lagarta13.y*zoom}),angulo);
				double a = ponto3.x-ponto1.x, b = ponto3.y-ponto1.y;
				if (sqrt(a*a+b*b) < 10*zoom) {
					Sint16 vx[]={ponto1.x,ponto3.x,ponto4.x,ponto2.x},vy[]={ponto1.y,ponto3.y,ponto4.y,ponto2.y};
					filledPolygonRGBA(ren,vx,vy,4,10,10,10,255);
				}
			}
		}
		//animacao da lagarta direita
		for (aux = posicao_lagarta_dir; aux < posicao_lagarta_dir+195*zoom; aux+=3*zoom) {
			ponto1 = rotacionar(centro_tanque,somar(centro_tanque,{lagarta11.x*zoom+(int)aux%(int)(195*zoom),lagarta21.y*zoom}),angulo);
			ponto2 = rotacionar(centro_tanque,somar(centro_tanque,{lagarta11.x*zoom+(int)aux%(int)(195*zoom),lagarta23.y*zoom}),angulo);
			lineRGBA(ren,ponto1.x,ponto1.y,ponto2.x,ponto2.y,0,0,0,255);
			if ((int)(aux/(3*zoom))%3 == 2) {
				ponto3 = rotacionar(centro_tanque,somar(centro_tanque,{lagarta11.x*zoom+(int)(aux+3*zoom)%(int)(195*zoom),lagarta21.y*zoom}),angulo);
				ponto4 = rotacionar(centro_tanque,somar(centro_tanque,{lagarta11.x*zoom+(int)(aux+3*zoom)%(int)(195*zoom),lagarta23.y*zoom}),angulo);
				double a = ponto3.x-ponto1.x, b = ponto3.y-ponto1.y;
				if (sqrt(a*a+b*b) < 10*zoom) {
					Sint16 vx[]={ponto1.x,ponto3.x,ponto4.x,ponto2.x},vy[]={ponto1.y,ponto3.y,ponto4.y,ponto2.y};
					filledPolygonRGBA(ren,vx,vy,4,10,10,10,255);
				}
			}
		}
		
		//centro da torre
		ponto ctorre = rotacionar(centro_tanque,somar(escalar(centro_torre,zoom),centro_tanque),angulo);
		
		//ponto onde o canhao realmente esta mirando
		ponto mira_real = rotacionar(ctorre,{mx,my},angulo_arma-angulo_alvo);
		
		//chassi do tanque
		renderizarModelo(ren, tanque_chassi, rotacionar(centro_tanque,somar(escalar(centro_chassi,zoom),centro_tanque),angulo), zoom);
		
		//laser desejado/laser real
		//lineRGBA(ren,ctorre.x,ctorre.y,mx-15*cos(radianos(angulo_alvo)),my-15*sin(radianos(angulo_alvo)),255,255,255,100);
		lineRGBA(ren,ctorre.x,ctorre.y,mira_real.x-16*cos(radianos(angulo_arma)),mira_real.y-16*sin(radianos(angulo_arma)),255,255,255,50);
		
		//torre do tanque
		renderizarModelo(ren,tanque_torre,ctorre,zoom);
		
		//reticula desejada/reticula real
		reticula(ren,mx,my,255);
		reticula(ren,mira_real.x,mira_real.y,127);

		//painel de controle do chassi
		sprintf(x, "x: %4.1lf", local.x-WIDTH/2);
		sprintf(y, "y: %4.1lf", -(local.y-HEIGHT/2));
		sprintf(grau, "Angulo: %3.2lf", angulo);
		sprintf(velocimetro, "Velocidade: ");
		sprintf(numero, "%2.1lf", velocidade*1.2/10);
		boxRGBA(ren,0,0,169,57,0,0,0,255);
		stringRGBA(ren, 3, 3, x, 255,255,255,255);
		stringRGBA(ren, 3, 14, y, 255,255,255,255);
		stringRGBA(ren, 3, 25, grau, 255,255,255,255);
		stringRGBA(ren, 3, 36, velocimetro, 255,255,255,255);
		stringRGBA(ren, 100, 36, numero, 255,250-0.50*(MOD(velocidade)),250-0.50*(MOD(velocidade)),255);
		stringRGBA(ren, 3, 47, state, 255,255,255,255);
		rectangleRGBA(ren, 0,0,170,58,255,255,255,255);
		//painel de controle da torre
		sprintf(angulotorre, "angulo da torre: %4.1lf", angulo_arma);
		sprintf(anguloalvo, "angulo alvo: %4.1lf", angulo_alvo);
		boxRGBA(ren,0,HEIGHT-25,179,HEIGHT,0,0,0,255);
		stringRGBA(ren, 3, HEIGHT-23, angulotorre, 255,255,255,255);
		stringRGBA(ren, 3, HEIGHT-12, anguloalvo, 255,255,255,255);
		rectangleRGBA(ren, 0,HEIGHT-26,180,HEIGHT,255,255,255,255);
		
		//nao exige explicações
		SDL_RenderPresent(ren);
		
		//coleta os inputs e atualiza o estado de movimentação do tanque
		while (SDL_PollEvent(&evt)){	
	        if (evt.type == SDL_KEYDOWN || evt.type == SDL_KEYUP) {
	        	if (tecP[SDL_SCANCODE_W] && !tecP[SDL_SCANCODE_S]) {
	        		if (tecP[SDL_SCANCODE_A]) {
	        			estado = ESQ_TRANSV;
	        			sprintf(state, "Estado: ESQ_TRANSV");
					} else if (tecP[SDL_SCANCODE_D]) {
	        			estado = DIR_TRANSV;
	        			sprintf(state, "Estado: DIR_TRANSV");
					} else {
	        			estado = TRANSV;
	        			sprintf(state, "Estado: TRANSV");
	        		}
	        			
	        	} else if (tecP[SDL_SCANCODE_S]  && !tecP[SDL_SCANCODE_W]) {
					if (tecP[SDL_SCANCODE_A]) {
	        			estado = ESQ_REV;
	        			sprintf(state, "Estado: ESQ_REV");
					} else if (tecP[SDL_SCANCODE_D]) {
	        			estado = DIR_REV;
	        			sprintf(state, "Estado: DIR_REV");
					} else {
	        			estado = REV;
	        			sprintf(state, "Estado: REV");
	        		}
	        			
				} else if (tecP[SDL_SCANCODE_A] && !tecP[SDL_SCANCODE_D]) {
					estado = ESQ;
					sprintf(state, "Estado: ESQ");
					
					
				} else if (tecP[SDL_SCANCODE_D] && !tecP[SDL_SCANCODE_A]) {
					estado = DIR;
					sprintf(state, "Estado: DIR");
					
					
				} else {
					estado = PARADO;
					sprintf(state, "Estado: PARADO");
				} 
				
				
				if (tecP[SDL_SCANCODE_KP_PLUS] && zoom < 2.0) {
					zoom += 0.05;
				} else if (tecP[SDL_SCANCODE_KP_MINUS] && zoom > 0.1) {
					zoom -= 0.05;
				}
	        } else if (evt.type == SDL_QUIT) {
				gamerunning = 0;
			} else if (evt.type == SDL_MOUSEMOTION) {
				SDL_GetMouseState(&mx,&my);
			}
    	}
    	
    	//atualiza a velocidade e direção do movimento do tanque com base no estado
		if (estado == PARADO) {
			;
		} else if (estado == TRANSV) {
			if (velocidade < 414)
				velocidade += 27;
			else
				velocidade = 441;
		} else if (estado == ESQ_TRANSV && velocidade <= 441) {
			if (velocidade < 414)
				velocidade += 27;
			else
				velocidade = 441;
        	angulo -= velocidade_angular;
        	angulo_arma -= velocidade_angular;
		} else if (estado == DIR_TRANSV && velocidade <= 441) {
			if (velocidade < 414)
				velocidade += 27;
			else
				velocidade = 441;
			angulo += velocidade_angular;
			angulo_arma += velocidade_angular;
    	} else if (estado == REV && velocidade >= -88) {
    		if (velocidade > -73)
				velocidade -= 15;
			else
				velocidade = -88;
		} else if (estado == ESQ_REV && velocidade >= -88) {
			if (velocidade > -73)
				velocidade -= 15;
			else
				velocidade = -88;
       		angulo += velocidade_angular;
       		angulo_arma += velocidade_angular;
		} else if (estado == DIR_REV && velocidade >= -88) {
			if (velocidade > -73)
				velocidade -= 15;
			else
				velocidade = -88;
			angulo -= velocidade_angular;
			angulo_arma -= velocidade_angular;
		} else if (estado == ESQ) {
			angulo -= velocidade_angular;
			angulo_arma -= velocidade_angular;
			posicao_lagarta_esq -= velocidade_angular*zoom;
			posicao_lagarta_dir += velocidade_angular*zoom;
		} else if (estado == DIR) {
			angulo += velocidade_angular;
			angulo_arma += velocidade_angular;
			posicao_lagarta_esq += velocidade_angular*zoom;
			posicao_lagarta_dir -= velocidade_angular*zoom;
		}
		
		
		SDL_Delay(25);
		
		//encontra o angulo da mira
		int k;
		if (mx-WIDTH/2 != 0) {
			angulo_alvo = graus(atan(((double)my-ctorre.y)/((double)mx-ctorre.x)));
			if (mx <= ctorre.x)
				angulo_alvo += 180;
		}
		angulo_alvo = (angulo_alvo < 0)?angulo_alvo+360:angulo_alvo;
		
		//atualiza o angulo da arma
		if (angulo_arma >= 360 )
			angulo_arma = 0;
		else if (angulo_arma < 0)
			angulo_arma = 360;
			
		if (angulo_arma == angulo_alvo)
			;
		else if (angulo_arma+velocidade_torre > angulo_alvo && angulo_arma-velocidade_torre < angulo_alvo) {
		    angulo_arma = angulo_alvo;
		} else {
			if (angulo_arma >= 180 && (((int)angulo_arma+180)%360 > angulo_alvo || angulo_arma < angulo_alvo))
				angulo_arma += velocidade_torre;
			else if (((int)angulo_arma+180)%360 > angulo_alvo && angulo_arma < angulo_alvo)
				angulo_arma += velocidade_torre;
			else
				angulo_arma -= velocidade_torre;
		}
		for (k = 0; k < 2; k++) {
			partes_da_torre[k].angulo = angulo_arma;
		}
		
		//atualiza o angulo do chassi e das esteiras
		if (angulo >= 360)
			angulo = 0;
		else if (angulo < 0)
			angulo = 360;
			
		for (k = 0; k < 4; k++) {
			partes_do_chassi[k].angulo = angulo;
		}
		for (k = 0; k < 2; k++) {
			lagartas[k].angulo = angulo;
		}
		
		//atualiza a posição do tanque e das lagartas
		local = somar(local, mover(velocidade/40, angulo));
		posicao_lagarta_esq += velocidade/40;
		posicao_lagarta_dir += velocidade/40;
		
		//atualiza a velocidade do tanque
		if (velocidade > 0)
			velocidade = MAX(velocidade-22, 0);
		else
			velocidade = MIN(velocidade+4, 0);
	}
	//finaliza a aplicação
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
}
