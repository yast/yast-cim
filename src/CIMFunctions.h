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

   File:	CIMFunctions.h

   Author:	Stanislav Visnovsky <visnov@suse.cz>
   Maintainer:  Stanislav Visnovsky <visnov@suse.cz>

   Purpose:	Access to CIMOM
		Handles CIM::function (list_of_arguments) calls
/-*/

#ifndef CIMFunctions_h
#define CIMFunctions_h

#include <y2/Y2Namespace.h>

#include <ycp/YCPVoid.h>

#include <openwbem/OW_CIM.hpp>
#include <openwbem/OW_CIMClient.hpp>

/**
 * A simple class for CIMOM access
 */

class CIMFunctions : public Y2Namespace
{
    public:
	// general
	/* TYPEINFO: list<string>(string) */
	YCPValue EnumerateInstanceNames (const YCPString& classname);

	// general
	/* TYPEINFO: list<map<string,any>>(string) */
	YCPValue EnumerateInstances (const YCPString& classname);


	// general
	/* TYPEINFO: list<string>(string,symbol) */
	YCPValue EnumerateClassNames (const YCPString& classname, const YCPSymbol& DeepFlag);
        
	// general
	/* TYPEINFO: list<string>(string,symbol) */
	YCPValue EnumerateClasses (const YCPString& classname, const YCPSymbol& DeepFlag);

	// general
	/* TYPEINFO: map<string,any>(string) */
        YCPValue GetInstance (const YCPString& objectName );


	// general
	/* TYPEINFO: boolean(string) */
        YCPValue DeleteInstance (const YCPString& objectName );

	// general
	/* TYPEINFO: list<string>(string) */
        YCPValue ReferenceNames (const YCPString& objectName );

	// general
	/* TYPEINFO: list<map<string,any>>(string,string,string) */
        YCPValue References (const YCPString& objectName ,const YCPString& resultClass, const YCPString& role);
        
	// general
	/* TYPEINFO: list<string>(string,string,string,string,string) */
        YCPValue AssociatorNames (const YCPString& objectName, const YCPString& assocClass, const YCPString& resultClass, const YCPString& role, const YCPString& resultRole);

	// general
	/* TYPEINFO: list<map<string,any>>(string,string,string,string,string) */
        YCPValue Associators (const YCPString& objectName, const YCPString& assocClass, const YCPString& resultClass, const YCPString& role, const YCPString& resultRole);
        

	// general
	/* TYPEINFO: string(string,string,map<string,any>) */
        YCPValue CreateObjectPath(const YCPString& ns, const YCPString& className, const YCPMap& pathMap);

	// general
	/* TYPEINFO: string() */
	YCPValue LastError ();
        
	// general
	/* TYPEINFO: boolean(string,string) */
        YCPValue Connect (const YCPString& url, const YCPString& ns);


	/**
	 * Constructor.
	 */
	CIMFunctions ();

	/**
	 * Destructor.
	 */
	virtual ~CIMFunctions ();

	virtual const string name () const
	{
    	    return "CIM";
	}

	virtual const string filename () const
	{
    	    return "CIM";
	}

	virtual string toString () const
	{
    	    return "// not possible toString";
	}

	virtual YCPValue evaluate (bool cse = false )
	{
    	    if (cse) return YCPNull ();
    	    else return YCPVoid ();
	}

	virtual Y2Function* createFunctionCall (const string name, constFunctionTypePtr type);

    protected:
        

    private:
	void registerFunctions ();
	OpenWBEM::CIMClient* client ();
        YCPList arrayValueToList(const OpenWBEM::CIMValue& value);
        YCPValue ValueToAny(const OpenWBEM::CIMValue &value);
	YCPValue GetInstance (const OpenWBEM::CIMObjectPath path );
	YCPValue GetInstance (const OpenWBEM::CIMInstance instance);
        YCPValue DeleteInstance (const OpenWBEM::CIMObjectPath path );
        YCPValue ReferenceNames (const OpenWBEM::CIMObjectPath path );
        YCPValue AssociatorNames (const OpenWBEM::CIMObjectPath path, 
                const YCPString& assocClass,
                const YCPString& resultClass,
                const YCPString& role,
                const YCPString& resultRole);

        YCPValue Associators (const OpenWBEM::CIMObjectPath path, 
                const YCPString& assocClass,
                const YCPString& resultClass,
                const YCPString& role,
                const YCPString& resultRole);

        YCPValue References (const OpenWBEM::CIMObjectPath path ,const YCPString& resultClass, const YCPString& role);

        bool cimomAvailable(OpenWBEM::CIMClient *client);

	vector<string> _registered_functions;
	OpenWBEM::CIMClient* m_client;
};

#endif // CIMFunctions_h
