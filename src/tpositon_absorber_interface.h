#ifndef __TPOSITON_ABSORBER_INTERFACE_H
#define __TPOSITON_ABSORBER_INTERFACE_H

class Tpositon_absorberInterface
{
public:
    Tpositon_absorberInterface() {}
    virtual ~Tpositon_absorberInterface() {}


private:
    Tpositon_absorberInterface(const Tpositon_absorberInterface& source);
    void operator = (const Tpositon_absorberInterface& source);
};


#endif // __TPOSITON_ABSORBER_INTERFACE_H
