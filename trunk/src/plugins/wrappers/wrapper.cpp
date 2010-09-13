/*
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 */


#include "wrapper.h"

QList<Wrapper*> Wrapper::wrappers;

Wrapper::Wrapper()
  : Plugin()
{
}

//QList<Wrapper*> Wrapper::availableFor(QString driver)
//{
//  QList<Wrapper*> ret;
//  foreach(Wrapper *w, wrappers)
//    if(w->driver() == driver)
//      ret << w;

//  return ret;
//}
