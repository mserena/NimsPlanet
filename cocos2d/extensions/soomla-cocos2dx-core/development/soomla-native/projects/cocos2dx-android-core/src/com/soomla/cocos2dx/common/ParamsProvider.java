package com.soomla.cocos2dx.common;

import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;


public class ParamsProvider {

    private static ParamsProvider INSTANCE = null;

    private Map<String, JSONObject> paramsMap = new HashMap<String, JSONObject>();

    public static ParamsProvider getInstance() {
        if (INSTANCE == null) {
            synchronized (ParamsProvider.class) {
                if (INSTANCE == null) {
                    INSTANCE = new ParamsProvider();
                }
            }
        }
        return INSTANCE;
    }

    public void setParams(String key, JSONObject params) {
        paramsMap.put(key, params);
    }

    public JSONObject getParams(String key) {
        return paramsMap.get(key);
    }
}
