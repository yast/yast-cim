/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:	CIMFunctions.cc

   Author:	Stanislav Visnovsky <visnov@suse.cz>
   Maintainer:  Stanislav Visnovsky <visnov@suse.cz>

   Purpose:	CIMFunctions constructor, destructor and call handling
/-*/

#include <y2/Y2Function.h>
#include <ycp/y2log.h>
#include <ycp/YCPMap.h>
#include <ycp/YCPBoolean.h>
#include <ycp/YCPInteger.h>

#include "CIMFunctions.h"

#include "openwbem/OW_CIMClient.hpp"
#include "openwbem/OW_Array.hpp"
#include "openwbem/OW_CIMClass.hpp"
#include "openwbem/OW_CIMDataType.hpp"
#include "openwbem/OW_CIMObjectPath.hpp"
#include "openwbem/OW_CIMInstance.hpp"
#include "openwbem/OW_CIMProperty.hpp"
#include "openwbem/OW_CIMValue.hpp"
#include "openwbem/OW_CIMException.hpp"
#include "openwbem/OW_CIMInstanceEnumeration.hpp"
#include "openwbem/OW_CIMObjectPathEnumeration.hpp"

using namespace OpenWBEM;

class Y2CIMFunction: public Y2Function 
{
    unsigned int m_position;
    CIMFunctions* m_instance;
    YCPValue m_param1;
    YCPValue m_param2;
    YCPValue m_param3;
    YCPValue m_param4;
public:

    Y2CIMFunction (CIMFunctions* instance, unsigned int pos);    
    bool attachParameter (const YCPValue& arg, const int position);
    constTypePtr wantedParameterType () const;
    bool appendParameter (const YCPValue& arg);
    bool finishParameters ();
    YCPValue evaluateCall ();
    bool reset ();
};


    Y2CIMFunction::Y2CIMFunction (CIMFunctions* instance, unsigned int pos) :
	m_position (pos)
	, m_instance (instance)
	, m_param1 ( YCPNull () )
	, m_param2 ( YCPNull () )
	, m_param3 ( YCPNull () )
	, m_param4 ( YCPNull () )
    {
    };
    
    bool Y2CIMFunction::attachParameter (const YCPValue& arg, const int position)
    {
	switch (position)
	{
	    case 0: m_param1 = arg; break; 
	    case 1: m_param2 = arg; break; 
	    case 2: m_param3 = arg; break; 
	    case 3: m_param4 = arg; break; 
	    default: return false;
	}

	return true;
    }
    
    constTypePtr Y2CIMFunction::wantedParameterType () const
    {
	y2internal ("wantedParameterType not implemented");
	return Type::Unspec;
    }
    
    bool Y2CIMFunction::appendParameter (const YCPValue& arg)
    {
	if (m_param1.isNull ())
	{
	    m_param1 = arg;
	    return true;
	} else if (m_param2.isNull ())
	{
	    m_param2 = arg;
	    return true;
	} else if (m_param3.isNull ())
	{
	    m_param3 = arg;
	    return true;
	} else if (m_param4.isNull ())
	{
	    m_param4 = arg;
	    return true;
	}
	y2internal ("appendParameter > 3 not implemented");
	return false;
    }
    
    bool Y2CIMFunction::finishParameters ()
    {
	y2internal ("finishParameters not implemented");
	return true;
    }
    
    YCPValue Y2CIMFunction::evaluateCall ()
    {
	switch (m_position) {
#include "CIMBuiltinCalls.h"
	}
	
	return YCPNull ();
    }
    
    bool Y2CIMFunction::reset ()
    {
	m_param1 = YCPNull ();
	m_param2 = YCPNull ();
	m_param3 = YCPNull ();
	m_param4 = YCPNull ();

	return true;
    }

/**
 * Constructor.
 */
CIMFunctions::CIMFunctions () :
    m_client (NULL)
{
    registerFunctions ();
}

/**
 * Destructor.
 */
CIMFunctions::~CIMFunctions ()
{
    if (m_client)
	delete m_client;
}

Y2Function* CIMFunctions::createFunctionCall (const string name, constFunctionTypePtr type)
{
    vector<string>::iterator it = find (_registered_functions.begin ()
	, _registered_functions.end (), name);
    if (it == _registered_functions.end ())
    {
	y2error ("No such function %s", name.c_str ());
	return NULL;
    }
    
    return new Y2CIMFunction (this, it - _registered_functions.begin ());
}

void CIMFunctions::registerFunctions()
{
#include "CIMBuiltinTable.h"
}

OpenWBEM::CIMClient* CIMFunctions::client ()
{
        if (m_client == NULL)
        {
            // create the client (connect)
            m_client = new OpenWBEM::CIMClient ("wbem://localhost/", "root/cimv2");
        }
        return m_client;
}


YCPValue CIMFunctions::EnumerateInstanceNames (const YCPString& classname)
{
    try
    {
        OpenWBEM::CIMObjectPathEnumeration result = client()->enumInstanceNamesE 
            ( OpenWBEM::String( classname->value ().c_str () ) ); 


        YCPList result_list;
        OpenWBEM::CIMObjectPath cop;

        while (result.hasMoreElements())
        {
            result.nextElement(cop);

            if (! cop)
            {
                y2error ("Nonexistent cop\n");
                continue;
            }

            result_list->add (YCPString(cop.toString().c_str())); 
        }

        return result_list;
    }
    catch (OpenWBEM::CIMException &e) 
    {
        y2error("%s", e.getMessage());
        return YCPVoid();
    }
}


// EnumInstances
//
YCPValue CIMFunctions::EnumerateInstances (const YCPString& classname)
{
    OpenWBEM::CIMInstanceEnumeration result = client()->enumInstancesE
	( OpenWBEM::String( classname->value ().c_str () ) ); 


    YCPList result_list;
    OpenWBEM::CIMObjectPath cop;

    while (result.hasMoreElements())
    {
        YCPMap instance;
        OpenWBEM::CIMInstance i = result.nextElement();
        OpenWBEM::CIMPropertyArray props = i.getProperties();

	if (! i)
	{
	    y2error ("Nonexistent cop\n");
	    continue;
	}
        for (size_t i = 0; i < props.size(); ++i)
        {
            OpenWBEM::CIMProperty& p = props[i];
            YCPValue v;
            if (p.getDataType().getType() == OpenWBEM::CIMDataType::CIMNULL)
            {
                instance->add(YCPString(p.getName().toString().c_str()), YCPVoid());
            }
            else if (p.getDataType().getType() == OpenWBEM::CIMDataType::STRING)
            {
                v = YCPString(p.getValue().toString().c_str());
                instance->add(YCPString(p.getName().toString().c_str()), v);
            }
            else if (p.getDataType().getType() == OpenWBEM::CIMDataType::BOOLEAN)
            {
                instance->add(YCPString(p.getName().toString().c_str()), YCPBoolean(p.getValue().toString().c_str()));
            }
            else
            {
                instance->add(YCPString(p.getName().toString().c_str()), YCPString(""));
            }

        }
	
	result_list->add (instance); 
    }

    return result_list;
}

// EnumerateClassNames
//
YCPValue CIMFunctions::EnumerateClassNames (const YCPString& classname)
{
    try
    {
        OpenWBEM::StringEnumeration enu = client()->enumClassNamesE 
            ( OpenWBEM::String( classname->value ().c_str () ),
           WBEMFlags::E_SHALLOW ); 

        YCPList result_list;

        while (enu.hasMoreElements())
        {
            result_list->add (YCPString(enu.nextElement().c_str() )); 
            //y2debug("%s", enu.nextElement().c_str());
        }
        return result_list;
    }

    catch (OpenWBEM::CIMException &e)
    {
        y2error("%s", e.getMessage());
        return YCPVoid();
    }
}

