/* ***************************************************************************************
* FILE:          ItemRegistry.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  ItemRegistry is part of HMI-Base Widget Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */
#ifndef ITEM_REGISTRY_H
#define ITEM_REGISTRY_H

#include <map>
#include <list>

namespace hmibase {
namespace util {

/*****************************************************************************/
/* Provides a registry in which different items are associated to ids. */
template <typename TId, typename TItem>
class ItemPtrRegistry
{
   public:
      typedef TId IdType;
      typedef TItem ItemType;

      /*****************************************************************************/
      /* Associates an item to an id. Returns false if an item is already associated to the specified id. */
      static bool registerItem(const IdType& id, ItemType* item)
      {
         if (item != NULL)
         {
            typename ItemMapType::const_iterator it = getItemMap().find(id);
            if (it == getItemMap().end())
            {
               getItemMap()[id] = item;
               return true;
            }
         }
         return false;
      }

      /*****************************************************************************/
      /* Unregisters the item associated to the specified id. Returns false if no item is associated. */
      static bool unregisterItem(const IdType& id)
      {
         typename ItemMapType::iterator it = getItemMap().find(id);
         if (it != getItemMap().end())
         {
            getItemMap().erase(it);
            return true;
         }
         return false;
      }

      /*****************************************************************************/
      /* Returns true if the item associated to the specified id is found. */
      static bool isItemRegistered(const IdType& id)
      {
         typename ItemMapType::const_iterator it = getItemMap().find(id);
         return (it != getItemMap().end());
      }

      /*****************************************************************************/
      /* Returns the item associated to the specified id or NULL is no item is found. */
      static ItemType* getItem(const IdType& id)
      {
         typename ItemMapType::const_iterator it = getItemMap().find(id);
         if (it != getItemMap().end())
         {
            return it->second;
         }
         return NULL;
      }

   protected:
      typedef std::map<IdType, ItemType*> ItemMapType;

      static ItemMapType& getItemMap()
      {
         static ItemMapType _itemMap;
         return _itemMap;
      }
};


/*****************************************************************************/
/* Provides a registry in which different items are associated to ids. */
template <typename TId, typename TItem>
class ItemValRegistry
{
   public:
      typedef TId IdType;
      typedef TItem ItemType;

      /*****************************************************************************/
      /* Associates an item to an id. */
      static void registerItem(const IdType& id, const ItemType& item)
      {
         getItemMap()[id] = item;
      }

      /*****************************************************************************/
      /* Unregisters the item associated to the specified id. */
      static void unregisterItem(const IdType& id)
      {
         typename ItemMapType::iterator it = getItemMap().find(id);
         if (it != getItemMap().end())
         {
            getItemMap().erase(it);
         }
      }

      /*****************************************************************************/
      /* Returns true if the item associated to the specified id is found. */
      static bool isItemRegistered(const IdType& id)
      {
         typename ItemMapType::const_iterator it = getItemMap().find(id);
         return (it != getItemMap().end());
      }

      /*****************************************************************************/
      /* Returns the item associated to the specified id or NULL if it is not found. */
      static const ItemType* getItem(const IdType& id)
      {
         typename ItemMapType::const_iterator it = getItemMap().find(id);
         if (it != getItemMap().end())
         {
            return &(it->second);
         }
         return NULL;
      }

      /*****************************************************************************/
      /* Returns the list of Ids which are mapped to the given item */
      static void getIds(const ItemType& item, std::list<IdType>* ptrIds)
      {
         if (ptrIds != NULL)
         {
            typename ItemMapType::const_iterator it = getItemMap().begin();
            while (it != getItemMap().end())
            {
               if (it->second == item)
               {
                  ptrIds->push_back(it->first);
               }
               it++;
            }
         }
      }

   protected:
      typedef std::map<IdType, ItemType> ItemMapType;

      static ItemMapType& getItemMap()
      {
         static ItemMapType _itemMap;
         return _itemMap;
      }
};


}//namespace util
}//namespace hmibase


#endif
