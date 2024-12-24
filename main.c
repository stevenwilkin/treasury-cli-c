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
	printf("%s\n", auth_msg);

	return 0;
}
