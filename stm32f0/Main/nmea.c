#include "nmea.h"
#include "usart.h"
#include "main.h"

unsigned int isEnd(char ch){
    return ((ch == 0) || (ch == '\r') || (ch == '\n'));
}

void initializeNMEAGLLStructure(NMEA_GLLStructure *data){
	data->latitude = 0;
	data->longtitude = 0;
	data->ew = 0;
	data->ns = 0;
	data->time = 0;
	data->status = 0;
	data->checksum = 0;
}

float stringToFloat(char *s){
    int i = 0,j, temp;
    double k = 1;
    int dotPoint = 0;
    double result = 0;
    while(*(s+i) != 0 && *(s+i) != '.' && ++i);
    dotPoint = i;
    i = 0;
    while(*(s+i) != 0){
        temp = *(s+i) - '0';
        if(*(s+i) == '.'){
            temp = 0;
        }
        if(dotPoint > 0){
            for(j=1; j<dotPoint; j++){
                k *= 10.0;
            }
        } else if(dotPoint < 0){
            for(j=0; j>dotPoint; j--){
                k /= 10.0;
            }
        }
        result += temp * k;
        i++;
        dotPoint--;
        k=1;
    }
    return result;
}

void NMEA_SetByIndex(NMEA_GLLStructure *data, int i, char *s){
    switch(i){
        case 0:
            data->latitude = stringToFloat(s);
            break;
        case 1:
            data->ns = *s;
            break;
        case 2:
            data->longtitude = stringToFloat(s);
            break;
        case 3:
            data->ew = *s;
            break;
        case 4:
            data->time = stringToFloat(s);
            break;
        case 5:
            break;
        case 6:
            data->status = *s;
            break;
        case 7:
            data->checksum = stringToFloat(s);
            break;
    }
}

void autoSetNMEAStructrue(char *s, int *i, NMEA_GLLStructure *data){
    char toMatch[] = "$GPGLL";
    int toMatchPos = 0;
    int commaCount = 0;
    char dataBuffer[16] = "";
    int j = 0, k = 0;
    char flagOK = 0;
	DMA_DeInit(DMA1_Channel3);
    while(*(s+*i) != 0 && !flagOK){
        if(*(s+*i) == toMatch[toMatchPos]){
            toMatchPos++;
            if(toMatchPos > 5){
                (*i) += 2;
                while(commaCount < 8){
                    while(*(s+*i) != ',' && *(s+*i) != '*' && !isEnd(*(s+*i))){
                        dataBuffer[j++] = *(s+(*i)++);
                    }
                    dataBuffer[j] = '\0';
                    NMEA_SetByIndex(data, k++, dataBuffer);
                    commaCount++;
                    (*i)++;
                    j = 0;
                }
				flagOK = 1;
                toMatchPos = 0;
            }
        } else{
            toMatchPos = 0;
        }
        (*i)++;
    }
	USART1_RxDMA_Restart((uint32_t)USART1_DataBuffer);
}
