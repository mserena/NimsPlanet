package com.soomla.cocos2dx.common;

import android.opengl.GLSurfaceView;

import com.soomla.data.KeyValueStorage;
import com.soomla.data.RewardStorage;
import com.soomla.rewards.BadgeReward;
import com.soomla.rewards.RandomReward;
import com.soomla.rewards.Reward;
import com.soomla.rewards.SequenceReward;
import com.soomla.Schedule;
import org.json.JSONException;
import org.json.JSONObject;

import java.lang.ref.WeakReference;


public class CoreService extends AbstractSoomlaService {

    private static CoreService INSTANCE = null;

    private boolean inited = false;

    public static CoreService getInstance() {
        if (INSTANCE == null) {
            synchronized (CoreService.class) {
                if (INSTANCE == null) {
                    INSTANCE = new CoreService();
                }
            }
        }
        return INSTANCE;
    }

    @SuppressWarnings("FieldCanBeLocal")
    private CoreEventHandlerBridge coreEventHandlerBridge;

    public CoreService() {
        coreEventHandlerBridge = new CoreEventHandlerBridge();

        DomainHelper.getInstance().registerTypeWithClassName(CommonConsts.JSON_JSON_TYPE_SCHEDULE, Schedule.class);
        DomainHelper.getInstance().registerTypeWithClassName(CommonConsts.JSON_JSON_TYPE_DATE_TIME_RANGE, Schedule.DateTimeRange.class);

        final DomainFactory domainFactory = DomainFactory.getInstance();

        domainFactory.registerCreator(CommonConsts.JSON_JSON_TYPE_BADGE, new DomainFactory.Creator<BadgeReward>() {
            @Override
            public BadgeReward create(JSONObject jsonObject) {
                try {
                    return new BadgeReward(jsonObject);
                } catch (JSONException e) {
                    throw new IllegalStateException(e);
                }
            }
        });
        domainFactory.registerCreator(CommonConsts.JSON_JSON_TYPE_RANDOM, new DomainFactory.Creator<RandomReward>() {
            @Override
            public RandomReward create(JSONObject jsonObject) {
                try {
                    return new RandomReward(jsonObject);
                } catch (JSONException e) {
                    throw new IllegalStateException(e);
                }
            }
        });
        domainFactory.registerCreator(CommonConsts.JSON_JSON_TYPE_SEQUENCE, new DomainFactory.Creator<SequenceReward>() {
            @Override
            public SequenceReward create(JSONObject jsonObject) {
                try {
                    return new SequenceReward(jsonObject);
                } catch (JSONException e) {
                    throw new IllegalStateException(e);
                }
            }
        });

        final NdkGlue ndkGlue = NdkGlue.getInstance();

        ndkGlue.registerCallHandler("CCCoreService::init", new NdkGlue.CallHandler() {
            @Override
            public void handle(JSONObject params, JSONObject retParams) throws Exception {
                CoreService.getInstance().init();
            }
        });
        ndkGlue.registerCallHandler("CCCoreService::getTimesGiven", new NdkGlue.CallHandler() {
            @Override
            public void handle(JSONObject params, JSONObject retParams) throws Exception {
                Reward reward = DomainFactory.getInstance().createWithJsonObject(params.getJSONObject("reward"));
                int result = RewardStorage.getTimesGiven(reward);
                retParams.put("return", result);
            }
        });
        ndkGlue.registerCallHandler("CCCoreService::setRewardStatus", new NdkGlue.CallHandler() {
            @Override
            public void handle(JSONObject params, JSONObject retParams) throws Exception {
                Reward reward = DomainFactory.getInstance().createWithJsonObject(params.getJSONObject("reward"));
                boolean give =  params.getBoolean("give");
                boolean notify =  params.getBoolean("notify");
                RewardStorage.setRewardStatus(reward, give, notify);
            }
        });
        ndkGlue.registerCallHandler("CCCoreService::getLastSeqIdxGiven", new NdkGlue.CallHandler() {
            @Override
            public void handle(JSONObject params, JSONObject retParams) throws Exception {
                Reward reward = DomainFactory.getInstance().createWithJsonObject(params.getJSONObject("reward"));
                int result = RewardStorage.getLastSeqIdxGiven((SequenceReward) reward);
                retParams.put("return", result);
            }
        });
        ndkGlue.registerCallHandler("CCCoreService::setLastSeqIdxGiven", new NdkGlue.CallHandler() {
            @Override
            public void handle(JSONObject params, JSONObject retParams) throws Exception {
                Reward reward = DomainFactory.getInstance().createWithJsonObject(params.getJSONObject("reward"));
                int idx =  params.getInt("idx");
                RewardStorage.setLastSeqIdxGiven((SequenceReward) reward, idx);
            }
        });

        ndkGlue.registerCallHandler("CCCoreService::getValue", new NdkGlue.CallHandler() {
            @Override
            public void handle(JSONObject params, JSONObject retParams) throws Exception {
                String key =  params.getString("key");
                String result = KeyValueStorage.getValue(key);
                retParams.put("return", result);
            }
        });
        ndkGlue.registerCallHandler("CCCoreService::setValue", new NdkGlue.CallHandler() {
            @Override
            public void handle(JSONObject params, JSONObject retParams) throws Exception {
                String key =  params.getString("key");
                String val =  params.getString("val");
                KeyValueStorage.setValue(key, val);
            }
        });
        ndkGlue.registerCallHandler("CCCoreService::deleteKeyValue", new NdkGlue.CallHandler() {
            @Override
            public void handle(JSONObject params, JSONObject retParams) throws Exception {
                String key =  params.getString("key");
                KeyValueStorage.deleteKeyValue(key);
            }
        });
        ndkGlue.registerCallHandler("CCCoreService::purge", new NdkGlue.CallHandler() {
            @Override
            public void handle(JSONObject params, JSONObject retParams) throws Exception {
                KeyValueStorage.purge();
            }
        });
    }

    private void init() {
        final GLSurfaceView glSurfaceView = glSurfaceViewRef.get();
        if (glSurfaceView != null) {
            coreEventHandlerBridge.setGlSurfaceView(glSurfaceView);
        }

        inited = true;
    }

    public void setGlSurfaceView(GLSurfaceView glSurfaceView) {
        if (inited) {
            coreEventHandlerBridge.setGlSurfaceView(glSurfaceView);
        } else {
            glSurfaceViewRef = new WeakReference<GLSurfaceView>(glSurfaceView);
        }
    }
}
