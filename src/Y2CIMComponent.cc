
#include <y2util/y2log.h>
#include <y2/Y2Namespace.h>
#include <y2/Y2Component.h>

#include "Y2CIMComponent.h"
#include "CIMFunctions.h"

Y2CIMComponent::Y2CIMComponent () : Y2Component ()
    ,m_functions(NULL)
{
}

Y2Namespace *Y2CIMComponent::import (const char* name)
{
    if ( strcmp (name, "CIM") == 0)
    {
	if (m_functions == NULL)
	{
	    m_functions = new CIMFunctions();
	}

	return m_functions;
    }

    return NULL;
}

Y2CIMComponent* Y2CIMComponent::m_instance = NULL;

Y2CIMComponent* Y2CIMComponent::instance ()
{
    if (m_instance == NULL)
    {
        m_instance = new Y2CIMComponent ();
    }

    return m_instance;
}

