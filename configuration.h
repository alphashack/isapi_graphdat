/********************************************************************************
 * configuration.h
 */

#ifndef ISAPI_GRAPHDAT_CONFIGURATION_H
#define ISAPI_GRAPHDAT_CONFIGURATION_H

typedef struct
{
	// From graphdat-filter.dll.config
	char * agent_request_socket_config;
} gd_config_t;

void config_free(void);
bool config_init(void);

#endif // ISAPI_GRAPHDAT_CONFIGURATION_H
