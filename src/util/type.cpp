#include "type.hpp"

using namespace std;

ostream& operator<<(ostream& os, const EdgeType& edge_type) {
  os << "EdgeType"
     << "(" << edge_type.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const LayerId& id) {
  os << "LayerId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const CellId& id) {
  os << "CellId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const CellPinId& id) {
  os << "CellPinId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const RowId& id) {
  os << "RowId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const SiteId& id) {
  os << "SiteId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const FenceRegionId& id) {
  os << "FenceRegionId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const RailId& id) {
  os << "RailId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const InstanceId& id) {
  os << "InstanceId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const InstancePinId& id) {
  os << "InstancePinId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const PinId& id) {
  os << "PinId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const BlockageId& id) {
  os << "BlockageId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const NetId& id) {
  os << "NetId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const GroupId& id) {
  os << "GroupId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const IntervalId& id) {
  os << "IntervalId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const SubInstanceId& id) {
  os << "SubInstanceId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const BinId& id) {
  os << "BinId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const BinVertexId& id) {
  os << "BinVertexId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const HotspotId& id) {
  os << "HotspotId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const SpreaderId& id) {
  os << "SpreaderId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const WindowId& id) {
  os << "WindowId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const WindowNodeId& id) {
  os << "WindowNodeId"
     << "(" << id.t << ")";

  return os;
}

ostream& operator<<(ostream& os, const NetType& type) {
  switch (type) {
    case NetType::NONE: {
      os << "NONE";
      break;
    }
    case NetType::POWER: {
      os << "POWER";
      break;
    }
    case NetType::GROUND: {
      os << "GROUND";
      break;
    }
    default: { break; }
  }

  return os;
}

ostream& operator<<(ostream& os, const PinType& type) {
  switch (type) {
    case PinType::NONE: {
      os << "NONE";
      break;
    }
    case PinType::INPUT: {
      os << "INPUT";
      break;
    }
    case PinType::OUTPUT: {
      os << "OUTPUT";
      break;
    }
    default: { break; }
  }

  return os;
}

ostream& operator<<(ostream& os, const Orientation& orientation) {
  switch (orientation) {
    case Orientation::N: {
      os << "N";
      break;
    }
    case Orientation::S: {
      os << "S";
      break;
    }
    case Orientation::W: {
      os << "W";
      break;
    }
    case Orientation::E: {
      os << "E";
      break;
    }
    case Orientation::FN: {
      os << "FN";
      break;
    }
    case Orientation::FS: {
      os << "FS";
      break;
    }
    case Orientation::FW: {
      os << "FW";
      break;
    }
    case Orientation::FE: {
      os << "FE";
      break;
    }
    default: { break; }
  }

  return os;
}

Orientation FlipOrientation(Orientation orientation) {
  const int num_orientations = 8;

  return static_cast<Orientation>((static_cast<int>(orientation) + 4) %
                                  num_orientations);
}
