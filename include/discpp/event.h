#ifndef DISCPP_EVENT_H
#define DISCPP_EVENT_H

namespace discpp {
    class Shard;

	class Event {
	public:
	    explicit Event(Shard& shard) : shard(shard) {}

	    virtual int GetEventType() const {
	        return 0;
	    }

        Shard& shard;
	};
}

#endif