#pragma once

#include "../config.h"

#include "ofxFiducial.h"
#ifdef USE_FINGER_TRACKING
#include "ofxFinger.h"
#endif

namespace ofxFiducialBacked {
	namespace gui {
		class GuiPage;
	}
}

namespace ofxFiducialBacked {
	namespace types {
		namespace gui {
			typedef int												fiducial_key_t;
			typedef ofxFiducialBacked::gui::GuiPage*				fiducial_value_t;
			typedef pair<fiducial_key_t, fiducial_value_t>			fiducial_pair_t;

			typedef list<fiducial_pair_t >							fiducial_set_t;
			typedef map<fiducial_key_t, fiducial_value_t>			fiducial_graph_t;
			typedef map<fiducial_key_t, pair<ofPoint, ofPoint> >	fiducial_rays_t;

			typedef int												fiducial_key_t;
			typedef ofxFiducialBacked::gui::GuiPage*				fiducial_value_t;
			typedef pair<fiducial_key_t, fiducial_value_t>			fiducial_pair_t;

			class fiducial_pair_by_key {
			public:
				const fiducial_key_t key;
				fiducial_pair_by_key(const fiducial_key_t _key) : key(_key) {}
				bool operator()(fiducial_pair_t& p) {
					return (p.first==key);
				}
			};
			class fiducial_pair_by_value {
			public:
				const fiducial_value_t value;
				fiducial_pair_by_value(const fiducial_value_t _value) : value(_value) {}
				bool operator()(fiducial_pair_t& p) {
					return (p.second==value);
				}
			};
		}
	}
}
