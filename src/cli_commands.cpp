#include <iostream>

extern "C" {
    #include "FreeRTOS.h"
    #include "FreeRTOS_CLI.h"
    #include "queue.h"
}

#include "fsm.hpp"

extern QueueHandle_t event_q;

TaskHandle_t cliTask_handle;

BaseType_t start_cmd(char *writeBuffer, size_t writeBufferLen, const char *cmdString) {
    Event event = Event::START;
    BaseType_t status = xQueueSend(event_q, &event, 0);
    if (status != pdPASS) {
        std::cout << "Failed to enque event: start\n";
        snprintf(writeBuffer, writeBufferLen, "FSM: Failed to send START command.\n");
    } else {
        snprintf(writeBuffer, writeBufferLen, "FSM: START command sent.\n");
    }
    return pdFALSE;
}

const CLI_Command_Definition_t startCommand = {
    "start",
    "start: Start acquiring data from sensor.\r\n",
    start_cmd,
    0
};

BaseType_t stop_cmd(char *writeBuffer, size_t writeBufferLen, const char *cmdString) {
    Event event = Event::STOP;
    BaseType_t status = xQueueSend(event_q, &event, 0);
    if (status != pdPASS) {
        std::cout << "Failed to enque event: stop\n";
        snprintf(writeBuffer, writeBufferLen, "FSM: Failed to send STOP command.\n");
    } else {
        snprintf(writeBuffer, writeBufferLen, "FSM: STOP command sent.\n");
    }
    return pdFALSE;
}

const CLI_Command_Definition_t stopCommand = {
    "stop",
    "stop: Stop acquiring data from sensor.\r\n",
    stop_cmd,
    0
};


void cliTask(void *pvParameters) {
    char inputBuffer[128];
    char outputBuffer[512];
    
    while (true) {
        printf(">> ");
        fflush(stdout);
        if (fgets(inputBuffer, sizeof(inputBuffer), stdin) != NULL) {
            //  Remove newline at the end (if present)
            size_t len = strlen(inputBuffer);
            if (len > 0 && inputBuffer[len - 1] == '\n') {
                inputBuffer[len - 1] = '\0';
            }
            BaseType_t status = pdTRUE;
            while (status != pdFALSE) {
                memset(outputBuffer, 0, sizeof(outputBuffer));
                status = FreeRTOS_CLIProcessCommand(inputBuffer, outputBuffer, sizeof(outputBuffer));
                if (strlen(outputBuffer) > 0) {
                    std::cout << outputBuffer;
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void cli_handler_init() {
    BaseType_t status = xTaskCreate(cliTask, "CLI-Task", 5000, NULL, 1, &cliTask_handle);
    if (pdPASS != status) {
        std::cout << "ERROR: Failed to create cliTask\n";
    }
    status = FreeRTOS_CLIRegisterCommand(&startCommand);
    if (pdPASS != status) {
        std::cout << "ERROR: Failed to register startCommand.\n";
    }
    status = FreeRTOS_CLIRegisterCommand(&stopCommand);
    if (pdPASS != status) {
        std::cout << "ERROR: Failed to register stopCommand.\n";
    }
}