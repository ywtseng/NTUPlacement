#ifndef TYPE_HPP
#define TYPE_HPP

#include <boost/serialization/strong_typedef.hpp>

#include <iostream>

BOOST_STRONG_TYPEDEF(int, EdgeType)
BOOST_STRONG_TYPEDEF(int, LayerId)
BOOST_STRONG_TYPEDEF(int, CellId)
BOOST_STRONG_TYPEDEF(int, CellPinId)
BOOST_STRONG_TYPEDEF(int, RowId)
BOOST_STRONG_TYPEDEF(int, SiteId)
BOOST_STRONG_TYPEDEF(int, FenceRegionId)
BOOST_STRONG_TYPEDEF(int, RailId)
BOOST_STRONG_TYPEDEF(int, InstanceId)
BOOST_STRONG_TYPEDEF(int, InstancePinId)
BOOST_STRONG_TYPEDEF(int, PinId)
BOOST_STRONG_TYPEDEF(int, BlockageId)
BOOST_STRONG_TYPEDEF(int, NetId)
BOOST_STRONG_TYPEDEF(int, GroupId)
BOOST_STRONG_TYPEDEF(int, IntervalId)
BOOST_STRONG_TYPEDEF(int, SubInstanceId)
BOOST_STRONG_TYPEDEF(int, BinId)
BOOST_STRONG_TYPEDEF(int, BinVertexId)
BOOST_STRONG_TYPEDEF(int, HotspotId)
BOOST_STRONG_TYPEDEF(int, SpreaderId)
BOOST_STRONG_TYPEDEF(int, WindowId)
BOOST_STRONG_TYPEDEF(int, WindowNodeId)
BOOST_STRONG_TYPEDEF(int, LineStyleId)
BOOST_STRONG_TYPEDEF(int, ArrowStyleId)
BOOST_STRONG_TYPEDEF(int, VariableId)

enum class NetType { NONE, POWER, GROUND };

enum class PinType { NONE, INPUT, OUTPUT, INOUT };

// Do not change the order!
enum class Orientation { N, W, S, E, FN, FW, FS, FE };

std::ostream& operator<<(std::ostream& os, const EdgeType& edge_type);
std::ostream& operator<<(std::ostream& os, const LayerId& id);
std::ostream& operator<<(std::ostream& os, const CellId& id);
std::ostream& operator<<(std::ostream& os, const CellPinId& id);
std::ostream& operator<<(std::ostream& os, const RowId& id);
std::ostream& operator<<(std::ostream& os, const SiteId& id);
std::ostream& operator<<(std::ostream& os, const FenceRegionId& id);
std::ostream& operator<<(std::ostream& os, const RailId& id);
std::ostream& operator<<(std::ostream& os, const InstanceId& id);
std::ostream& operator<<(std::ostream& os, const InstancePinId& id);
std::ostream& operator<<(std::ostream& os, const PinId& id);
std::ostream& operator<<(std::ostream& os, const BlockageId& id);
std::ostream& operator<<(std::ostream& os, const NetId& id);
std::ostream& operator<<(std::ostream& os, const GroupId& id);
std::ostream& operator<<(std::ostream& os, const IntervalId& id);
std::ostream& operator<<(std::ostream& os, const SubInstanceId& id);
std::ostream& operator<<(std::ostream& os, const BinId& id);
std::ostream& operator<<(std::ostream& os, const BinVertexId& id);
std::ostream& operator<<(std::ostream& os, const HotspotId& id);
std::ostream& operator<<(std::ostream& os, const SpreaderId& id);
std::ostream& operator<<(std::ostream& os, const WindowId& id);
std::ostream& operator<<(std::ostream& os, const WindowNodeId& id);
std::ostream& operator<<(std::ostream& os, const VariableId& id);

std::ostream& operator<<(std::ostream& os, const NetType& type);
std::ostream& operator<<(std::ostream& os, const PinType& type);
std::ostream& operator<<(std::ostream& os, const Orientation& orientation);

Orientation FlipOrientation(Orientation orientation);

#endif
