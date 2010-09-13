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

#ifndef EXPORTENGINE_H
#define EXPORTENGINE_H

#include "../plugin.h"

class ExportEngine : public Plugin
{
public:
  ExportEngine();
};

Q_DECLARE_INTERFACE(ExportEngine, "dbmaster.ExportEngine")

#endif // EXPORTENGINE_H
