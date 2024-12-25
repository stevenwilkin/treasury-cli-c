#include <stdio.h>
#include <dotenv.h>
#include <jansson.h>

char *auth_json(char *auth) {
	json_t *obj = json_object();

	if(json_object_set_new(obj, "auth", json_string(auth)) != 0) {
		return NULL;
	}

	char *json = json_dumps(obj, 0);
	if(json == NULL) {
		return NULL;
	}

	json_decref(obj);

	return json;
}

int main(void) {
	env_load(".", false);

	char *ws_url = getenv("TREASURY_WS_URL");
	char *ws_auth = getenv("TREASURY_AUTH_TOKEN");
	if (!ws_url || !ws_auth) {
		printf("Config not found\n");
		return 1;
	}

	char *auth_msg = auth_json(ws_auth);

	char host[256];
	int port;
	char path[256];

	int n = sscanf(ws_url, "%*[^:]://%[^:]:%d%s", host, &port, path);
	if(n != 3) {
		printf("Error parsing url\n");
		return 1;
	}

	printf("%s\n", host);
	printf("%d\n", port);
	printf("%s\n", path);

	return 0;
}
