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

   File:       Y2CCCIM.h

   Author:     Stanislav Visnovsky <visnov@suse.de>
   Maintainer: Stanislav Visnovsky <visnov@suse.de>

/-*/
// -*- c++ -*-

/*
 * Component Creator that executes access to CIM
 *
 * Author:     Stanislav Visnovsky <visnov@suse.de>
 */

#ifndef Y2CCCIM_h
#define Y2CCCIM_h

#include <y2/Y2ComponentCreator.h>

class Y2Component;

class Y2CCCIM : public Y2ComponentCreator
{
    
public:
    /**
     * Creates a pkg component creator.
     */
    Y2CCCIM() : Y2ComponentCreator(Y2ComponentBroker::BUILTIN) {}

    /**
     * Tries to create a Pkg module
     */
    virtual Y2Component *createInLevel(const char *name, int level, int current_level) const;

    virtual bool isServerCreator() const;
    
    /**
     * We provide the CIM namespace
     */
    virtual  Y2Component *provideNamespace(const char *name);
};


#endif // Y2CCCIM_h
