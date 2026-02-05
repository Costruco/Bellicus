#ifndef VEHICLE_MOVEMENT_COMPONENT_HPP
#define VEHICLE_MOVEMENT_COMPONENT_HPP

#include "Components.hpp"
#include "Vector2D.hpp"
#include "MovementInput.hpp"

class VehicleMovementComponent : public Component {
	public:
		TransformComponent * transform;
		float speed;
		float turningSpeed;
		int enginePower; 
		int mass;
		int brakePower;
		float rollingDrag;
			   
		VehicleMovementComponent(float speed, float turningSpeed, int enginePower, int mass, int brakePower, float rollingDrag) {
			this->speed = speed;
			this->turningSpeed = turningSpeed;
			this->enginePower = enginePower;
			this->mass = mass;
			this->brakePower = brakePower;
			this->rollingDrag = rollingDrag;
		}
		
		void init() override {
			transform = &entity->getComponent<TransformComponent>();
		}
		
		void update() override {
			transform->position += Vector2D::fromPolar(speed,transform->direction);
			//atualiza a marcha do tanque com base na marcha atual, velocidade e controle
			if (marcha == PONTO_MORTO) {
				if (aproxIgual(velocidade,velocidades[marcha].y) && passando == 0 && movimento == TRANSV) {
					inicioDaTroca = SDL_GetTicks();
					passando = 1;
				} else if (aproxIgual(velocidade,velocidades[marcha].x) && reduzindo == 0 && movimento == REV) {
					inicioDaTroca = SDL_GetTicks();
					reduzindo = 1;
				}
			} else {
				if (marcha != QUARTA && aproxIgual(velocidade,velocidades[marcha].y) && passando == 0 && movimento == TRANSV) {
					inicioDaTroca = SDL_GetTicks();
					passando = 1;
				} else if (marcha > PONTO_MORTO && aproxIgual(velocidade,velocidades[marcha].x) && reduzindo == 0 && movimento == REV) {
					marcha = MAX(marcha-1,PONTO_MORTO);
				} else if (marcha > PONTO_MORTO && aproxIgual(velocidade,velocidades[marcha].x) && reduzindo == 0 && movimento == NULO) {
					inicioDaTroca = SDL_GetTicks();
					reduzindo = 1;
				}
			}
			
			if (passando == 1 && SDL_GetTicks()-inicioDaTroca >= 250/VELOCIDADE_DE_TROCA_DE_MARCHA) {
				marcha = MIN(marcha+1,QUARTA);
				passando = 0;
			} else if (reduzindo == 1 && SDL_GetTicks()-inicioDaTroca >= 250/VELOCIDADE_DE_TROCA_DE_MARCHA) {
				marcha = MAX(marcha-1,RE);
				reduzindo = 0;
			}
			
			//acelera ou desacelera o veiculo com base na marcha atual, velocidade e controle
			int troca = (passando == 1 || reduzindo == 1);
			double TURNING = 0.035*(direcao != RETO);
			if (velocidade < 0) {
				if (movimento == NULO)
					velocidade = MIN(velocidades[marcha].y,velocidade+desaceleracao(FRENAGEM,MASSA)*32.5/FPS);
				else if (movimento == TRANSV)
					velocidade = MIN(velocidades[marcha].y,velocidade+desaceleracao(FRENAGEM,MASSA)*32.5/FPS);
				else if (movimento == REV && !troca)
					velocidade = MAX(velocidades[marcha].x,velocidade+aceleracao(velocidade,MASSA,POTENCIA,ATRITO_DO_SOLO,ATRITO_DE_ROLAMENTO+TURNING)/FPS);
			} else if (velocidade == 0) {
				if (movimento == TRANSV && !troca)
					velocidade = MAX(velocidades[marcha].x,velocidade+aceleracao(velocidade,MASSA,POTENCIA,ATRITO_DO_SOLO,ATRITO_DE_ROLAMENTO+TURNING)/FPS);
				else if (movimento == REV && !troca)
					velocidade = MIN(velocidades[marcha].y,velocidade-aceleracao(velocidade,MASSA,POTENCIA,ATRITO_DO_SOLO,ATRITO_DE_ROLAMENTO+TURNING)/FPS);
			} else if (velocidade > 0){
				if (movimento == NULO)
					velocidade = MAX(velocidades[marcha].x,velocidade-desaceleracao(FRENAGEM,MASSA)*32.5/FPS);
				else if (movimento == TRANSV && !troca)
					velocidade = MIN(velocidades[marcha].y,velocidade+aceleracao(velocidade,MASSA,POTENCIA,ATRITO_DO_SOLO,ATRITO_DE_ROLAMENTO+TURNING)/FPS);
				else if (movimento == REV)
					velocidade = MAX(velocidades[marcha].x,velocidade-desaceleracao(FRENAGEM,MASSA)*32.5/FPS);
			}
		
			//atualiza o angulo do veiculo com base nos controles
			switch (direcao) {
				case RETO:
					break;
				case ESQ:
					if (movimento != REV) {
						angulo -= VELOCIDADE_ANGULAR/FPS;
						angulo_arma -= VELOCIDADE_ANGULAR/FPS;
						angulo_metra -= VELOCIDADE_ANGULAR/FPS;	
						break;
					} else {
						angulo += VELOCIDADE_ANGULAR/FPS;
						angulo_arma += VELOCIDADE_ANGULAR/FPS;
						angulo_metra += VELOCIDADE_ANGULAR/FPS;
						break;
					}
				case DIR:
					if (movimento != REV) {
						angulo += VELOCIDADE_ANGULAR/FPS;
						angulo_arma += VELOCIDADE_ANGULAR/FPS;
						angulo_metra += VELOCIDADE_ANGULAR/FPS;	
						break;
					} else {
						angulo -= VELOCIDADE_ANGULAR/FPS;
						angulo_arma -= VELOCIDADE_ANGULAR/FPS;
						angulo_metra -= VELOCIDADE_ANGULAR/FPS;
						break;
					}
			}
		}	
};

#endif
