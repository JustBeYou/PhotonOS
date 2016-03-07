#include <kernel/kernel_class.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <kernel/io.h>

#ifdef __cplusplus
}
#endif

void KernelClass::setID(const int ID)
{
    this->ID_number = ID;
}

void KernelClass::setVersion(const char *version)
{
    memcpy(this->version, version, strlen(version) + 1);
}

int KernelClass::getID()
{
    return this->ID_number;
}

char *KernelClass::getVersion()
{
    return version;
}

KernelClass::KernelClass()
{
    printk("C++ support enabled. Kernel Class created.\n");
}

KernelClass::~KernelClass()
{
    printk("Kernel Class destroyed. Kernel go down.");
}
