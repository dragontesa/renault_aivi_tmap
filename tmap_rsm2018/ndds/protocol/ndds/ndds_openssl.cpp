// ndds_openssl.cpp

#include "ndds_openssl.h"
#include "ndds_base_def.h"
#include "ndds_tmap_def.h"
#include "debug.h"

#ifdef __ECM__
#define ENGINE_ID         "sdc-ecm"
#endif // __ECM__




int ndds_ssl_open_and_connect(const char *hostname, const int port, SSL_VARS *vars, bool useECMEngine, int& retryCount) {
#ifndef __MM2018__
    // load all error strings
    ERR_load_crypto_strings();
    ERR_load_SSL_strings();
    SSL_load_error_strings();

    // init ssl lib
    SSLeay_add_ssl_algorithms();    // same the SSL_library_init();

    OPENSSL_config(NULL);
#endif

#if 1
    if (useECMEngine == true) {
        vars->ecm = init_engine();
        if (vars->ecm == nullptr)
        {
            NddsExternalLog(E_DEBUG, "ndds_openssl :: init_engine error");
            //fprintf(stderr, "init_engine error\n");
            //ERR_print_errors_fp(stderr);
            return NDDSERR_SSL_ECM_INIT;
        }
    }

    NddsExternalLog(E_DEBUG, "ndds_openssl :: connection logic start...");
    // SSL context for certifying
    vars->ctx = SSL_CTX_new(TLSv1_2_client_method());
    NddsExternalLog(E_DEBUG, "ndds_openssl :: SSL_CTX_new = 0x%x", (void*)vars->ctx);
    if (vars->ctx == nullptr) {
        return NDDSERR_SSL_CTX;
    }

    vars->cctx = SSL_CONF_CTX_new();
    NddsExternalLog(E_DEBUG, "ndds_openssl :: SSL_CONF_CTX_new = 0x%x", (void*)vars->cctx);
    if (vars->cctx == nullptr) {
        return NDDSERR_SSL_CTX;
    }
    SSL_CONF_CTX_set_flags(vars->cctx, SSL_CONF_FLAG_CLIENT);
    SSL_CONF_CTX_set_flags(vars->cctx, SSL_CONF_FLAG_FILE);
    SSL_CONF_CTX_set_ssl_ctx(vars->cctx, vars->ctx);
    NddsExternalLog(E_DEBUG, "ndds_openssl :: SSL_CONF_CTX_set_ssl_ctx = 0x%x, 0x%x", (void*)vars->ctx, (void*)vars->cctx);

    for (int ii = 0; ii < sk_CONF_VALUE_num(vars->sect); ii++) {
        vars->cnf = sk_CONF_VALUE_value(vars->sect, ii);
        int ret = SSL_CONF_cmd(vars->cctx, vars->cnf->name, vars->cnf->value);
        if (ret > 0)
            continue;

        if (ret != -2) {
            NddsExternalLog(E_DEBUG, "ndds_openssl :: Error processing :: cnf->name = %s, cnf->value = %s", vars->cnf->name, vars->cnf->value);
            //fprintf(stderr, "ndds_openssl Error processing %s = %s\n", vars->cnf->name, vars->cnf->value);
            //ERR_print_errors_fp(stderr);
            return NDDSERR_SSL_ECM_CONF;
        }
        if (!strcmp(vars->cnf->name, "Connect")) {
            vars->connect_str = vars->cnf->value;
        } else {
            NddsExternalLog(E_DEBUG, "ndds_openssl :: Unknown configuration option :: cnf->name = %s", vars->cnf->name);
            //fprintf(stderr, "ndds_openssl Unknown configuration option %s\n", vars->cnf->name);
            return NDDSERR_SSL_ECM_CONF_OPT;
        }
    }

    if (useECMEngine == true) {
        if (!set_engine_related_conf(vars->cctx)) {
            NddsExternalLog(E_DEBUG, "ndds_openssl :: set_engine_related_conf error");
            //fprintf(stderr, "set_engine_related_conf error\n");
            //ERR_print_errors_fp(stderr);
            return NDDSERR_SSL_ECM_REL_CONF;
        }

        if (!SSL_CONF_CTX_finish(vars->cctx)) {
            NddsExternalLog(E_DEBUG, "ndds_openssl :: Finish error");
            //fprintf(stderr, "ndds_openssl Finish error\n");
            //ERR_print_errors_fp(stderr);
            return NDDSERR_SSL_ECM_CONF_FINISH;
        }

        if (!set_engine_to_ctx(vars->ctx, vars->ecm)) {
            NddsExternalLog(E_DEBUG, "ndds_openssl :: set_engine_to_ctx error");
            //fprintf(stderr, "ndds_openssl set_engine_to_ctx error\n");
            //ERR_print_errors_fp(stderr);
            return NDDSERR_SSL_ECM_TO_CTX;
        }
    } else {
        SSL_CTX_set_cipher_list(vars->ctx, "ECDHE-RSA-AES256-GCM-SHA384");
    }

    vars->sbio = BIO_new_ssl_connect(vars->ctx);
    NddsExternalLog(E_DEBUG, "ndds_openssl :: sbio address = 0x%x", (void*)vars->sbio);
    BIO_get_ssl(vars->sbio, &vars->ssl);
    NddsExternalLog(E_DEBUG, "ndds_openssl :: ssl address = 0x%x", (void*)vars->ssl);
    if (!vars->ssl) {
        NddsExternalLog(E_DEBUG, "ndds_openssl :: Can't locate SSL pointer");
        //fprintf(stderr, "ndds_openssl Can't locate SSL pointer\n");
        return NDDSERR_SSL_ECM_BIO_GET_SSL;
    }

    BIO_set_nbio(vars->sbio, 1);
    SSL_set_tlsext_host_name(vars->ssl, hostname);
    NddsExternalLog(E_DEBUG, "ndds_openssl :: set tlsexthostname = %s", hostname);
    char hostnameport[1024];
    sprintf(hostnameport, "%s:%d", hostname, port);
    BIO_set_conn_hostname(vars->sbio, hostnameport);
    NddsExternalLog(E_DEBUG, "ndds_openssl :: set hostnameport = %s", hostnameport);

    while (BIO_do_connect(vars->sbio) <= 0)
    {
        if (retryCount >= RETRY_COUNT)// || BIO_should_retry(vars->sbio) <= 0)
        {
            NddsExternalLog(E_DEBUG, "ndds_openssl :: connect error, no more retry!!! retryCount = %d", retryCount);
            return NDDSERR_SSL_ECM_CONNECT;
        }
        retryCount++;
        delay(RETRY_DELAY);
        NddsExternalLog(E_DEBUG, "ndds_openssl :: connect error1!!! retryCount = %d", retryCount);
    }

    if (BIO_do_handshake(vars->sbio) <= 0) {
        NddsExternalLog(E_DEBUG, "ndds_openssl :: handshake failed!!!");
        return NDDSERR_SSL_ECM_HANDSHAKE;
    }

    if (useECMEngine == false) {
        SSL_get_cipher(vars->ssl);
    }

    return NDDSERR_SUCCESS;
#else // 0

    unsigned long time;
    int nRetryCount = 0;
    int err = -1;

    memset(&vars->server_addr, 0x00, sizeof(vars->server_addr));

    if((vars->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return NDDSERR_SOCKETCREATE;

//    progress = 2;
//    delay(TIME_DELAY);

    if (useECMEngine == true) {
        vars->ecm = init_engine();
        if (vars->ecm == nullptr)
        {
            //fprintf(stderr, "init_engine error\n");
            //ERR_print_errors_fp(stderr);
            return NDDSERR_SSL_ECM_INIT;
        }
    }

    // SSL context for certifying
    vars->ctx = SSL_CTX_new(TLSv1_2_client_method());
    if (vars->ctx == nullptr)
        return NDDSERR_SSL_CTX;

    vars->cctx = SSL_CONF_CTX_new();
    if (vars->cctx == nullptr)
        return NDDSERR_SSL_CTX;

    SSL_CONF_CTX_set_flags(vars->cctx, SSL_CONF_FLAG_CLIENT);
    SSL_CONF_CTX_set_flags(vars->cctx, SSL_CONF_FLAG_FILE);
    SSL_CONF_CTX_set_ssl_ctx(vars->cctx, vars->ctx);

    for (int ii = 0; ii < sk_CONF_VALUE_num(vars->sect); ii++) {
        vars->cnf = sk_CONF_VALUE_value(vars->sect, ii);
        int ret = SSL_CONF_cmd(vars->cctx, vars->cnf->name, vars->cnf->value);
        if (ret > 0)
            continue;

        if (ret != -2) {
            //fprintf(stderr, "ndds_openssl Error processing %s = %s\n", vars->cnf->name, vars->cnf->value);
            //ERR_print_errors_fp(stderr);
            return NDDSERR_SSL_ECM_CONF;
        }
        if (!strcmp(vars->cnf->name, "Connect")) {
            vars->connect_str = vars->cnf->value;
        } else {
            //fprintf(stderr, "ndds_openssl Unknown configuration option %s\n", vars->cnf->name);
            return NDDSERR_SSL_ECM_CONF_OPT;
        }
    }

    if (useECMEngine == true) {
        if (!set_engine_related_conf(vars->cctx)) {
            //fprintf(stderr, "set_engine_related_conf error\n");
            //ERR_print_errors_fp(stderr);
            return NDDSERR_SSL_ECM_REL_CONF;
        }

        if (!SSL_CONF_CTX_finish(vars->cctx)) {
            //fprintf(stderr, "ndds_openssl Finish error\n");
            //ERR_print_errors_fp(stderr);
            return NDDSERR_SSL_ECM_CONF_FINISH;
        }

        if (!set_engine_to_ctx(vars->ctx, vars->ecm)) {
            //fprintf(stderr, "ndds_openssl set_engine_to_ctx error\n");
            //ERR_print_errors_fp(stderr);
            return NDDSERR_SSL_ECM_TO_CTX;
        }
    } else {
        SSL_CTX_set_cipher_list(vars->ctx, "ECDHE-RSA-AES256-GCM-SHA384");
    }

    struct hostent* phostent;
    phostent = gethostbyname(hostname);
    if(phostent == nullptr)
        return NDDSERR_SOCKETHOST;

    // 서버 접속
    //////////////////////////////////////////////////////////////////////////
    vars->server_addr.sin_family = AF_INET;
     //주소 체계를 AF_INET 로 선택
    //server_addr.sin_addr.s_addr = inet_addr(m_szHost);
    memcpy(&vars->server_addr.sin_addr.s_addr, phostent->h_addr_list[0], static_cast<size_t>(phostent->h_length));
    //32비트의 IP주소로 변환
    vars->server_addr.sin_port = htons(static_cast<uint16_t>(port));

    while(connect(vars->socket, reinterpret_cast<struct sockaddr *>(&vars->server_addr), sizeof(vars->server_addr)) < 0)
    {
        DBGPRINT(_T("******************************** Connect #2 Time(%d)\n"), GetTickCount() - time);
        nRetryCount++;
        if(nRetryCount >= RECONNECT_COUNT)
        {
            return NDDSERR_SOCKETCONNECT;
        }
        time = GetTickCount();
        DBGPRINT(_T("******************************** ReConnect #1(%d)\n"), nRetryCount);
    }
    DBGPRINT(_T("******************************** Connect #3 Time(%d)\n"), GetTickCount()-time);

    // Now we have TCP conncetion. Start SSL negotiation.
    // create a new SSL structure for a connection
    vars->ssl = SSL_new(vars->ctx);
    if (vars->ssl == nullptr)
        return NDDSERR_SSL_NEW;

    // connect the SSL object with a file descriptor
    SSL_set_fd(vars->ssl, vars->socket);

    // initiate the TLS/SSL handshake with an TLS/SSL server
    err = SSL_connect(vars->ssl);
    if (err == -1)
        return NDDSERR_SSL_CONNECT;

    /*
     * Following two steps are optional and not required for
     * data exchange to be successful.
     */

    //printf ("SSL connection using %s\n", SSL_get_cipher (vars->ssl)); fflush(stdout);

    SSL_get_cipher(vars->ssl);

    // Get server's certificate (note: beware of dynamic allocation) - opt
    vars->server_cert = SSL_get_peer_certificate(vars->ssl);
    if (vars->server_cert == nullptr)
        return NDDSERR_SSL_CERT;

    //print_x509(vars->ssl);

/*
    printf ("Server certificate:\n");
    str = X509_NAME_oneline (X509_get_subject_name (vars->server_cert),0,0);
    CHK_NULL(str);
    printf ("\t subject: %s\n", str);
    free (str);

    str = X509_NAME_oneline (X509_get_issuer_name  (vars->server_cert),0,0);
    CHK_NULL(str);
    printf ("\t issuer: %s\n", str);
    free (str);
*/

    /*
     * We could do all sorts of certificate verification stuff here before
     * deallocating the certificate.
     */
    X509_free(vars->server_cert);

    return NDDSERR_SUCCESS;
}

void ndds_ssl_close(SSL_VARS *vars)
{
    if (vars->server_cert != nullptr)
    {
        X509_free(vars->server_cert);
        vars->server_cert = nullptr;
    }
#endif // 0
}

void ndds_ssl_close(SSL_VARS *vars)
{
    if (vars->ctx != nullptr)
    {
        SSL_CTX_free(vars->ctx);
        vars->ctx = nullptr;
    }

    if (vars->cctx != nullptr)
    {
        SSL_CONF_CTX_free(vars->cctx);
        vars->cctx = nullptr;
    }

    if (vars->sbio != nullptr)
    {
        SSL_shutdown(vars->ssl);
        BIO_ssl_shutdown(vars->sbio);
        BIO_free_all(vars->sbio);
        vars->sbio = nullptr;
        vars->ssl = nullptr;
    }
}


ENGINE *init_engine(void)
{
    ENGINE *e = nullptr;

#ifdef __ECM__
    /* get dynamic sdc-ecm engine */
    e = ENGINE_by_id(ENGINE_ID);
    if (!e)
        return nullptr;
#endif __ECM__
    return e;
}

int set_engine_related_conf(SSL_CONF_CTX *cctx) {
    SSL_CONF_cmd(cctx, "Protocol", "-ALL,TLSv1.2");
    SSL_CONF_cmd(cctx, "Curves", "prime256v1:secp384r1:secp521r1");
    SSL_CONF_cmd(cctx, "CipherString", "ECDHE+AESGCM:DHE+AESGCM:DHE+AES:ECDHE+AES:!DSS:!SHA");
    return SSL_CONF_cmd(cctx, "SignatureAlgorithms", "RSA+SHA256");
}

int SDCECM_verify_callback(X509_STORE_CTX *ctx, void *arg) {
#ifdef __ECM__
    X509 *serv_cert = ctx->cert;

    if (arg == nullptr)
        return 3;

    //  ganesh's code
    if (!ENGINE_ctrl_cmd(static_cast<ENGINE*>(arg), "ECM_CHECK_CERT", 0, serv_cert, nullptr, 0)) {
        //ERR_print_errors_fp(stderr);
        return 2;
    }

    //ERR_print_errors_fp(stderr);
#endif __ECM__
    return 0;
}

int set_engine_to_ctx(SSL_CTX *ctx, ENGINE *e) {
    SSL_CTX_set_cert_verify_callback(ctx, SDCECM_verify_callback, static_cast<void*>(e));
    return SSL_CTX_set_client_cert_engine(ctx, e);
}
