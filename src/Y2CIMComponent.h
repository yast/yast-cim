
#include <y2/Y2Component.h>

class CIMFunctions;
class Y2Namespace;

class Y2CIMComponent : public Y2Component {
public:
    Y2CIMComponent ();

    virtual Y2Namespace *import (const char* name);

    virtual string name () const { return "CIM";}
    
    static Y2CIMComponent* instance();
    
private:
    static Y2CIMComponent* m_instance;
    CIMFunctions* m_functions;
};

