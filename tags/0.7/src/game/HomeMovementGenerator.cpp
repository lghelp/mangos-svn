/* 
 * Copyright (C) 2005,2006,2007 MaNGOS <http://www.mangosproject.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "HomeMovementGenerator.h"
#include "Creature.h"
#include "Traveller.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "DestinationHolderImp.h"

void
HomeMovementGenerator::Initialize(Creature & owner)
{
    _setTargetLocation(owner);
}

void
HomeMovementGenerator::Reset(Creature &)
{
}

void
HomeMovementGenerator::_setTargetLocation(Creature & owner)
{
    if( !&owner )
        return;

    if( owner.hasUnitState(UNIT_STAT_ROOT) || owner.hasUnitState(UNIT_STAT_STUNDED) )
        return;

    float x, y, z;
    owner.GetRespawnCoord(x, y, z);
    Traveller<Creature> traveller(owner);

    DestinationHolder<Traveller<Creature> > i_destinationHolder;

    uint32 travel_time = i_destinationHolder.SetDestination(traveller, x, y, z);
    modifyTravelTime(travel_time);
    traveller.Relocation(x,y,z);
    owner.clearUnitState(UNIT_STAT_ALL_STATE);
}

void
HomeMovementGenerator::_reLocate(Creature &owner)           // resend clients the creature's actual position
{

    if (owner.GetTypeId() == TYPEID_UNIT)
    {
        sLog.outDebug("HomeMovementGenerator::_reLocate() called, where Unit.GetGUIDLow()=%d", owner.GetGUIDLow());
        ObjectAccessor::UpdateObjectVisibility(&owner);
    }
}

bool
HomeMovementGenerator::Update(Creature &owner, const uint32& time_diff)
{

    if (time_diff > i_travel_timer)
    {
        _reLocate(owner);
        owner.setMoveRunFlag(false);
        return false;
    }

    i_travel_timer -= time_diff;

    return true;

}
