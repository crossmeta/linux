#ifdef _M_IX86
#include <linux/i386-bitops.h>
#elif defined(_M_AMD64)
#include <linux/amd64-bitops.h>
#else
#error "bitops.h needed for your platform"
#endif 
