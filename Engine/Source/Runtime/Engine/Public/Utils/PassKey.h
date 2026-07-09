#ifndef MANGO_PASSKEY_H
#define MANGO_PASSKEY_H

namespace tomato
{
    template<typename T>
    class PassKey
    {
    private:
        PassKey() = default;
        friend T;
    };
}

#endif //MANGO_PASSKEY_H