#include <stdio.h>
#include <dotenv.h>

int main(void) {
	env_load(".", false);

	char *ws_url = getenv("TREASURY_WS_URL");
	char *ws_auth = getenv("TREASURY_AUTH_TOKEN");
	if (!ws_url || !ws_auth) {
		printf("Config not found\n");
		return 1;
	}

	printf("%s\n%s\n", ws_url, ws_auth);

	return 0;
}
