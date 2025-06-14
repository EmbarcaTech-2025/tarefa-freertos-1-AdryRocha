
# Tarefa: Roteiro de FreeRTOS #1 - EmbarcaTech 2025

Autor: Adriana Rocha

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Campinas, 13 de 2025

Estrutura de Pastas do Projeto

RTOS_BitDogLab_FreeRTOS

/
│

├── .gitignore             # Ignora arquivos desnecessários no Git

├── CMakeLists.txt         # Arquivo de configuração da compilação

├── FreeRTOSConfig.h       # Arquivo de configuração do FreeRTOS

├── LICENSE                # Licença de uso do projeto (MIT)

├── README.md              # Documentação principal do projeto

│

└── src/                   # Pasta com todo o código-fonte da aplicação

    ├── button.c
    ├── button.h
    ├── buzzer.c
    ├── buzzer.h
    ├── led_rgb.c
    ├── led_rgb.h
    └── main.c

## Sistema Multitarefa com FreeRTOS na BitDogLab (Raspberry Pi Pico W)
1. Visão Geral do Projeto
Este repositório contém um sistema embarcado multitarefa desenvolvido para a placa BitDogLab V6, que utiliza um Raspberry Pi Pico W. O projeto demonstra o uso de um Sistema Operacional de Tempo Real (RTOS), especificamente o FreeRTOS, para gerenciar de forma concorrente três periféricos: um LED RGB, um Buzzer e dois botões de interação.

O objetivo é servir como um material de estudo e um template robusto para projetos mais complexos que exijam gerenciamento de múltiplas tarefas, previsibilidade e modularidade em sistemas embarcados.

## 2. Funcionalidades
O firmware implementa as seguintes funcionalidades simultaneamente:

Tarefa do LED RGB: Controla um LED RGB, alternando ciclicamente entre as cores Vermelho, Verde e Azul a cada 500 milissegundos.

Tarefa do Buzzer: Aciona um buzzer periodicamente, emitindo um "beep" curto de 200ms a cada 1 segundo.

Tarefa de Controle (Botões): Monitora dois botões para interação com o usuário:

Botão A (GPIO 5): Suspende ou retoma a tarefa do LED RGB. Ao pressionar, o ciclo de cores para; ao pressionar novamente, ele continua de onde parou.

Botão B (GPIO 6): Suspende ou retoma a tarefa do Buzzer. Ao pressionar, os beeps param; ao pressionar novamente, eles retornam.

## 3. Hardware Necessário
Placa BitDogLab V6 com Raspberry Pi Pico W.
Cabo Micro-USB para gravação do firmware e alimentação.

Pinagem Utilizada
Periférico   -   Pino Físico   -   GPIO   -  Descrição
LED Vermelho         17          GPIO 13    Componente do LED RGB externo

LED Verde            15          GPIO 11    Componente do LED RGB externo

LED Azul             16          GPIO 12    Componente do LED RGB externo

Buzzer               27          GPIO 21    Buzzer para emissão de som

Botão A              7           GPIO 5     Botão para suspender/retomar LED

Botão B              9           GPIO 6     Botão para suspender/retomar Buzzer

## 4. Software e Ferramentas
Para compilar e gravar este projeto, você precisará ter o ambiente de desenvolvimento para o Raspberry Pi Pico configurado.

VS Code: Editor de código principal.
Pico SDK: Kit de desenvolvimento de software da Raspberry Pi.
ARM GCC Toolchain: Compilador para a arquitetura ARM.
CMake: Ferramenta de automação de compilação.
Git: Para clonar o repositório.
(Opcional) Ninja: Sistema de compilação, geralmente mais rápido que o Make.

## 5. Como Compilar e Gravar
Passo 1: Clonar o Repositório
Clone este repositório e o submódulo do kernel do FreeRTOS, que é um requisito.

# Clone o projeto principal
git clone <URL_DO_SEU_REPOSITORIO_AQUI>
cd RTOS_BitDogLab_FreeRTOS

# Clone o kernel do FreeRTOS (necessário)
git clone [https://github.com/FreeRTOS/FreeRTOS-Kernel.git](https://github.com/FreeRTOS/FreeRTOS-Kernel.git) free_rtos_kernel

Passo 2: Configurar o Projeto com CMake

Crie um diretório build e execute o CMake a partir dele para configurar o projeto.

mkdir build
cd build
# Aponte para o caminho onde o seu Pico SDK está instalado
cmake .. -DPICO_SDK_PATH=/caminho/para/seu/pico-sdk

Passo 3: Compilar o Código

Execute o ninja (ou make) para compilar todo o código-fonte.
# Usando Ninja (recomendado)
ninja
# Ou, se não tiver o Ninja, use o Make
make

Se tudo ocorrer bem, um arquivo rtos_bitdoglab.uf2 será gerado dentro da pasta build.

Passo 4: Gravar na Placa
Pressione e segure o botão BOOTSEL da sua Raspberry Pi Pico W.
Conecte-a ao computador via cabo USB. Ela será montada como um disco removível.
Arraste e solte o arquivo rtos_bitdoglab.uf2 para dentro do disco da Pico.
A placa irá reiniciar automaticamente e começará a executar o programa.

## 6. Análise da Arquitetura e do Código
A arquitetura do projeto foi pensada para ser modular, escalável e de fácil manutenção, princípios fundamentais no desenvolvimento de firmware de qualidade.

### 6.1. Estrutura do Projeto
O código foi desacoplado em módulos lógicos (led_rgb, buzzer, button). Cada módulo tem seu próprio arquivo .h (interface) e .c (implementação). Isso permite que as equipes trabalhem em paralelo e facilita a substituição ou o teste de um componente sem afetar os outros. O main.c atua como um "integrador", orquestrando a criação das tarefas, mas sem conter a lógica de negócio de cada periférico.

### 6.2. Análise do Código-Fonte
main.c
Este arquivo é o ponto de entrada (main) e o coração do sistema multitarefa. Sua única responsabilidade é inicializar o hardware essencial, criar as tarefas (threads) que executarão as funcionalidades e entregar o controle ao escalonador do FreeRTOS com vTaskStartScheduler(). A partir daí, o RTOS assume o controle total do fluxo de execução.

# Usamos a função xTaskCreate() da API do FreeRTOS.
xTaskCreate(led_rgb_task, "LED_Task", 256, NULL, 1, &led_rgb_task_handle);
led_rgb_task: Ponteiro para a função que implementa a tarefa.
"LED_Task": Nome descritivo para depuração.
256: Tamanho da pilha (stack) da tarefa, em palavras. 256 palavras (1024 bytes) é um valor seguro para nossas tarefas simples.
NULL: Parâmetros passados para a tarefa (nenhum neste caso).
1: Prioridade da tarefa. Demos prioridade 2 à tarefa dos botões para garantir uma resposta mais rápida à interação do usuário.
&led_rgb_task_handle: Ponteiro para uma variável que armazenará o "handle" (identificador) da tarefa. Precisamos dele para poder suspendê-la ou retomá-la de outra tarefa.

# led_rgb.c / buzzer.c
Essas tarefas representam "threads" independentes e bloqueantes. Elas vivem em um loop while(1) eterno, mas utilizam a função vTaskDelay(). Essa chamada é crucial: ela não é um delay "busy-wait" que consome CPU. Em vez disso, ela informa ao escalonador do RTOS que a tarefa pode ser "adormecida" pelo tempo especificado, permitindo que outras tarefas (como a do buzzer ou dos botões) usem o processador. Isso garante a concorrência e o uso eficiente da CPU.
A tarefa do LED manipula GPIOs com gpio_put(), enquanto a do buzzer usa o periférico de hardware PWM (pwm_set_chan_level()) para gerar uma onda sonora na frequência desejada, uma abordagem muito mais eficiente do que tentar "bit-banging" por software.

# button.c
A lógica de controle do sistema está encapsulada aqui. Esta tarefa tem uma prioridade maior para garantir que a entrada do usuário seja processada rapidamente. Ela demonstra a comunicação inter-tarefas, onde uma tarefa (botões) controla o estado de outras (LED e buzzer).
A tarefa utiliza uma técnica de polling com debounce por software.
Polling: !gpio_get(BUTTON_A_PIN) verifica continuamente se o pino do botão está em nível baixo.
Debounce: Uma flag a_pressed e um vTaskDelay() após a detecção garantem que um único pressionamento mecânico não seja interpretado como múltiplos cliques.
Controle de Tarefas: As funções vTaskSuspend(led_rgb_task_handle) e vTaskResume(led_rgb_task_handle) são usadas para pausar e continuar a execução das outras tarefas, usando os handles que foram salvos durante a criação.

### 7. A Importância de Usar um RTOS
Em sistemas embarcados simples, um loop while(1) na main() (conhecido como "super loop") pode ser suficiente. No entanto, à medida que a complexidade aumenta, essa abordagem se torna insustentável.

Previsibilidade e Tempo Real: Um RTOS garante que tarefas críticas sejam executadas dentro de prazos definidos (determinismo), algo que um super loop não pode garantir. O escalonador preemptivo do FreeRTOS sempre garante que a tarefa de maior prioridade pronta para executar receba o controle da CPU.

Modularidade e Manutenção: Dividir a lógica em tarefas independentes torna o código mais limpo, mais fácil de entender e de dar manutenção. Adicionar uma nova funcionalidade significa criar uma nova tarefa, sem precisar reestruturar todo o super loop.

Eficiência de Recursos: Funções bloqueantes como vTaskDelay() e semáforos permitem que as tarefas "durmam" quando não têm nada para fazer, liberando a CPU para outras atividades ou para entrar em modo de baixo consumo de energia. Um delay implementado com um laço for em um super loop consumiria 100% da CPU sem fazer nada útil.

Comunicação e Sincronização: O RTOS oferece mecanismos robustos e seguros (filas, semáforos, mutexes) para que as tarefas troquem dados e sincronizem suas ações, evitando condições de corrida e corrupção de dados.


## Relatório de Análise do Sistema Multitarefa com FreeRTOS

# 1. Análise de Comportamento: Tarefas com a Mesma Prioridade
Pergunta: O que acontece se todas as tarefas tiverem a mesma prioridade?
Resposta: Quando todas as tarefas em um sistema FreeRTOS são configuradas com a mesma prioridade, o escalonador implementa uma política de agendamento conhecida como Round-Robin com Time-Slicing.

# Comportamento Detalhado:
Execução em Fatias de Tempo (Time-Slicing): O escalonador do FreeRTOS concede a cada tarefa uma pequena "fatia" de tempo de CPU para executar. Ele alterna rapidamente entre as tarefas que estão no estado "Pronto" (Ready), criando a ilusão de que todas estão rodando simultaneamente.

O Papel do vTaskDelay(): Neste projeto, todas as tarefas (led_rgb_task, buzzer_task, button_task) contêm chamadas para vTaskDelay(). Esta função é crucial: quando uma tarefa a chama, ela voluntariamente cede o controle da CPU e entra no estado "Bloqueado" (Blocked) pelo tempo especificado.

Cooperação e Eficiência: Assim que a tarefa A (ex: LED) entra em delay, o escalonador imediatamente passa o controle para a próxima tarefa pronta de mesma prioridade, como a tarefa B (ex: Buzzer). Quando a tarefa B também entra em delay, o controle passa para a tarefa C (ex: Botões), e assim por diante. O processador nunca fica ocioso executando um delay "vazio"; ele está sempre executando o código útil da próxima tarefa pronta.

Conclusão: Se todas as três tarefas tivessem a mesma prioridade, o sistema continuaria funcionando corretamente. A alternância entre elas seria gerenciada de forma cooperativa pelas chamadas de vTaskDelay(). No entanto, perderíamos a capacidade de garantir que a tarefa dos botões (a mais crítica em termos de resposta ao usuário) seja atendida antes das outras, o que poderia levar a uma pequena latência perceptível se as outras tarefas estivessem realizando operações muito intensivas.

### 2. Análise de Consumo de CPU
Pergunta: Qual tarefa consome mais tempo da CPU?
Resposta: Para determinar qual tarefa consome mais tempo de CPU, devemos analisar com que frequência cada tarefa está no estado "Executando" (Running) em vez de "Bloqueado" (Blocked).

Tarefa do LED (led_rgb_task): Esta tarefa executa um pequeno trecho de código (liga/desliga GPIO) e então entra em um estado bloqueado por 500ms (vTaskDelay(pdMS_TO_TICKS(500))). Ela fica inativa a maior parte do tempo.

Tarefa do Buzzer (buzzer_task): Similarmente, a tarefa do buzzer fica bloqueada por 200ms e depois por 800ms. No total, ela fica bloqueada por 1000ms a cada ciclo, executando apenas breves operações de PWM entre os delays.

Tarefa dos Botões (button_task): Esta é a tarefa que mais consome CPU. Seu loop principal tem um delay de apenas 20ms (vTaskDelay(pdMS_TO_TICKS(20))). Isso significa que, aproximadamente 50 vezes por segundo (1000ms / 20ms), esta tarefa "acorda", executa sua lógica de verificação dos GPIOs (gpio_get) e volta a dormir.

Conclusão: A button_task é a que consome mais tempo de CPU. Embora o tempo de execução de sua lógica seja muito curto, a frequência com que ela é executada pelo escalonador é significativamente maior do que a das outras duas tarefas. Em um sistema com muitas tarefas, otimizar o tempo de polling ou usar interrupções em vez de polling seria uma estratégia para reduzir ainda mais o consumo da CPU.

### 3. Análise de Riscos: Polling sem Prioridades
Pergunta: Quais seriam os riscos de usar polling sem prioridades?
Resposta: Usar "polling sem prioridades" é a abordagem típica de um sistema bare-metal (sem sistema operacional), onde todas as verificações são feitas sequencialmente dentro de um único loop while(1).

// Exemplo de Super Loop (Bare-metal)
while(1) {
    checar_led();       // Leva, por ex., 1ms
    checar_buzzer();    // Leva, por ex., 1ms
    checar_botoes();    // Leva, por ex., 1ms
    checar_sensor_x();  // Leva, por ex., 300ms!
}

## Os riscos dessa abordagem são significativos, especialmente à medida que o sistema cresce:

Latência e Falta de Responsividade: O maior risco é o atraso na resposta a eventos críticos. No exemplo acima, se a função checar_sensor_x() levar 300ms para ser concluída, o sistema ficará "cego" para qualquer outra entrada durante esse período. Se um usuário pressionar um botão de emergência no início da checagem do sensor, a resposta ao botão só ocorrerá após 300ms, o que pode ser inaceitável.

Inanição de Tarefas (Task Starvation): Uma tarefa longa e bloqueante pode impedir que outras tarefas mais curtas e urgentes sejam executadas. Isso é conhecido como "starvation". A tarefa de checar_botoes() está "morrendo de fome" enquanto espera a tarefa do sensor terminar.

Dependência Temporal: O tempo de execução de todo o loop depende da soma do tempo de cada função. Se uma função for modificada e passar a demorar mais, todo o comportamento temporal do sistema é afetado, tornando-o frágil e difícil de manter. Uma pequena mudança pode ter consequências imprevisíveis.

Impossibilidade de Previsão (Não-Determinismo): É impossível garantir que um evento será tratado dentro de um prazo máximo, pois o tempo de resposta depende do ponto exato do loop em que o evento ocorre.

Conclusão: O polling sem prioridades, característico de sistemas bare-metal, é simples para projetos pequenos, mas extremamente arriscado para sistemas que exigem responsividade e previsibilidade. A introdução de um RTOS com escalonamento preemptivo por prioridades resolve esses problemas, permitindo que uma tarefa de alta prioridade (como a resposta a um botão) interrompa uma tarefa de baixa prioridade (como a leitura de um sensor lento), garantindo que os eventos críticos sejam sempre tratados com a urgência necessária.

## 8. Glossário de Termos do FreeRTOS
Tarefa (Task): Uma função independente que roda em seu próprio contexto, com sua própria pilha. É a unidade fundamental de execução.

Escalonador (Scheduler): O núcleo do RTOS, responsável por decidir qual tarefa deve ser executada em cada instante.

Preempção: A capacidade do escalonador de interromper uma tarefa de baixa prioridade para executar uma tarefa de maior prioridade que se tornou pronta.

Handle de Tarefa (Task Handle): Um "ponteiro" ou identificador para uma tarefa, usado para manipulá-la (ex: suspender, retomar, deletar).

Tick do Sistema: Uma interrupção periódica de um timer que serve como o "batimento cardíaco" do RTOS, permitindo medir o tempo e gerenciar delays.

Polling: Técnica de verificar repetidamente o estado de um dispositivo (como um botão) para ver se ele mudou.

Debounce: Processo de eliminar os ruídos elétricos gerados pelo acionamento mecânico de um botão, para que um único toque seja lido como um único evento.

## 9. Referências e Links Úteis
Documentação Oficial do FreeRTOS
Documentação do Raspberry Pi Pico SDK
Site Oficial da BitDogLab


## 📜 Licença
GNU GPL-3.0.
