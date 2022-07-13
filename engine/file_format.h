// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include <stdint.h>

namespace hg {

static const uint32_t HarfangMagic = 0x46464748; // 'HGFF';

static const uint8_t NodeMarker = 0x00;
static const uint8_t SceneMarker = 0x01;
static const uint8_t NodesMarker = 0x02;

static const uint8_t MaterialMarker = 0x10;
static const uint8_t ModelMarker = 0x20;
static const uint8_t GeometryMarker = 0x30;

} // namespace hg
