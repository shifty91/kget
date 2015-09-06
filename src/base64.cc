#include "base64.h"
#include "logger.h"
#include "ssl/ssl_wrapper.h"

std::string Base64::encode() const
{
    std::string result;
    BIOHandle b64(BIO_f_base64()), bmem(BIO_s_mem());

    b64.push(bmem);
    b64.write(m_content.data(), m_content.size());
    b64.flush();
    auto *bptr = b64.get_mem_ptr();

    result.reserve(bptr->length);
    result.insert(0, bptr->data, bptr->length-1);
    result[bptr->length-1] = '\0';

    b64.pop();

    return result;
}
