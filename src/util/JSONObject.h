#ifndef __JSON_OBJECT_H__
#define __JSON_OBJECT_H__

#include "json/single_include/nlohmann/json.hpp"

namespace tse{

typedef nlohmann::json JSON;

class JSONObject{

public: 
	virtual void load(const JSON& data){}
	virtual JSON save(){ return JSON(); }

};

}

#endif // !__JSON_OBJECT_H__
