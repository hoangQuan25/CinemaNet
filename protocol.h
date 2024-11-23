// protocol.h
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdbool.h>

bool process_client_message(const char *message, char *response);

#endif // PROTOCOL_H
