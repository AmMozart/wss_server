#ifndef ELECTRIC_GROUPS_H
#define ELECTRIC_GROUPS_H

#include <stdio.h>
#define MAX_ELECTRIC_GROUPS 10

typedef struct electric_group
{
  char name[10];
  uint8_t dmx_channel;
} electric_group_t;

electric_group_t electric_groups[MAX_ELECTRIC_GROUPS];

#endif /* ELECTRIC_GROUPS_H */