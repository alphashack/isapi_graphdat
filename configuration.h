/********************************************************************************
 * configuration.h
 */

#ifndef ISAPI_GRAPHDAT_CONFIGURATION_H
#define ISAPI_GRAPHDAT_CONFIGURATION_H

typedef struct
{
	// From graphdat-filter.dll.config
	char * agent_request_socket_config;
	bool debug;
} gd_config_t;

bool config_init(void);
void config_free(void);

#endif // ISAPI_GRAPHDAT_CONFIGURATION_H
