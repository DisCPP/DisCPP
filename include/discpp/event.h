#ifndef DISCPP_EVENT_H
#define DISCPP_EVENT_H

namespace discpp {
	class Event {
	public:
	    virtual int GetEventType() const {
	        return 0;
	    }
	};
}

#endif