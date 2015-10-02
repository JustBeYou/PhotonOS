#ifndef _kernel_class_h
#define _kernel_class_h

class KernelClass {
    private:
        int ID_number;
        char version[30];
        char version_name[50];
    public:
        KernelClass();
        ~KernelClass();
        void setID(int ID);
        void setVersion(const char *version);
        int getID();
        char *getVersion();
};

#endif
