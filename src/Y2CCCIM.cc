/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       Y2CCPkg.cc

   Author:     Stanislav Visnovsky <visnov@suse.cz>
   Maintainer: Stanislav Visnovsky <visnov@suse.cz>

/-*/
/*
 * Component Creator that executes access to CIM
 *
 * Author: Stanislav Visnovsky <visnov@suse.cz>
 */


#include <ycp/y2log.h>
#include <y2/Y2Component.h>
#include "Y2CCCIM.h"
#include "Y2CIMComponent.h"

Y2Component *Y2CCCIM::createInLevel(const char *name, int level, int) const
{
    if (strcmp (name, "CIM") == 0)
    {
	return Y2CIMComponent::instance ();
    }
    else
    {
	return NULL;
    }
}

bool Y2CCCIM::isServerCreator() const
{
    return false;
}

Y2Component* Y2CCCIM::provideNamespace(const char* name)
{
    if (strcmp (name, "CIM") == 0)
    {
	return Y2CIMComponent::instance ();
    }
    else
    {
	return NULL;
    }
}

// Create global variable to register this component creator

Y2CCCIM g_y2ccCIM;
