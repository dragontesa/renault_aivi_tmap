
#ifndef __EXTERNALTHIRDPARTYNAVIGATIONAPP_INTERFACE_H
#define __EXTERNALTHIRDPARTYNAVIGATIONAPP_INTERFACE_H

class ExternalThirdPartyNavigationApp_interface
{
public:

   //Start the external third party navigation
   virtual bool bExtThirdPartyNavigaitonStartApp ( ) = 0;

   //Monitor the external third party navigation with watchdog
   virtual bool bExtThirdPartyNavigaitonAppWatchdog ( ) = 0;

   //Load the persistent data of external third party navigation
   virtual bool bExtThirdPartyNavigaitonAppLoadPersistentData ( ) = 0;

   //Save the persistent data of external third party navigation
   virtual bool bExtThirdPartyNavigaitonAppSavePersistentData ( ) = 0;
   
};

//TODO Third party navigation should create a class to derive the base class(ExternalThirdPartyNavigationApp_interface) and implement below functions
ExternalThirdPartyNavigationApp_interface* CreateExternalThirdPartyNavigationAppInterface(void);

void DeleteExternalThirdPartyNavigationAppInterface(ExternalThirdPartyNavigationApp_interface** ppInterface);

#ifdef ENABLE_FAKE_NAVI
/*!
 *
 * \brief   This is a sample for implement the third party navigation startup
 *
 * \author  Neil Mao (CM/ESW21-CN)
 *
 */
class ExternalThirdPartyNavigationApp: public ExternalThirdPartyNavigationApp_interface
{
public:
   ExternalThirdPartyNavigationApp ()
   {
   }

   virtual ~ExternalThirdPartyNavigationApp ( )
   {
   }

   //Start the external third party navigation
   virtual bool bExtThirdPartyNavigaitonStartApp ( )
   {
      //TODO start third party navigation now
      // Suggest create a new thread to start third party navigation

      return true;
   }

   //Monitor the external third party navigation with watchdog
   virtual bool bExtThirdPartyNavigaitonAppWatchdog ( )
   {
      return true;
   }

   //Load the persistent data of external third party navigation
   virtual bool bExtThirdPartyNavigaitonAppLoadPersistentData ( )
   {
      return true;
   }

   //Save the persistent data of external third party navigation
   virtual bool bExtThirdPartyNavigaitonAppSavePersistentData ( )
   {
      return true;
   }
};


inline ExternalThirdPartyNavigationApp_interface* CreateExternalThirdPartyNavigationAppInterface (void)
{
   ExternalThirdPartyNavigationApp* pInterface = new ExternalThirdPartyNavigationApp();

   if(pInterface)
   {
      return dynamic_cast<ExternalThirdPartyNavigationApp_interface*> (pInterface);
   }
   else
   {
      return NULL;
   }
}

inline void DeleteExternalThirdPartyNavigationAppInterface (ExternalThirdPartyNavigationApp_interface** ppInterface)
{
   ExternalThirdPartyNavigationApp* pInterface = dynamic_cast<ExternalThirdPartyNavigationApp*>(*ppInterface);

   delete pInterface;

   *ppInterface = NULL;
}

#endif

#endif   //__EXTERNALTHIRDPARTYNAVIGATIONAPP_INTERFACE_H
