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


#ifndef PLUGIN_H
#define PLUGIN_H

#include <QString>
#include <QtPlugin>

class Plugin
{
public:
  Plugin();

  virtual QString title()   =0;
  virtual QString version() =0;
};

Q_DECLARE_INTERFACE(Plugin, "dbmaster.Plugin")

#endif // PLUGIN_H
