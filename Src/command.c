//
// Created by Mingji Han on 2019/12/14.
//
#include <stdio.h>
#include <stdint-gcc.h>
#include <string.h>
#include "usart.h"
#include "main.h"
#include "../Inc/command.h"
#define SEND_VALID(s)   do {\
                            sprintf(msg, "Valid command %s\r\n", s);\
                            /*send_message()*/; \
                        }while(0);

#define SEND_INVALID()   do {\
                            sprintf(msg, "Invalid command\r\n");\
                            send_message(); \
                        }while(0);

extern UART_HandleTypeDef huart1;
extern char msg[100];
extern int setting_values[6];
int date_to_set[3];
int time_to_set[3];

extern my_time alarm[4];
extern my_time countdown[4];

extern uint32_t time_in_sec;


const unsigned char* set_comm[] = {"time", "alarm", "countdown", "hour", "minute","second","year","month","day"};
const unsigned char* lc_comm[] = {"alarm", "countdown"};

void send_message() {
    HAL_UART_Transmit(&huart1, (uint8_t *) msg, strlen(msg), HAL_MAX_DELAY);
}


// parse
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
uint8_t parse_date_str(unsigned char *s) {
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
//        sprintf(msg, "%d-%d-%d\r\n", time[0], time[1], time[2]);
//        send_message();
    }

    if (day_is_valid(time[0], time[1], time[2])!=1) {
        sprintf(msg, "date invalid, retry\r\n");
        send_message();
        return 1;
    } else {
        // date valid
        date_to_set[0] = time[0];
        date_to_set[1] = time[1];
        date_to_set[2] = time[2];
        return 0;
    }

}

uint8_t time_is_valid(uint8_t hour, u_int8_t minute, u_int8_t second){
    if( (0<=hour && hour<=23) && (0<=minute && minute<=59) && (0<=second && second<=59)  ){
        return 1;
    } else {
        return 0;
    }
}

uint8_t parse_time_str(unsigned char *s) {
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
//        sprintf(msg, "%d:%d:%d\r\n", time[0], time[1], time[2]);
//        send_message();
    }

    if(time_is_valid(time[0], time[1], time[2])!=1){
        sprintf(msg, "time invalid, retry~!\r\n");
        send_message();
        return 1;
    } else{
        time_to_set[0] = time[0];
        time_to_set[1] = time[1];
        time_to_set[2] = time[2];
        return 0;
    }
}


// set
void set_time(unsigned char *s) {
    uint8_t len = strlen(s);
    unsigned char time_str[100];
    memset(time_str, 0, sizeof(time_str));
    SEND_VALID("set time");
    if (len >= 5 + 14) {
        SEND_VALID("time length");
        strncpy(time_str, s + 5, 14);
//        sprintf(msg, "Time String : %s\r\n", time_str);
//        send_message();
        uint8_t ret1 = parse_date_str(time_str);
        uint8_t ret2 = parse_time_str(time_str + 8);
        if( (ret1==0) && (ret2==0) ){ // both date, time valid
            setting_values[0] = date_to_set[0];
            setting_values[1] = date_to_set[1];
            setting_values[2] = date_to_set[2];
            setting_values[3] = time_to_set[0];
            setting_values[4] = time_to_set[1];
            setting_values[5] = time_to_set[2];
            save_set_value_back();
        }
}
    else {
        SEND_INVALID();
    }

}



// type 1: alarm, 2: countdown
void set_alarm_count(unsigned char *s, uint8_t type) {
    uint8_t len = strlen(s);
    unsigned char time_str[100];
    memset(time_str, 0, sizeof(time_str));
    SEND_VALID("set alarm / count ");

    uint8_t offset = type == 1 ? 6 : 10;
    if (len >= offset + 8) {
        SEND_VALID("time length");
        uint8_t id = s[offset] - '0';

//        sprintf(msg,"set_id = %d\r\n", id);
//        send_message();

        if(!(0<=id && id<=3)){ // id valid check
            sprintf(msg,"invalid id\r\n");
            send_message();
            SEND_INVALID();
            return;
        }

        strncpy(time_str, s + offset + 2, 6);
//        sprintf(msg,"Time String :%s\r\n", time_str);
//        send_message();
        uint8_t ret = parse_time_str(time_str);
        if(ret==0){ // valid
            if(type==1){ // alarm
                alarm[id].hour = time_to_set[0];
                alarm[id].minute = time_to_set[1];
                alarm[id].second = time_to_set[2];
            } else{ // countdown
                // do time conversion
                uint32_t countdown_value = time_to_set[0] * 60 * 60 + time_to_set[1] * 60 + time_to_set[2];
                uint32_t target = countdown_value + time_in_sec;
                if(target>86400) target -= 86400;

                uint8_t hour, minute, second;
                hour = target / (60 * 60);
                minute = target / 60  - hour*60;
                second = target % 60;

                countdown[id].hour = hour;
                countdown[id].minute = minute;
                countdown[id].second = second;
            }
        }
    }
    else {
        SEND_INVALID();
    }

}

// type: 0 alarm, 1 countdown
void list(int type) {
    if (type == 0) {
        SEND_VALID("list alarm");

        for(uint8_t i=0;i<4;i++){
            if(alarm[i].hour<24){ // enabled
                sprintf(msg, "Alarm #%d - %02d:%02d:%02d\r\n", i, alarm[i].hour, alarm[i].minute, alarm[i].second);
            } else { // disabled
                switch (alarm[i].hour){
                    case 255:
                        sprintf(msg, "Alarm #%d - [DISABLED]\r\n",i);
                        break;
                    case 254:
                        sprintf(msg, "Alarm #%d - [RINGING]\r\n",i);
                        break;
                }

            }
            send_message();
        }

    }
    else {
        SEND_VALID("list countdown");

        for(uint8_t i=0;i<4;i++){
            if(countdown[i].hour<24){ // enabled
                uint32_t countdown_value = countdown[i].hour * 60 * 60 + countdown[i].minute * 60 + countdown[i].second;
                uint32_t diff = countdown_value - time_in_sec;
                if(diff<0) diff += 86400;

                uint8_t hour, minute, second;
                hour = diff / (60 * 60);
                minute = diff / 60  - hour*60;
                second = diff % 60;

                sprintf(msg, "Countdown #%d - %02d:%02d:%02d remaining\r\n", i, hour, minute, second);
            } else { // disabled
                switch (countdown[i].hour){
                    case 255:
                        sprintf(msg, "Countdown #%d - [DISABLED]\r\n",i);
                        break;
                    case 254:
                        sprintf(msg, "Countdown #%d - [RINGING]\r\n",i);
                        break;
                }
            }
            send_message();
        }
    }
}

// type: 0 alarm, 1 countdown
void cancel(unsigned char *s, int type) {
    uint8_t len = strlen(s);
    unsigned char time_str[100];
    memset(time_str, 0, sizeof(time_str));
    SEND_VALID("cancel alarm / count ");

    uint8_t offset = type == 0 ? 6 : 10;
    if (len >= offset + 1) {
        strncpy(time_str, s + offset, 1);
        uint8_t id = time_str[0] - '0';
//        sprintf(msg, "ID : %d\r\n", id);
//        send_message();

        if(!(0<=id && id<=3)){
            sprintf(msg, "invalid id\r\n");
            send_message();
            SEND_INVALID();
            return;
        }

        if(type==0){ // alarm
            alarm[id].hour = 255;
        } else { // countdown
            countdown[id].hour = 255;
        }

    }
    else {
        SEND_INVALID();
    }

}

void set_rest(unsigned char *s, uint8_t type){
    freeze_values_for_setting(); // freeze values

    switch(type){
        case 3: // hour
            s += 4;
            break;
        case 4: // minute
            s += 6;
            break;
        case 5: // second
            s += 6;
            break;
        case 6: // year
            s += 4;
            break;
        case 7: // month
            s += 5;
            break;
        case 8: // day
            s += 3;
            break;
    }

    s++;

    uint32_t val = 0;


    if(type==6){ // year, 4 digit
        for(uint8_t i=0;i<4;i++){
            if(s[i]<'0' || s[i]>'9') break;
            val *= 10;
            val += s[i] - '0';
        }
    } else { // other, 2 digit
        for(uint8_t i=0;i<2;i++){
            if(s[i]<'0' || s[i]>'9') break;
            val = val * 10 + s[i] - '0';
        }
    }

    uint8_t valid_to_change = 0;

    switch(type){
        case 3: // hour
            setting_values[3] = val;
            break;
        case 4: // minute
            setting_values[4] = val;
            break;
        case 5: // second
            setting_values[5] = val;
            break;
        case 6: // year
            setting_values[0] = val;
            break;
        case 7: // month
            setting_values[1] = val;
            break;
        case 8: // day
            setting_values[2] = val;
            break;
    }

    switch(type){
        case 3:
        case 4:
        case 5:
            valid_to_change = time_is_valid(setting_values[3], setting_values[4], setting_values[5]);
            break;
        case 6:
        case 7:
        case 8:
            valid_to_change = day_is_valid(setting_values[0], setting_values[1], setting_values[2]);
            break;
    }

    if(valid_to_change==1){
        save_set_value_back();
    } else {
        sprintf(msg, "invalid value\r\n");
        send_message();
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
                    for (i = 0; i < 9; i++) {
                        if (parse_literal(p, set_comm[i])) {
                            if(i == 0) {
                                set_time(p);
                            }
                            else if (i==1 || i==2) {
                                set_alarm_count(p, i);
                            } else {
                                set_rest(p,i);
                            }
                            break;
                        }
                    }
                    if (i == 9) {
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

