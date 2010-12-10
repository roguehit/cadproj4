#ifndef CAD_PROJ4_H
#define CAD_PROJ4_H


#define MAX_SERVER    10
#define MAX_HALF_CONN 100
#define MAX_ATTACKER  100
#define FALSE         0x0
#define TRUE          0x1
#define SERVER        0x2
#define ATTACKER      0x3

struct __serverIP{
  /*Stores IP as a Decimal Number*/
  uint32_t ip;

  /*Stores IP in a 4 byte buffer in Human readble format*/
  uint8_t buf[4];

  int reserved;

  /*Current Half-Open Connection Count*/
  int count;
}ServerIP[MAX_SERVER];

struct __attackerIP{

  uint32_t ip;

  uint8_t buf[4];

}AttackerIP[MAX_ATTACKER];

#endif
