#include <stdint.h>

#define RCC_AHB1ENR  (*(volatile uint32_t *)0x40023830)
#define RCC_APB1ENR  (*(volatile uint32_t *)0x40023840)
#define GPIOA_MODER  (*(volatile uint32_t *)0x40020000)
#define GPIOA_AFRL   (*(volatile uint32_t *)0x40020020)
#define USART2_SR    (*(volatile uint32_t *)0x40004400)
#define USART2_DR    (*(volatile uint32_t *)0x40004404)
#define USART2_BRR   (*(volatile uint32_t *)0x40004408)
#define USART2_CR1   (*(volatile uint32_t *)0x4000440C)

void hw_init() {
    RCC_AHB1ENR |= (1 << 0);
    RCC_APB1ENR |= (1 << 17);
    
    GPIOA_MODER |= (2 << 4);
    GPIOA_AFRL  |= (7 << 8);
    
    USART2_BRR = 0x0682; // 9600 @ 16MHz
    USART2_CR1 = (1 << 13) | (1 << 3);
}

void dbg_print(const char *msg) {
    while (*msg) {
        while (!(USART2_SR & (1 << 7))); 
        USART2_DR = *msg++;
    }
}


int chamber_temp_c = 22; 
int heater_relay_active = 0;

int read_thermocouple() {
    // crude physical simulation of thermal mass inside the chamber
    if (heater_relay_active) chamber_temp_c += 3;
    else if (chamber_temp_c > 22) chamber_temp_c -= 1;
    
    return chamber_temp_c;
}

int main(void) {
    hw_init();
    dbg_print("SYS: Kiln controller booting...\r\n");
    
    int target_cure_temp = 130; 
    int soak_cycles = 0;
    int state = 1; // 1=ramp, 2=soak, 3=cooldown, 9=fault
    
    dbg_print("CTRL: Engaging elements, ramping up\r\n");
    
    while (1) {
        int current_temp = read_thermocouple();
        
        // hardware safety cutoff 
        if (current_temp > 155) {
            heater_relay_active = 0;
            state = 9;
            dbg_print("FAULT: THERMAL RUNAWAY. RELAY KILLED.\r\n");
            while(1); 
        }
        
        if (state == 1) {
            heater_relay_active = 1;
            
            if (current_temp >= target_cure_temp) {
                state = 2;
                dbg_print("CTRL: Target reached. Starting soak timer.\r\n");
            }
        } 
        else if (state == 2) {
            // simple bang-bang control to hold temp
            if (current_temp < target_cure_temp - 2) {
                heater_relay_active = 1;
            } else {
                heater_relay_active = 0;
            }
            
            soak_cycles++;
            if (soak_cycles > 45) { // scaled down duration for sim
                state = 3;
                heater_relay_active = 0;
                dbg_print("CTRL: Cure cycle complete. Elements disengaged.\r\n");
            }
        }
        
        // block execution to avoid flooding terminal
        for (volatile int i = 0; i < 90000; i++); 
    }
    return 0;
}

__attribute__((section(".isr_vector")))
uint32_t *vector_table[] = {
    (uint32_t *)0x20020000,
    (uint32_t *)main,
};
