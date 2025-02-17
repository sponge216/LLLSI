#include "network.h"

static WSADATA wsaData;
int res = WSAStartup(MAKEWORD(2, 2), &wsaData);