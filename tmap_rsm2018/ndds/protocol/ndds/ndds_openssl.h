// ndds_openssl.h

#ifndef __NDDS_OPENSSL_H
#define __NDDS_OPENSSL_H

#define __SSL_SOCKET__	// OpenSSL 적용여부

#ifdef __MM2018__
#define __ECM__
#endif // __MM2018__

#define NO_SYS_TYPES_H

#ifdef __ECM__
#include <openssl/engine.h>
#endif // __ECM__
#include <openssl/x509v3.h>
#include <openssl/crypto.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#ifdef _LINUX
#include <sys/time.h>
#include <unistd.h>
#include <netdb.h>
#endif

#ifdef WIN32
    #pragma comment(lib, "libssl32MDd.lib")
    #pragma comment(lib, "libcrypto32MDd.lib")
#endif // WIN32


struct SSL_VARS {
    SSL_CTX                 *ctx;
    SSL                     *ssl;
    BIO                     *sbio;
    SSL_CONF_CTX            *cctx;
    STACK_OF(CONF_VALUE)    *sect;
    CONF_VALUE              *cnf;
    ENGINE                  *ecm;
    const char              *connect_str;
//    X509                    *server_cert;

    SSL_VARS() {
        ctx = nullptr;
        ssl = nullptr;
        sbio = nullptr;
        cctx = nullptr;
        sect = nullptr;
        ecm = nullptr;
        connect_str = "";
//        server_cert = nullptr;
    }
};

int ndds_ssl_open_and_connect(const char *hostname, const int port, SSL_VARS *vars, bool useECMEngine, int& retryCount);
void ndds_ssl_close(SSL_VARS *vars);

ENGINE *init_engine(void);
int set_engine_related_conf(SSL_CONF_CTX *cctx);
int set_engine_to_ctx(SSL_CTX *ctx, ENGINE *e);

#endif // __NDDS_OPENSSL_H
