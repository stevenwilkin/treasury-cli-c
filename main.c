#include <stdio.h>
#include <dotenv.h>
#include <jansson.h>
#include <libwebsockets.h>

static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

static const struct lws_protocols protocols[] = {
	{ "lws-minimal-client", callback, 0, 0, 0, NULL, 0 },
	LWS_PROTOCOL_LIST_TERM
};

static const struct lws_extension extensions[] = {
	{
		"permessage-deflate",
		lws_extension_callback_pm_deflate,
		"permessage-deflate"
		 "; client_no_context_takeover"
		 "; client_max_window_bits"
	},
	{ NULL, NULL, NULL /* terminator */ }
};

static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
	switch (reason) {
	case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		printf("error: %s\n", in ? (char *)in : "(null)");
		return -1;

	case LWS_CALLBACK_CLIENT_RECEIVE:
		printf("%s\n", (char *)in);
		break;

	case LWS_CALLBACK_CLIENT_ESTABLISHED:
		printf("connected\n");
		break;

	case LWS_CALLBACK_CLIENT_CLOSED:
		printf("closed\n");
		return -1;

	default:
		break;
	}

	return 0;
}

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

	struct lws *wsi;
	struct lws_context *context;
	struct lws_client_connect_info i;
	struct lws_context_creation_info info;

	lws_set_log_level(LLL_ERR | LLL_WARN, lwsl_emit_syslog); // We don't need to see the notice messages

	memset(&i, 0, sizeof(i));
	memset(&info, 0, sizeof info);

	info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	info.port = CONTEXT_PORT_NO_LISTEN; /* we do not run any server */
	info.protocols = protocols;
	info.fd_limit_per_thread = 1 + 1 + 1;
	info.extensions = extensions;

	context = lws_create_context(&info);
	if (!context) {
		printf("error creating context\n");
		return 1;
	}

	i.context = context;
	i.port = port;
	i.address = host;
	i.path = path;
	i.host = i.host;
	i.origin = i.host;
	i.protocol = NULL;
	i.local_protocol_name = "lws-minimal-client";
	i.pwsi = &wsi;

	if (!lws_client_connect_via_info(&i)) {
		printf("error connecting\n");
		return 1;
	}

	while (1) {
		lws_service(context, 0);
	}

	lws_context_destroy(context);

	return 0;
}
