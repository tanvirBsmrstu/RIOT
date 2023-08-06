
#include "tls_api.h"


app_ctx_t app_ctx;
CallbackGetSocketFromAppCtx cb_socket_from_appCtx;

void set_app_ctx(app_ctx_t _app_ctx){
    app_ctx = _app_ctx;
    return;
}

void set_cb_socket_from_appCtx(CallbackGetSocketFromAppCtx _get_socket_from_appCtx){
    cb_socket_from_appCtx = _get_socket_from_appCtx;
    return;
}

app_ctx_t get_app_ctx(void){
    return app_ctx;
}

CallbackGetSocketFromAppCtx get_cb_socket_from_appCtx(void){
    return cb_socket_from_appCtx;
}

