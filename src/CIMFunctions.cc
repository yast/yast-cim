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
#include "openwbem/OW_CIMQualifier.hpp"
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


template <class type>
static YCPValue typeToString(const OpenWBEM::CIMValue &value)
{
  type stringtype;
  value.get(stringtype);
  return YCPString(stringtype.toString().c_str());
}

template <class type, class numtype>
static YCPValue realToFloat(const OpenWBEM::CIMValue &value)
{
  type floattype;
  value.get(floattype);
  return YCPFloat((numtype)floattype);
}

template <class type, class numtype>
static YCPValue numToInteger(const OpenWBEM::CIMValue &value)
{
  type inttype;
  value.get(inttype);
  return YCPInteger((numtype)inttype);
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

YCPValue CIMFunctions::ValueToAny(const OpenWBEM::CIMValue &value)
{

  if (value && value.isArray())
    return arrayValueToList(value);

  // numeric types
  if (value.getType() == OpenWBEM::CIMDataType::UINT8)
    return numToInteger<OpenWBEM::UInt8, OpenWBEM::UInt8>(value);
  if (value.getType() == OpenWBEM::CIMDataType::SINT8)
    return numToInteger<OpenWBEM::Int8, OpenWBEM::Int8>(value);
  if (value.getType() == OpenWBEM::CIMDataType::UINT16)
    return numToInteger<OpenWBEM::UInt16, OpenWBEM::UInt16>(value);
  if (value.getType() == OpenWBEM::CIMDataType::SINT16)
    return numToInteger<OpenWBEM::Int16, OpenWBEM::Int16>(value);
  if (value.getType() == OpenWBEM::CIMDataType::UINT32)
    return numToInteger<OpenWBEM::UInt32, OpenWBEM::UInt32>(value);
  if (value.getType() == OpenWBEM::CIMDataType::SINT32)
    return numToInteger<OpenWBEM::Int32, OpenWBEM::Int32>(value);
  if (value.getType() == OpenWBEM::CIMDataType::REAL32)
    return realToFloat<OpenWBEM::Real32, OpenWBEM::Real32>(value);
  if (value.getType() == OpenWBEM::CIMDataType::REAL64)
    return realToFloat<OpenWBEM::Real64, OpenWBEM::Real64>(value);
  if (value.getType() == OpenWBEM::CIMDataType::UINT64)
    return numToInteger<OpenWBEM::UInt64, OpenWBEM::UInt64>(value);
  if (value.getType() == OpenWBEM::CIMDataType::SINT32)
    return numToInteger<OpenWBEM::Int64, OpenWBEM::Int64>(value);  

  // boolean
  if (value.getType() == OpenWBEM::CIMDataType::BOOLEAN)
    return YCPBoolean(value.toString().c_str());

  // strings
  if (value.getType() == OpenWBEM::CIMDataType::STRING)
    return typeToString<OpenWBEM::String>(value);

  // date and time value
  if (value.getType() == OpenWBEM::CIMDataType::DATETIME)
    //return typeToString<OpenWBEM::String>(value);
    return YCPString(value.toString().c_str());

  // reference
  if (value.getType() == OpenWBEM::CIMDataType::REFERENCE)
    return typeToString<OpenWBEM::CIMObjectPath>(value);

  // embedded class
  if (value.getType() == OpenWBEM::CIMDataType::EMBEDDEDCLASS)
    return typeToString<OpenWBEM::CIMClass>(value);

  // embedded instance
  if (value.getType() == OpenWBEM::CIMDataType::EMBEDDEDINSTANCE)
    return typeToString<OpenWBEM::CIMInstance>(value);

  // unknown array, should not happen
  return YCPVoid();
}

//
// Create an Instance from a YCP Map
// 
//
/*
OpenWBEM::CIMInstance CIMFunctions::CreateInstance(const YCPMap& instance )
{
    OpenWBEM::CIMInstance inst;

    return inst;
}
*/



// ModifyInstance
//
/*
YCPValue CIMFunctions::ModifyInstance (const YCPMap& instance )
{
    OpenWBEM::CIMInstance inst = CreateInstance(instance);

}
*/



// DeleteInstance
//
YCPValue CIMFunctions::DeleteInstance (const YCPString& instanceName )
{

    OpenWBEM::CIMObjectPath cop;
    cop = OpenWBEM::CIMObjectPath::parse(instanceName->value().c_str());
    return  DeleteInstance(cop);
}

// DeleteInstance
//
YCPValue CIMFunctions::DeleteInstance (OpenWBEM::CIMObjectPath path )
{

    try
    {
        client()->deleteInstance ( path  ); 
        return YCPBoolean("true");
    }
    catch (OpenWBEM::CIMException &e)
    {
        y2error("%s", e.getMessage());
        return YCPBoolean("false");
    }
}

// GetInstance
//
YCPValue CIMFunctions::GetInstance (const YCPString& instanceName )
{

    OpenWBEM::CIMObjectPath cop;
    cop = OpenWBEM::CIMObjectPath::parse(instanceName->value().c_str());
    return (GetInstanceI(cop));
}

// GetInstance
//
//YCPValue CIMFunctions::GetInstance (const YCPString& instanceName )
YCPValue CIMFunctions::GetInstanceI (OpenWBEM::CIMObjectPath path)
{

    OpenWBEM::CIMInstance result = client()->getInstance ( path  ); 
    
    YCPMap instance;

    OpenWBEM::CIMPropertyArray props = result.getProperties(OpenWBEM::CIMDataType::INVALID);

    for (size_t i = 0; i < props.size(); ++i)
    {
        YCPValue v;
        // Property
        OpenWBEM::CIMProperty& p = props[i];

        // Qualifiers
        OpenWBEM::CIMQualifierArray q = p.getQualifiers();
        YCPMap qualifiers;
        YCPMap propertyMap;

        for (size_t iq = 0; iq < q.size(); ++iq)
        {
            y2debug("Qualifier: %s ", q[iq].getName().c_str() );
            CIMValue qv = q[iq].getValue();

            qualifiers->add(YCPString(q[iq].getName().c_str()), ValueToAny(qv) );
                    
        }
        propertyMap->add(YCPString("qualifiers"), qualifiers );

        CIMValue cv = p.getValue();
        YCPString name(p.getName().toString().c_str());
        OpenWBEM::String pname = p.getName();
        if (cv)
        {
            y2debug("property= %s, datatype %s", 
                    name->value().c_str(),
                    p.getDataType().toString().c_str());

            propertyMap->add(YCPString("value"), ValueToAny(cv) );
            instance->add(name, propertyMap);
        }
        else
        {
            propertyMap->add(YCPString("value"), YCPVoid());
            instance->add(name, propertyMap);
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

        y2debug("Getting Instance: %s", cop.toString().c_str());
        result_list->add (GetInstanceI(cop));
    }

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
