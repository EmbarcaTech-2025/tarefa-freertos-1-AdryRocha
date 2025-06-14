/**
 * @file led_rgb.h
 * @brief Definições para o controle do LED RGB.
 *
 * Este arquivo contém as definições para inicialização e controle do LED RGB,
 * incluindo suspensão e retomada da tarefa do LED.
 */

#ifndef LED_RGB_H
#define LED_RGB_H

#include "FreeRTOS.h"
#include "task.h"

// Pinos do LED RGB conforme o esquemático da BitDogLab V6
#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12

/**
 * @brief Handle para a tarefa do LED RGB.
 * Usado para controlar a tarefa (suspender/retomar) a partir de outras tarefas.
 */
extern TaskHandle_t led_rgb_task_handle;

/**
 * @brief Tarefa que controla o ciclo de cores do LED RGB.
 * @param pvParameters Parâmetros da tarefa (não utilizado).
 */
void led_rgb_task(void *pvParameters);

#endif // LED_RGB_H