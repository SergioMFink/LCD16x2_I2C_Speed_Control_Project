#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "stm32f1xx.h"

// Endereço do display I2C. Isso pode variar dependendo do módulo I2C.
#define LCD_ADDR 0x27

// Inicializa o display LCD com 16 colunas e 2 linhas
LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

void setup() {
    // Inicia a comunicação com o LCD
    lcd.init();
    
    // Liga a luz de fundo do LCD
    lcd.backlight();
    
    // Posiciona o cursor na primeira coluna da primeira linha
    lcd.setCursor(0, 0);    
    lcd.print("Iniciando "); //Print this
    lcd.setCursor(0, 1); //At secound row first column 
    lcd.print("a medição"); //Print this
    delay(2000); //wait for two secounds 
    lcd.clear(); //Clear the screen  
}

void PWM_Init(void);
void Set_PWM_DutyCycle(uint16_t dutyCycle);


int main(void) {
    PWM_Init();

    while (1) {
        // Aumentar velocidade de 0% a 100%
        for (uint16_t dutyCycle = 0; dutyCycle <= 1000; dutyCycle += 10) {
            Set_PWM_DutyCycle(dutyCycle);
            lcd.setCursor(0, 1); // Set cursor to the second line
            lcd.print("Duty: ");
            lcd.print(dutyCycle); // Print the value of dutyCycle
			lcd.print("    "); // Clear any remaining characters
            delay(500); // Delay to make the change visible            
        }
        
        // Diminuir velocidade de 100% a 0%
        for (uint16_t dutyCycle = 1000; dutyCycle > 0; dutyCycle -= 10) {
            Set_PWM_DutyCycle(dutyCycle);
            lcd.setCursor(0, 1); // Set cursor to the second line
            lcd.print("Duty: ");
            lcd.print(dutyCycle); // Print the value of dutyCycle
			lcd.print("    "); // Clear any remaining characters
            delay(500); // Delay to make the change visible  
        }
    }
}



void PWM_Init(void) {
    // Habilitar clock para GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    
    // Configurar PA8 como saída alternativa push-pull (TIM1 CH1)
    GPIOA->CRH &= ~GPIO_CRH_CNF8;
    GPIOA->CRH |= GPIO_CRH_CNF8_1;
    GPIOA->CRH |= GPIO_CRH_MODE8;

    // Habilitar clock para TIM1
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    // Configurar PWM no TIM1 CH1
    TIM1->PSC = 72 - 1;  // Prescaler (72MHz/72 = 1MHz)
    TIM1->ARR = 1000 - 1;  // Período (1MHz/1000 = 1kHz PWM)
    
    TIM1->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos);  // Modo PWM1 no canal 1
    TIM1->CCMR1 |= TIM_CCMR1_OC1PE;            // Ativar preload para CCR1
    
    TIM1->CCER |= TIM_CCER_CC1E;               // Ativar saída no canal 1
    
    TIM1->CR1 |= TIM_CR1_ARPE;                 // Ativar preload para ARR
    TIM1->EGR |= TIM_EGR_UG;                   // Gerar atualização para aplicar as configurações
    
    TIM1->BDTR |= TIM_BDTR_MOE;                // Ativar a saída principal
    TIM1->CR1 |= TIM_CR1_CEN;                  // Iniciar o timer
}

void Set_PWM_DutyCycle(uint16_t dutyCycle) {
    // dutyCycle: 0 a 1000 para 0% a 100%
    TIM1->CCR1 = dutyCycle;
}
