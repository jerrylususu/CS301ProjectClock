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
void parse_date_str(unsigned char *s) {
    uint8_t len = strlen(s);
    int time[3] = {0,0,0};
    uint8_t flag = 1;
    if (len < 8) {
        SEND_INVALID();
    }
    else {
        int temp = 0;
        for (int i = 0 ; i < 8 ; i++) {

            if ( s[i] >= '0' && s[i] <= '9' ) {
                temp = temp * 10 + s[i] - '0';

                if ( i == 3) {
                    time[0] = temp;
                    temp = 0;
                }
                else if (i > 3 && (i + 1) % 2 == 0){
                    time[ (i - 3) / 2] = temp;
                    temp = 0;
                }
            }
            else {
                flag = 0;
                SEND_INVALID();
                break;
            }
        }
    }

    if (flag) {
        sprintf(msg, "%d-%d-%d\r\n", time[0], time[1], time[2]);
        send_message();
    }
}

void parse_time_str(unsigned char *s) {
    uint8_t len = strlen(s);
    int time[3] = {0,0,0};
    uint8_t flag = 1;
    if (len < 6) {

        SEND_INVALID();
    }
    else {
        uint8_t  temp = 0;
        for (int i = 0 ; i < 6 ; i++) {

            if ( s[i] >= '0' && s[i] <= '9' ) {
                temp = temp * 10 + s[i] - '0';
                if ( (i + 1) % 2 == 0) {
                    time[i / 2] = temp;
                    temp = 0;
                }
            }
            else {
                flag = 0;
                SEND_INVALID();
                break;
            }

        }
    }
    if (flag) {
        sprintf(msg, "%d:%d:%d\r\n", time[0], time[1], time[2]);
        send_message();
    }
}

void set_time(unsigned char *s) {
    uint8_t len = strlen(s);
    unsigned char time_str[100];
    memset(time_str, 0, sizeof(time_str));
    SEND_VALID("set time");
    if (len >= 5 + 14) {
        SEND_VALID("time length");
        strncpy(time_str, s + 5, 14);
        sprintf(msg, "Time String : %s\r\n", time_str);
        send_message();
        parse_date_str(time_str);
        parse_time_str(time_str + 8);

}
    else {
        SEND_INVALID();
    }

}




void set_alarm_count(unsigned char *s, uint8_t type) {
    uint8_t len = strlen(s);
    unsigned char time_str[100];
    memset(time_str, 0, sizeof(time_str));
    SEND_VALID("set alarm / count ");

    uint8_t offset = type == 1 ? 6 : 10;
    if (len >= offset + 6) {
        SEND_VALID("time length");

        strncpy(time_str, s + offset, 6);
        sprintf(msg,"Time String :%s\r\n", time_str);
        send_message();
        parse_time_str(time_str);
    }
    else {
        SEND_INVALID();
    }

}

void list(int type) {
    if (type == 1) {
        SEND_VALID("list alarm");
    }
    else {
        SEND_VALID("list countdown");
    }

}

void cancel(unsigned char *s, int type) {
    uint8_t len = strlen(s);
    unsigned char time_str[100];
    memset(time_str, 0, sizeof(time_str));
    SEND_VALID("cancel alarm / count ");

    uint8_t offset = type == 0 ? 6 : 10;
    if (len >= offset + 1) {
        strncpy(time_str, s + offset, 1);
        sprintf(msg, "ID : %s\r\n", time_str);
        send_message();
    }
    else {
        SEND_INVALID();
    }

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
                    int i;
                    for (i = 0; i < 3; i++) {
                        if (parse_literal(p, set_comm[i])) {
                            if(i == 0) {
                                set_time(p);
                            }
                            else {
                                set_alarm_count(p, i);
                            }
                            break;
                        }
                    }
                    if (i == 3) {
                        SEND_INVALID();
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
                    int i = 0;
                    for (i = 0 ; i < 2; i++) {
                        if (parse_literal(p, lc_comm[i])) {
                            list(i);
                            break;
                        }
                    }
                    if (i == 2) {
                        SEND_INVALID();
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
                    int i = 0;
                    for (i = 0 ; i < 2 ; i++) {
                        if (parse_literal(p, lc_comm[i])) {
                            cancel(p, i);
                            break;
                        }
                    }
                    if (i == 2) {
                        SEND_INVALID();
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

