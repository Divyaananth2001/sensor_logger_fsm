#ifndef SRC_FSM_HPP_
#define SRC_FSM_HPP_

enum class State {
    IDLE,
    LOGGING,
    ERR
};

enum class Event {
    START,
    STOP,
    FAIL,
    RESET
};

void fsmTask_init(void);

#endif  // SRC_FSM_HPP_