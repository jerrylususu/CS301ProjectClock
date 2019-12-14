//
// Created by Mingji Han on 2019/12/14.
//
#include <stdio.h>
#include <stdint-gcc.h>
#include <string.h>
#include "usart.h"
#include "../Inc/command.h"
extern UART_HandleTypeDef huart1;
extern char msg[100];


const unsigned char* set_comm[] = {"time", "alarm", "countdown"};
const unsigned char* lc_comm[] = {"alerm", "countdown"};

void send_message() {
    HAL_UART_Transmit(&huart1, (uint8_t *) msg, strlen(msg), HAL_MAX_DELAY);
}



uint8_t parse_literal(unsigned char *s ,uint8_t len, unsigned char *literal) {
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

void parse_command(unsigned char *s) {
    uint8_t  len = strlen(s);

    if (len > 0) {
        switch (s[0]) {
            case 's' : {
                if (parse_literal(s, len, "set")) {
                    parse_set();
                }

                } break;
            case 'l' : {
                parse_literal(s, len, "list");
             //   parse_list();
                } break;
            case 'c' : {
                parse_literal(s, len, "cancel");
              // parse_cancel();
            } break;
            default: {
                sprintf(msg, "Invalid command\r\n");
                send_message();
            }

        }
    }
}

