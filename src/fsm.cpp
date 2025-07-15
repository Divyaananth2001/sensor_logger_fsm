#include <iostream>

#include "fsm.hpp"
#include "sensor.hpp"

extern "C" {
    #include "FreeRTOS.h"
    #include "queue.h"
}

extern QueueHandle_t event_q;

TaskHandle_t fsmTask_handle;

class SensorLoggerFSM {
 private:
    State currentState;
 public:
    SensorLoggerFSM() {
        currentState = State::IDLE;
    }
    void handle_event(Event event) {
        switch (currentState)
        {
        case State::IDLE:
            if (Event::START == event) {
                currentState = State::LOGGING;
                sensor_start_acquiring();
            }
            break;
        
        case State::LOGGING:
            if (Event::STOP == event) {
                currentState = State::IDLE;
                sensor_stop_acquiring();
            } else if (Event::FAIL == event) {
                currentState = State::ERR;
            }
            
            break;
        
        case State::ERR:
            if (Event::RESET == event) {
                currentState = State::IDLE;
            }
            break;
        }
    }
    State get_state() {
        return currentState;
    }
};

void print_current_state(SensorLoggerFSM *fsm_obj) {
    switch(fsm_obj->get_state()) {
    case State::IDLE:
        std::cout << "CurrentState: IDLE\n";
        break;
    case State::LOGGING:
        std::cout << "CurrentState: LOGGING\n";
        break;
    case State::ERR:
        std::cout << "CurrentState: ERROR\n";
        break;
    default:
        std::cout << "CurrentState: INVALID\n";
    }
}

void fsmTask(void *pvParameters) {
    SensorLoggerFSM fsm;
    Event event = Event::STOP;
    while (true) {
        print_current_state(&fsm);
        BaseType_t status = xQueueReceive(event_q, reinterpret_cast<void *>(&event), portMAX_DELAY);
        if (status == pdTRUE) {
            std::cout << "Event received = "<< static_cast<int>(event) << "\n";
            fsm.handle_event(event);
        }
        vTaskDelay(10);
    }
}

void fsmTask_init() {
    BaseType_t status = xTaskCreate(fsmTask, "FSM-Task", 1000, NULL, 1, &fsmTask_handle);
    if (pdPASS != status) {
        std::cout << "ERROR: Failed to create fsmTask\n";
    }
}