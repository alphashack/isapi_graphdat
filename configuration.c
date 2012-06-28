/********************************************************************************
 * configuration.c
 *
 */
#include "common.h"
#include "configuration.h"

#define ISAPI_GRAPHDAT_DEFAULT_SOCKET_PORT "26873"
#define CONFIG_FILE "graphdat_filter.conf"

extern gd_config_t CONFIG;
extern HMODULE MODULE_HANDLE;

typedef struct
{
	bool agent_request_socket_config;
} gd_config_validation; 

bool validate_config(gd_config_validation * config_validation);
bool init_validation(gd_config_validation * config_validation);

/****************************************
 * free_config
 */
void config_free(
	void)
{
	free(CONFIG.agent_request_socket_config);
}

char * skipws(
	char *psz)
{
	while(*psz && isspace(*psz))
		psz++;

	return psz;
}

bool parse_conf_line(
	char *	psz,
	char **	kv) // array of two
{
	psz = skipws(psz);

	kv[0] = psz;

	while(*psz && *psz != ':')
		psz++;

	if (*psz)
	{
		*psz++ = '\0';
		kv[1] = skipws(psz);

		// Cut extra from end
		while(*++psz);

		while(isspace(*--psz))
			*psz = '\0';

		return true;
	}
	else
		return false;
}

/****************************************
 * config_init
 */
bool config_init(
	void)
{
	FILE *fh;
	char buffer[1025];

	gd_config_validation validation;
	if (!init_validation(&validation))
		return false;

    CONFIG.agent_request_socket_config = (char *)malloc(strlen(ISAPI_GRAPHDAT_DEFAULT_SOCKET_PORT));
	strcpy(CONFIG.agent_request_socket_config, ISAPI_GRAPHDAT_DEFAULT_SOCKET_PORT);

	char * path;

	char exepath[MAX_PATH + 1];
	if (GetModuleFileNameA(MODULE_HANDLE, exepath, MAX_PATH) != 0)
	{
		char * psz = strrchr(exepath, '\\') + 1;
		
		strcpy(psz, CONFIG_FILE);

		path = exepath;
	}
	else
	{
		return false;
	}

	if ((fh = fopen(path, "r")) == NULL)
	{
		return false;
	}

	while (fgets(buffer, 1024, fh) != NULL)
	{
		char *args[2];
		if (!parse_conf_line(buffer, args))
			continue;

		if (strcasecmp(args[0], "port") == 0)
		{
			free(CONFIG.agent_request_socket_config);
			CONFIG.agent_request_socket_config = (char *)malloc(strlen(args[1]));
			strcpy(CONFIG.agent_request_socket_config, args[1]);
			validation.agent_request_socket_config = true;
		}
	}

	fclose(fh);

	return validate_config(&validation);
}


/****************************************
 * sets everthing to false;
 */
bool init_validation(gd_config_validation * config_validation)
{
	config_validation->agent_request_socket_config = false;

	return true;
}

/****************************************
 * makes sure a config_validation is
 * all good
 */
bool validate_config(gd_config_validation * config_validation)
{
	return (
		config_validation->agent_request_socket_config
	);
}
