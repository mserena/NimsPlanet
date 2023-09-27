package com.soomla.cocos2dx.common;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import org.json.JSONObject;

import java.lang.ref.WeakReference;
import java.util.HashSet;
import java.util.Set;


public class ServiceManager implements SoomlaService {

    private static ServiceManager INSTANCE = null;

    private WeakReference<GLSurfaceView> glSurfaceViewRef = new WeakReference<GLSurfaceView>(null);

    public static ServiceManager getInstance() {
        if (INSTANCE == null) {
            synchronized (ServiceManager.class) {
                if (INSTANCE == null) {
                    INSTANCE = new ServiceManager();
                }
            }
        }
        return INSTANCE;
    }

    @SuppressWarnings("MismatchedQueryAndUpdateOfCollection")
    private Set<SoomlaService> services = new HashSet<SoomlaService>();

    public void registerService(SoomlaService service) {
        services.add(service);
        service.setGlSurfaceView(glSurfaceViewRef.get());
    }

    ServiceManager() {
        final CoreService coreService = CoreService.getInstance();
        this.registerService(coreService);

        final NdkGlue ndkGlue = NdkGlue.getInstance();
        ndkGlue.registerCallHandler("CCServiceManager::setCommonParams", new NdkGlue.CallHandler() {
            @Override
            public void handle(JSONObject params, JSONObject retParams) throws Exception {
                JSONObject commonParams = params.optJSONObject("params");
                ParamsProvider.getInstance().setParams("common", commonParams);
            }
        });
    }

    public void setActivity(Activity activity) {
        NdkGlue.getInstance().setActivity(activity);
    }

    public void setGlSurfaceView(GLSurfaceView glSurfaceView) {
        glSurfaceViewRef = new WeakReference<GLSurfaceView>(glSurfaceView);
        for (SoomlaService service : services) {
            service.setGlSurfaceView(glSurfaceView);
        }
    }

    @Override
    public void onPause() {
        for (SoomlaService service : services) {
            service.onPause();
        }
    }

    @Override
    public void onResume() {
        for (SoomlaService service : services) {
            service.onResume();
        }
    }
}
