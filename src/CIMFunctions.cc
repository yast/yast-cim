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
#include <ycp/YCPSymbol.h>
#include <ycp/YCPFloat.h>

#include "CIMFunctions.h"

#include "openwbem/OW_config.h"
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
            y2debug("connection...");
        }
        return m_client;
}

YCPValue CIMFunctions::Connect (const YCPString& url, const YCPString& ns)
{
    //y2debug("url: %s, namespace: %s", url.toString().c_str(), ns.toString().c_str());
    return YCPVoid();
}


// EnumerateInstanceNames
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
    catch (OpenWBEM::CIMException& e) 
    {
        y2error("%s", e.getMessage());
        return YCPVoid();
    }
}

template <class type>
static YCPList typeToArray(const OpenWBEM::CIMValue &value)
{
  YCPList result;
  OpenWBEM::Array<type> array;
  value.get(array);
  typename OpenWBEM::Array<type>::iterator it;
  for (it = array.begin(); it != array.end(); ++it)
  {
    //result->add(YCPString(it->toString().c_str() ));
    result->add(YCPString(""));
  }
  return result;
}


template <class type, class numtype>
static YCPList numToArray(const OpenWBEM::CIMValue &value)
{
  YCPList result;
  OpenWBEM::Array<type> array;
  value.get(array);
  typename OpenWBEM::Array<type>::iterator it;
  for (it = array.begin(); it != array.end(); ++it)
  {
    result->add(YCPInteger((numtype)*it));
  }
  return result;
}

template <class type, class numtype>
static YCPList floatToArray(const OpenWBEM::CIMValue &value)
{
  YCPList result;
  OpenWBEM::Array<type> array;
  value.get(array);
  typename OpenWBEM::Array<type>::iterator it;
  for (it = array.begin(); it != array.end(); ++it)
  {
    result->add(YCPFloat((numtype)*it));
  }
  return result;
}

YCPList CIMFunctions::arrayValueToList(const OpenWBEM::CIMValue &value)
{
  if (!value && !value.isArray())
    return YCPList();

  // numeric types
  if (value.getType() == OpenWBEM::CIMDataType::UINT8)
    return numToArray<OpenWBEM::UInt8, OpenWBEM::UInt8>(value);
  if (value.getType() == OpenWBEM::CIMDataType::SINT8)
    return numToArray<OpenWBEM::Int8, OpenWBEM::Int8>(value);
  if (value.getType() == OpenWBEM::CIMDataType::UINT16)
    return numToArray<OpenWBEM::UInt16, OpenWBEM::UInt16>(value);
  if (value.getType() == OpenWBEM::CIMDataType::SINT16)
    return numToArray<OpenWBEM::Int16, OpenWBEM::Int16>(value);
  if (value.getType() == OpenWBEM::CIMDataType::UINT32)
    return numToArray<OpenWBEM::UInt32, OpenWBEM::UInt32>(value);
  if (value.getType() == OpenWBEM::CIMDataType::SINT32)
    return numToArray<OpenWBEM::Int32, OpenWBEM::Int32>(value);
  if (value.getType() == OpenWBEM::CIMDataType::REAL32)
    return floatToArray<OpenWBEM::Real32, OpenWBEM::Real32>(value);
  if (value.getType() == OpenWBEM::CIMDataType::REAL64)
    return floatToArray<OpenWBEM::Real64, OpenWBEM::Real64>(value);
  if (value.getType() == OpenWBEM::CIMDataType::UINT64)
    return numToArray<OpenWBEM::UInt64, OpenWBEM::UInt64>(value);
  if (value.getType() == OpenWBEM::CIMDataType::SINT32)
    return numToArray<OpenWBEM::Int64, OpenWBEM::Int64>(value);  

  // strings
  if (value.getType() == OpenWBEM::CIMDataType::STRING)
    return typeToArray<OpenWBEM::String>(value);

  // date and time value
  if (value.getType() == OpenWBEM::CIMDataType::DATETIME)
    return typeToArray<OpenWBEM::String>(value);

  // reference
  if (value.getType() == OpenWBEM::CIMDataType::REFERENCE)
    return typeToArray<OpenWBEM::CIMObjectPath>(value);

  // embedded class
  if (value.getType() == OpenWBEM::CIMDataType::EMBEDDEDCLASS)
    return typeToArray<OpenWBEM::CIMClass>(value);

  // embedded instance
  if (value.getType() == OpenWBEM::CIMDataType::EMBEDDEDINSTANCE)
    return typeToArray<OpenWBEM::CIMInstance>(value);

  // unknown array, should not happen
  return YCPList();
}



// GetInstance
//
YCPValue CIMFunctions::GetInstance (const YCPString& instanceName )
{
    return YCPMap();
}

// GetInstance
//
//YCPValue CIMFunctions::GetInstance (const YCPString& instanceName )
YCPValue CIMFunctions::GetInstanceI (OpenWBEM::CIMObjectPath path)
{
    //OpenWBEM::CIMObjectPath path;
    //path.parse(instanceName->value ().c_str ());
    y2milestone("GetInstance: %s", path.toString ().c_str ());

    OpenWBEM::CIMInstance result = client()->getInstance ( path  ); 
    
    YCPMap instance;

    OpenWBEM::CIMPropertyArray props = result.getProperties(OpenWBEM::CIMDataType::INVALID);

    for (size_t i = 0; i < props.size(); ++i)
    {
        OpenWBEM::CIMProperty& p = props[i];
        YCPValue v;

        CIMValue cv = p.getValue();
        YCPString name(p.getName().toString().c_str());
        OpenWBEM::String pname = p.getName();
        if (cv)
        {
            y2debug("property= %s, datatype %s", 
                    name->value().c_str(),
                    p.getDataType().toString().c_str());
            if (p.getDataType().isArrayType())
            {
                y2debug("is array");
                YCPList a = arrayValueToList(cv);
                instance->add(name, a);
            }
            else if (p.getDataType().isReferenceType())
            {
                y2debug("is reference");
            }
            else 
            {
                OpenWBEM::UInt64 aa;
                OpenWBEM::UInt32 ab;
                OpenWBEM::UInt16 ac;

                switch (p.getDataType().getType())
                {
                case OpenWBEM::CIMDataType::CIMNULL:
                    instance->add(name, YCPVoid());
                    break;
                case OpenWBEM::CIMDataType::STRING:
                    v = YCPString(cv.toString().c_str());
                    instance->add(name, v);
                    break;
                case OpenWBEM::CIMDataType::EMBEDDEDCLASS:
                    v = YCPString(cv.toString().c_str());
                    instance->add(name, v);
                    break;
                case OpenWBEM::CIMDataType::DATETIME:
                    v = YCPString(cv.toString().c_str());
                    instance->add(name, v);
                    break;
                case OpenWBEM::CIMDataType::UINT64:
                    cv.get(aa);
                    instance->add(name, YCPInteger((int) aa));
                    break;
                case OpenWBEM::CIMDataType::UINT32:
                    y2debug("%s uint32", name->value().c_str());
                    cv.get(ab);
                    instance->add(name, YCPInteger((int) ab));
                    break;
                case OpenWBEM::CIMDataType::UINT16:
                    cv.get(ac);
                    instance->add(name, YCPInteger((int) ac));
                    break;
                case OpenWBEM::CIMDataType::BOOLEAN:
                    instance->add(name, YCPBoolean(cv.toString().c_str()));
                    break;
                default:
                    instance->add(name, YCPVoid());
                    break;
                }
            }
        }
        else
        {
            instance->add(name, YCPVoid());
        }
    }

    return instance;
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
        OpenWBEM::CIMInstance i = result.nextElement();
        cop = OpenWBEM::CIMObjectPath("root/cimv2", i);

        if (! cop)
        {
            y2error ("Nonexistent cop\n");
            continue;
        }

        result_list->add (GetInstanceI(cop));
        //result_list->add (YCPMap());
    }


    /*
    YCPList result_list;

    while (result.hasMoreElements())
    {
        YCPMap instance;
        OpenWBEM::CIMInstance in = result.nextElement();
        OpenWBEM::CIMPropertyArray props = in.getProperties(OpenWBEM::CIMDataType::INVALID);



        y2debug("debug 1");

        for (size_t i = 0; i < props.size(); ++i)
        {
            OpenWBEM::CIMProperty& p = props[i];
            YCPValue v;

            CIMValue cv = p.getValue();
            YCPString name(p.getName().toString().c_str());
            OpenWBEM::String pname = p.getName();
            if (cv)
            {
                y2debug("property= %s, datatype %s", 
                        name->value().c_str(),
                        p.getDataType().toString().c_str());
                if (p.getDataType().isArrayType())
                {
                    y2debug("is array");
                    YCPList a = arrayValueToList(cv);
                    instance->add(name, a);
                }
                else if (p.getDataType().isReferenceType())
                {
                    y2debug("is reference");
                }
                else 
                {
                    OpenWBEM::UInt64 aa;
                    OpenWBEM::UInt32 ab;
                    OpenWBEM::UInt16 ac;

                    switch (p.getDataType().getType())
                    {
                    case OpenWBEM::CIMDataType::CIMNULL:
                        instance->add(name, YCPVoid());
                        break;
                    case OpenWBEM::CIMDataType::STRING:
                        v = YCPString(cv.toString().c_str());
                        instance->add(name, v);
                        break;
                    case OpenWBEM::CIMDataType::EMBEDDEDCLASS:
                        v = YCPString(cv.toString().c_str());
                        instance->add(name, v);
                        break;
                    case OpenWBEM::CIMDataType::DATETIME:
                        v = YCPString(cv.toString().c_str());
                        instance->add(name, v);
                        break;
                    case OpenWBEM::CIMDataType::UINT64:
                        cv.get(aa);
                        instance->add(name, YCPInteger((int) aa));
                        break;
                    case OpenWBEM::CIMDataType::UINT32:
                        y2debug("%s uint32", name->value().c_str());
                        cv.get(ab);
                        instance->add(name, YCPInteger((int) ab));
                        break;
                    case OpenWBEM::CIMDataType::UINT16:
                        cv.get(ac);
                        instance->add(name, YCPInteger((int) ac));
                        break;
                    case OpenWBEM::CIMDataType::BOOLEAN:
                        instance->add(name, YCPBoolean(cv.toString().c_str()));
                        break;
                    default:
                        instance->add(name, YCPVoid());
                        break;
                    }
                }
            }
            else
            {
                instance->add(name, YCPVoid());
            }

        }
	
	result_list->add (instance); 
    }
    */

    return result_list;
}

// EnumerateClasses
//
YCPValue CIMFunctions::EnumerateClasses (const YCPString& classname, 
        const YCPSymbol& DeepFlag)
{

    WBEMFlags::EDeepFlag deep = WBEMFlags::E_SHALLOW;
    string flag = DeepFlag->symbol();
    if (flag == "deep")
    {
        y2debug("Deep selected");
        deep = WBEMFlags::E_DEEP;
    }
    else if (flag == "shallow")
    {
        y2debug("Shallow selected");
        deep = WBEMFlags::E_SHALLOW;
    }

    try
    {
        OpenWBEM::CIMClassEnumeration enu = client()->enumClassE
            ( OpenWBEM::String( classname->value ().c_str () ),
           deep ); 

        YCPList result_list;

        while (enu.hasMoreElements())
        {
            YCPMap cls;
            YCPValue v;
            OpenWBEM::CIMClass c = enu.nextElement();
            OpenWBEM::CIMPropertyArray props = c.getProperties();
            for (size_t i = 0; i < props.size(); ++i)
            {
                OpenWBEM::CIMProperty& p = props[i];
                if (p.getDataType().getType() == OpenWBEM::CIMDataType::CIMNULL)
                {
                    cls->add(YCPString(p.getName().toString().c_str()), YCPVoid());
                }
                else if (p.getDataType().getType() == OpenWBEM::CIMDataType::STRING)
                {
                    v = YCPString(p.getValue().toString().c_str());
                    cls->add(YCPString(p.getName().toString().c_str()), v);
                }
                else if (p.getDataType().getType() == OpenWBEM::CIMDataType::BOOLEAN)
                {
                    cls->add(YCPString(p.getName().toString().c_str()), YCPBoolean(p.getValue().toString().c_str()));
                }
                else
                {
                    cls->add(YCPString(p.getName().toString().c_str()), YCPString(""));
                }
            }
            result_list->add (cls ); 
        }
        return result_list;
    }

    catch (OpenWBEM::CIMException &e)
    {
        y2error("%s", e.getMessage());
        return YCPVoid();
    }
}


// EnumerateClassNames
//
YCPValue CIMFunctions::EnumerateClassNames (const YCPString& classname, 
        const YCPSymbol& DeepFlag)
{

    WBEMFlags::EDeepFlag deep = WBEMFlags::E_SHALLOW;
    string flag = DeepFlag->symbol();
    if (flag == "deep")
    {
        y2debug("Deep selected");
        deep = WBEMFlags::E_DEEP;
    }
    else if (flag == "shallow")
    {
        y2debug("Shallow selected");
        deep = WBEMFlags::E_SHALLOW;
    }

    try
    {
        OpenWBEM::StringEnumeration enu = client()->enumClassNamesE 
            ( OpenWBEM::String( classname->value ().c_str () ),
           deep ); 

        YCPList result_list;

        while (enu.hasMoreElements())
        {
            result_list->add (YCPString(enu.nextElement().c_str() )); 
        }
        return result_list;
    }

    catch (OpenWBEM::CIMException &e)
    {
        y2error("%s", e.getMessage());
        return YCPVoid();
    }
}

// EnumerateClassNames
//
YCPValue CIMFunctions::LastError ()
{

    return(YCPString(""));
}
