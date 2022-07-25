#ifndef _STATUS_H_
#define _STATUS_H_

void Status_Init();
void Keep_Status(unsigned char keep);
unsigned char Get_Keep_Status();
void Update_Status(unsigned char pin, unsigned char status);
void Save_Status();
unsigned char Get_Status(unsigned char pin);

#endif