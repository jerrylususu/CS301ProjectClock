//
// Created by Mingji Han on 2019/12/14.
//
#include <stdio.h>
#include <stdint-gcc.h>
#include <string.h>
#include "usart.h"
#include "../Inc/command.h"
#define SEND_VALID(s)   do {\
                            sprintf(msg, "Valid command %s\r\n", s);\
                            send_message(); \
                        }while(0);
#define SEND_INVALID()   do {\
                            sprintf(msg, "Invalid command\r\n");\
                            send_message(); \
                        }while(0);

extern UART_HandleTypeDef huart1;
extern char msg[100];


const unsigned char* set_comm[] = {"time", "alarm", "countdown"};
const unsigned char* lc_comm[] = {"alarm", "countdown"};

void send_message() {
    HAL_UART_Transmit(&huart1, (uint8_t *) msg, strlen(msg), HAL_MAX_DELAY);
}



uint8_t parse_literal(unsigned char *s, const unsigned char *literal) {
    uint8_t len = strlen(s);
    uint8_t litlen = strlen(literal);
    uint8_t tag = 1;
    if (len >= litlen) {

        for (int i = 0 ; i < litlen; i++) {
            if (s[i] != literal[i]) {
                tag = 0;
                break;
            }
        }

    }
    else {
         tag = 0;
    }
    return tag;
}

void set_time() {
    SEND_VALID("set time");
}

void set_alarm_count() {
    SEND_VALID("set alarm / count ");
}

void list() {
    SEND_VALID("list alarm/countdown");
}

void cancel() {
    SEND_VALID("cancel alarm/countdown");
}
void parse_command(unsigned char *s) {
    uint8_t  len = strlen(s);
    unsigned char *p = s;

    if (len > 0) {
        switch (s[0]) {
            case 's' : {
                if (parse_literal(s, "set") && len >= 5) {
                    SEND_VALID("set");
                    p += 4;
                    for (int i = 0; i < 3; i++) {
                        if (parse_literal(p, set_comm[i])) {
                            if(i == 0) {
                                set_time();
                            }
                            else {
                                set_alarm_count();
                            }
                        }
                    }
                }
                else {
                    SEND_INVALID();
                }
            } break;
            case 'l' : {
                if (parse_literal(s, "list") && len >= 6) {
                    SEND_VALID("list");
                    p += 5;
                    for (int i = 0 ; i < 2; i++) {
                        if (parse_literal(p, lc_comm[i])) {
                            list();
                        }
                    }

                }
                else {
                    SEND_INVALID();
                }
             //   parse_list();
            } break;
            case 'c' : {
                if (parse_literal(s, "cancel") && len >= 8) {
                    SEND_VALID("cancel");
                    p += 7;
                    for (int i = 0 ; i < 2 ; i++) {
                        if (parse_literal(p, lc_comm[i])) {
                            cancel();
                        }
                    }
                }
                else {
                    SEND_INVALID();
                }

            } break;

            default: {
               SEND_INVALID();
            }

        }
    }
}

