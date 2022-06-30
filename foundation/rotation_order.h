// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

namespace hg {

/// Euler angles rotation order.
enum RotationOrder { RO_ZYX, RO_YZX, RO_ZXY, RO_XZY, RO_YXZ, RO_XYZ, RO_XY, RO_Default = RO_YXZ };

/// Return the reverse rotation order from a given input order.
RotationOrder ReverseRotationOrder(RotationOrder order);

} // namespace hg
