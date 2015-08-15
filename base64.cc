#include <openssl/md5.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#include "base64.h"
#include "logger.h"

std::string Base64::encode() const
{
    std::string result;
    BIO *bmem, *b64;
    BUF_MEM *bptr;

    b64  = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());

    if (b64 == nullptr || bmem == nullptr)
        EXCEPTION("BIO_new() failed");

    b64 = BIO_push(b64, bmem);
    BIO_write(b64, &m_content[0], m_content.size());
    (void)BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    result.reserve(bptr->length);
    result.insert(0, bptr->data, bptr->length-1);
    result[bptr->length-1] = '\0';

    BIO_free_all(b64);

    return result;
}
