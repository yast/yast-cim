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

/**
 * A simple class for CIMOM access
 */
 
namespace OpenWBEM {
    class CIMClient;
};


class CIMFunctions : public Y2Namespace
{
    public:
	// general
	/* TYPEINFO: list<string>(string) */
	YCPValue EnumerateInstanceNames (const YCPString& classname);

	// general
	/* TYPEINFO: list<string>(string) */
	YCPValue EnumerateInstances (const YCPString& classname);


	// general
	/* TYPEINFO: list<string>(string) */
	YCPValue EnumerateClassNames (const YCPString& classname);
        
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
	
    private:
	void registerFunctions ();
	OpenWBEM::CIMClient* client ();

	vector<string> _registered_functions;
	OpenWBEM::CIMClient* m_client;
};

#endif // CIMFunctions_h
