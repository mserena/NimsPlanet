
package es.fourpoints.animalplanet;

import java.util.Calendar;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import bolts.Continuation;
import bolts.Task;

import com.parse.ParseAnalytics;
import com.parse.ParseInstallation;
import com.parse.ParseUser;
import com.screw.facebook.Facebook;
import com.soomla.cocos2dx.common.ServiceManager;
import com.soomla.cocos2dx.store.StoreService;

public class AnimalPlanetActivity extends Cocos2dxActivity {

	private static final String TAG = "AnimalPlanetActivity";
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		Log.d("AnimalPlanetActivity", "onCreate");
		
		ParseAnalytics.trackAppOpenedInBackground(getIntent());
		
		ParseUser.enableAutomaticUser();
		ParseUser theUser = ParseUser.getCurrentUser();
		final boolean isNew = theUser.getObjectId()==null;
		if (isNew) {
			ParseUser.logOut();
		}
		final ParseUser user = ParseUser.getCurrentUser();
		user.increment("RunCount");
		user.saveInBackground().continueWith(new Continuation<Void, Void>() {
			@Override
			public Void then(Task<Void> task) throws Exception {
				Exception ex = task.getError();
				if (ex!=null) {
					Log.e(TAG,"Error al guardar el usuario:",ex);
				}
				if (!task.isCancelled() && !task.isFaulted()) {
					if (user.isAuthenticated()) {
						ParseInstallation currentInstallation = ParseInstallation.getCurrentInstallation();
						currentInstallation.put("user", user);
						currentInstallation.saveInBackground(); //en este momento, tambien se guarda el usuario
					}
					if (isNew) {
						user.fetchInBackground();  //refrescamos el usuario
					}
				}
				return null;
			}
		});

		//instalacion de parse
		Facebook.onActivityCreate(this, savedInstanceState);
	}
	

	@Override
	protected void onPause() {
		super.onPause();
		Facebook.onActivityPause();
		//SOOMLA
		ServiceManager.getInstance().onPause();
	}

	@Override
	protected void onResume() {
		//SOOMLA
		ServiceManager.getInstance().onResume();
		super.onResume();
		Facebook.onActivityResume();
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		//quitarintegracion con screw para el login
		Facebook.onActivityResult(requestCode, resultCode, data);
	}
	
	@Override
	public Cocos2dxGLSurfaceView onCreateView() {

		Cocos2dxGLSurfaceView glSurfaceView = super.onCreateView();
		// para que la pantalla de idiomas se vea correctamente en android!!
		glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8);
		
		//evitar que la pantalla entre en sleep
		glSurfaceView.setKeepScreenOn(true);
		
		// initialize services
		final ServiceManager serviceManager = ServiceManager.getInstance();
		serviceManager.setActivity(this);
		serviceManager.setGlSurfaceView(glSurfaceView);
		serviceManager.registerService(StoreService.getInstance());

		return glSurfaceView;
	}

	//Local Notifications
	public static void showLocalNotification(String message, int interval, int tag) {
		Log.v(TAG, "showLocalNotification: " + tag);
		PendingIntent sender = getPendingIntent(message, tag);
		Log.v("showLocalNotification","Message: "+message);
		Calendar calendar = Calendar.getInstance();
		calendar.setTimeInMillis(System.currentTimeMillis());
		calendar.add(Calendar.SECOND, interval);
		
		AlarmManager am = (AlarmManager)getContext().getSystemService(ALARM_SERVICE);
		am.set(AlarmManager.RTC_WAKEUP, calendar.getTimeInMillis(), sender);
	}
	
	public static void cancelLocalNotification(int tag) {
		Log.v(TAG, "cancelLocalNotification: " + tag);
		PendingIntent sender = getPendingIntent(null,tag);
		AlarmManager am = (AlarmManager)getContext().getSystemService(ALARM_SERVICE);
		am.cancel(sender);
	}
	
	private static PendingIntent getPendingIntent(String message, int tag) {
		Intent i = new Intent(getContext().getApplicationContext(), LocalNotificationReceiver.class);
		i.putExtra("notification_id", tag);
		i.putExtra("message", message);
		Log.v("getPendingIntent","Message: "+message);
		PendingIntent sender = PendingIntent.getBroadcast(getContext(), tag, i, PendingIntent.FLAG_UPDATE_CURRENT);
		return sender;
	}
	
}
