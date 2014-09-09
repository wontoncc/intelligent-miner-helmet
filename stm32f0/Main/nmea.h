#ifndef __NMEA_H
#define __NMEA_H

typedef struct{
    float latitude;
    char ns;
    float longtitude;
    char ew;
    float time;
    char status;
    int checksum;
} NMEA_GLLStructure;

unsigned int isEnd(char ch);
float stringToFloat(char *s);
void NMEA_SetByIndex(NMEA_GLLStructure *data, int i, char *s);
void autoSetNMEAStructrue(char *s, int *i, NMEA_GLLStructure *data);
void initializeNMEAGLLStructure(NMEA_GLLStructure *data);

#endif
