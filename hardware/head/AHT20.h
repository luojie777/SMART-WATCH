#ifndef AHT20_H
#define AHT20_H

#include "main.h"

void AHT20_Init(void);
void AHT20_Read(float *Temperature, float *Humidity);

#endif