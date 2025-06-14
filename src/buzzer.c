/**
 * @file buzzer.c
 * @brief Implementação do controle do buzzer.
 *
 * Este arquivo contém a implementação das funções para inicialização e controle
 * do buzzer, incluindo a tarefa que gera o sinal PWM para o buzzer.
 */

#include "buzzer.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "FreeRTOS.h"
#include "task.h"

// Handle da tarefa, definido no main.c
TaskHandle_t buzzer_task_handle = NULL;

/**
 * @brief Inicializa o pino do buzzer para operar com PWM.
 */
void buzzer_init(void) {
    // Configura o pino do buzzer para a função PWM
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    // Obtém o "slice" PWM correspondente ao pino
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    // Obtém a configuração padrão de PWM
    pwm_config config = pwm_get_default_config();
    // Inicia o PWM com a configuração padrão
    pwm_init(slice_num, &config, true);
    // Garante que o buzzer comece desligado (nível 0)
    pwm_set_gpio_level(BUZZER_PIN, 0);
}

/**
 * @brief Tarefa do buzzer.
 *
 * Inicializa o buzzer e entra em um loop infinito para gerar um beep
 * de 200ms a cada 1 segundo.
 *
 * @param pvParameters Parâmetros da tarefa (não utilizado).
 */
void buzzer_task(void *pvParameters) {
    buzzer_init();
    
    // Obtém o "slice" e o "channel" do PWM para o pino do buzzer
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint chan = pwm_gpio_to_channel(BUZZER_PIN);

    // Configura o PWM para uma frequência de ~2kHz
    pwm_set_wrap(slice_num, 4095);
    pwm_set_clkdiv(slice_num, 25);

    while (true) {
        // Ativa o buzzer com 50% de duty cycle para gerar o som
        pwm_set_chan_level(slice_num, chan, 2048);
        vTaskDelay(pdMS_TO_TICKS(200));

        // Desativa o buzzer
        pwm_set_chan_level(slice_num, chan, 0);
        vTaskDelay(pdMS_TO_TICKS(800));
    }
}