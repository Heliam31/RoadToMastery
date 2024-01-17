/**
 * \file sonar.c
 * \brief Programme de tests.
 * \author Léo BALBARIE
 * \version 0.1
 * \date 29 novembre 2023
 *
 * Programme de test de sonar sur stm32f4 Discovery
 *
 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include <stm32f4/adc.h>

#define PIN_ECHO 1
#define PIN_TRIG 2

// /**
//  * \fn void init_sonar (void)
//  * \brief Fonction d'initialisation du gpio pour le sonar.
// */
void init_sonar(){
    GPIOD_MODER = REP_BITS(GPIOD_MODER,PIN_ECHO*2,2,GPIO_MODER_IN);
    GPIOD_OTYPER &= ~(1<<PIN_ECHO);
    GPIOD_MODER = REP_BITS(GPIOD_MODER,PIN_TRIG*2,2,GPIO_MODER_OUT);
    GPIOD_OTYPER &= ~(1<<PIN_TRIG);
}

// /**
//  * \fn void diffuse (int d)
//  * \brief Fonction de diffusion d'un signal, durant d microsecondes.
//  * 
//  * \param[in] d Durée de diffusion du signal, exprimée en microsecondes.
// */
void diffuse(int d){
    int duree_ms = APB1_CLK * (d/1000000);
    GPIOD_BSRR = 1<<(PIN_TRIG+16);
    for (int i = 0; i < duree_ms; i++) NOP;
    GPIOD_BSRR = 1<<PIN_TRIG;
    for (int i = 0; i < duree_ms; i++) NOP;
    GPIOD_BSRR = 1<<(PIN_TRIG+16);
}

// /**
//  * \fn int recover (int d)
//  * \brief Fonction récuperant l'echo du singal émis, durant d microsecondes.
//  * 
//  * \param[in] d Durée d'écoute, exprimée en microsecondes.
//  * \return Durée de réception de l'echo exprimée par le type int (microsecondes) ou -1 si aucun echo n'est trouvé.
// */
int recover(int d){
    int duree_ms = APB1_CLK * (d/1000000);
    int cnt = 0;
    // for (int i = 0; i < duree_ms; i++) {
    //     if ((GPIOD_IDR & (1<<PIN_ECHO)) != 0) break;
    //     cnt++;
    // }
    // if (cnt >= duree_ms) cnt = -1;
    while ((GPIOD_IDR & (1<<PIN_ECHO)));
    while(!(GPIOD_IDR & (1<<PIN_ECHO))) cnt++;
    return APB1_CLK*cnt;
}

// /**
//  * \fn int convert_ms_to_cm (int d)
//  * \brief Fonction de convertion de microsecondes en centimètres.
//  * 
//  * \param[in] d Durée qu'a pris la réception de l'echo, exprimée en microsecondes.
//  * \return Distance, exprimée en centimètres.
// */
int convert_ms_to_cm(int d) {
    return (int)(d/29/2);
}

int main() {

	RCC_AHB1ENR |= RCC_GPIODEN;
    init_sonar();
    GPIOD_BSRR = 1<<(PIN_ECHO+16);

    while(1) {
        diffuse(10);
        int duree = recover(10);
        printf("%d\n",duree);
        // if (duree <= 1) printf("None\n");
        // else printf("%d cm\n", convert_ms_to_cm(duree));
    }

}
