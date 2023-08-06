
#include "tls_api_types.h"

//int tls_server(int port);

typedef tls_api_socket_t*(*CallbackGetSocketFromAppCtx)(app_ctx_t);

void set_app_ctx(app_ctx_t _app_ctx);
void set_cb_socket_from_appCtx(CallbackGetSocketFromAppCtx);



app_ctx_t get_app_ctx(void);
CallbackGetSocketFromAppCtx get_cb_socket_from_appCtx(void);
