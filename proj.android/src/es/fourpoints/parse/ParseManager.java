/**
 * 
 */
package es.fourpoints.parse;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TimeZone;

import org.cocos2dx.lib.Cocos2dxHelper;

import android.util.Log;
import bolts.Continuation;
import bolts.Task;

import com.parse.ParseAnalytics;
import com.parse.ParseCloud;
import com.parse.ParseException;
import com.parse.ParseObject;
import com.parse.ParseQuery;
import com.parse.ParseUser;

/**
 * Parse integration
 * 
 * @author albert
 *
 */
public class ParseManager {
	private static final int MAX_OBJECTS_TO_SEND = 20;
	private static final String TAG = "ParseManager-JNI";
	
    /*
     * Retorna el id del objeto de usuario
     */
	public static String getUserParseIDParse() {
		final ParseUser user = ParseUser.getCurrentUser();
		final String idUser = user.getString("username");
		return idUser ==null ? "" : idUser;
	}
	
    /*
     * Setea el nivel más alto del usuario
     */
	public static void  setUserActualLevelParse(final int _level) {
		Log.d(TAG,"setUserActualLevelParse");
	
		final ParseUser user = ParseUser.getCurrentUser();
		user.put("highLevel", _level);
		user.saveInBackground().onSuccessTask(new Continuation<Void, Task<ParseObject>>() {
			@Override
			public Task<ParseObject> then(Task<Void> task) throws Exception {
				if (isUserConnectedToFacebookParse()) {
					String userFacebookId = user.getString("facebookId");
					ParseQuery<ParseObject> getUF = new ParseQuery<ParseObject>("UserFacebook");
					getUF.whereEqualTo("idFacebook", userFacebookId);
					return getUF.getFirstInBackground();
				}
				return null;
			}
		}).onSuccess(new Continuation<ParseObject, Void>() {
			@Override
			public Void then(Task<ParseObject> task) throws Exception {
				ParseObject object = task.getResult();
				if (object!=null) {
					object.put("highLevel", _level);
					object.saveInBackground();
				}
				return null;
			}
		});
	}

	 /*
     * Función para la migración de datos del usuario
     */
	public static void migrateUserLevelsParse(String[] _levelsInfo) {
		Log.d(TAG,"migrateUserLevelsParse");

		ParseUser user = ParseUser.getCurrentUser();
		List<ParseObject> objects = new ArrayList<ParseObject>();
		
		if (_levelsInfo!=null) {
			for (String levelInfo : _levelsInfo) {
				String[] datos = levelInfo.split("\\|");
				if (datos.length==5) {
					ParseObject levelInfoObj = new ParseObject("LevelsInfo");
					levelInfoObj.put("user", user);
					levelInfoObj.put("level", Integer.valueOf(datos[0]));
					levelInfoObj.put("score", Integer.valueOf(datos[1]));
					levelInfoObj.put("stars", Integer.valueOf(datos[2]));
					levelInfoObj.put("movements", Integer.valueOf(datos[3]));
					levelInfoObj.put("timePlaying", Integer.valueOf(datos[4]));
					objects.add(levelInfoObj);
				}
			}
		}
		
		ParseObject.pinAllInBackground("NoSyncCloud", objects).onSuccess(new Continuation<Void, Void>() {
			@Override
			public Void then(Task<Void> task) throws Exception {
		        callNativeSyncCloud();
		        //Llamamos al callback de la sincronización
		        callNativeMigrationDoneCallback();
				return null;
			}
		});

	}
	
	public static void migrateUserParse(final String[] _levelsInfo, final String _facebookId) {
		Log.d(TAG,"migrateUserParse");
	
		final int  actualHighLevel = _levelsInfo.length-1;
	   // Seteamos el highLevel de los usuarios
	    setUserActualLevelParse(actualHighLevel);
	 
		if (_facebookId!=null && !_facebookId.equals("")) {
			ParseUser user = ParseUser.getCurrentUser();
			user.put("facebookId", _facebookId);
			
			ParseObject newUserFacebook = new ParseObject("UserFacebook");
			newUserFacebook.put("idFacebook",_facebookId);
			newUserFacebook.put("highLevel",actualHighLevel);

			newUserFacebook.pinInBackground("UserFacebookNoSync").onSuccess(new Continuation<Void, Void>() {
				@Override
				public Void then(Task<Void> task) throws Exception {
					migrateUserLevelsParse(_levelsInfo);
					return null;
				}
			});
		} else {
			migrateUserLevelsParse(_levelsInfo);
		}
	}
	
    /*
     * Almacena la información del usuario sobre varios niveles, de manera masiva
     */
	private static void storeUserLevelsInfoParsePartitioned(final List<ParseObject> _levelsInfo, final int _indexInicial) {
		Log.d(TAG,"storeUserLevelsInfoParse(List, index)");
		final List<ParseObject> _listaToSend = _levelsInfo.subList(_indexInicial, Math.min(_indexInicial+MAX_OBJECTS_TO_SEND,_levelsInfo.size()) );
    	//Intentamos un guardado de objetos
    	ParseObject.saveAllInBackground(_listaToSend).continueWith(new Continuation<Void, Void>() {
    		@Override
    		public Void then(Task<Void> task) throws Exception {
    			if (!task.isCancelled() && !task.isFaulted()) {
    				//Marcamos los objectos de la lista enviada como no sincronizado con el cloud
    				ParseObject.unpinAllInBackground("NoSyncCloud",_listaToSend);
    				if (_indexInicial+MAX_OBJECTS_TO_SEND < _levelsInfo.size()) {
    					storeUserLevelsInfoParsePartitioned(_levelsInfo, _indexInicial+MAX_OBJECTS_TO_SEND); 
    				} else {
    					syncCloudDataFacebookParse();
    				}
    			} else {
    				Exception ex = task.getError();
    				Log.e(TAG,"Error al guardar en Parse",ex);
    				callNativeSyncFinishCallback();
    			}
    			return null;
    		}
    	});
	}
	
	
	/*
	 * Almacena la información del usuario sobre un nivel o la updatea
	 * en caso de que ya existiera y el score nuevo sea mayor.
	 */
	public static void storeUserLevelInfoParse(final int _levelId, final int _newScore, final int _stars, final int _movements, final int _timePlaying ){
		Log.d(TAG,"storeUserLevelInfoParse");
		
		final ParseUser user = ParseUser.getCurrentUser();
		
    	final ParseObject levelsInfoObj = new ParseObject("LevelsInfo");
    	levelsInfoObj.put("user",user);
    	levelsInfoObj.put("level",_levelId);
    	levelsInfoObj.put("score",_newScore);
    	levelsInfoObj.put("stars",_stars);
    	levelsInfoObj.put("movements",_movements);
    	levelsInfoObj.put("timePlaying",_timePlaying);

        //Marcamos el objeto como no sincronizado con el cloud e intentamos sincronizarlo
    	levelsInfoObj.pinInBackground("NoSyncCloud").onSuccessTask(new Continuation<Void, Task<Void>>() {
			@Override
			public Task<Void> then(Task<Void> task) throws Exception {
				return levelsInfoObj.saveInBackground();
			}
		}).onSuccess(new Continuation<Void, Void>() {
			@Override
			public Void then(Task<Void> task) throws Exception {
				levelsInfoObj.unpinInBackground("NoSyncCloud");
				return null;
			}
		});
   	}

	
    /*
     * Sincroniza los objetos locales no sincornizados con la nube
     */
	public static void syncCloudDataFacebookParse() {
		Log.d(TAG,"syncCloudDataFacebookParse");
		
	    // Intenta sincronizar la clase de niveles de cloud a local
	    // Si el usuario no esta conectado a Facebook, nunca hay que sincronizar de Cloud
		
	    if(isUserConnectedToFacebookParse()) {
	    	final ParseUser user = ParseUser.getCurrentUser();
	    	
	    	// Sincronizamos los datos con el usuario de Facebook
	    	String userFacebookId = user.getString("facebookId");

	    	//QUERY DE SOLO COGER LOS NIVELES POSTERIORES DE FACEBOOK AL DEL UA
	    	ParseQuery<ParseObject> getUFLevels = new ParseQuery<ParseObject>("LevelsInfoFacebook");
	    	getUFLevels.whereEqualTo("idFacebook", userFacebookId);
	    	getUFLevels.whereGreaterThan("level", user.getInt("highLevel"));
	    	getUFLevels.addAscendingOrder("level").setLimit(1000);
	    	
	    	getUFLevels.findInBackground().continueWith(new Continuation<List<ParseObject>, Void>() {
				@Override
				public Void then(Task<List<ParseObject>> task) throws Exception {
					List<ParseObject> objectsUF = task.getResult();
					if (!task.isCancelled() && !task.isFaulted()) {
						if (objectsUF.size()>0) {
							 final List<ParseObject> objectsToAnonymousUser = new ArrayList<ParseObject>();
							 for (ParseObject objUF : objectsUF) {
								 ParseObject newObjUA = new ParseObject("LevelsInfo");
								 newObjUA.put("user",user);
								 newObjUA.put("level",objUF.getInt("level"));
								 newObjUA.put("score",objUF.getInt("score"));
								 newObjUA.put("stars",objUF.getInt("stars"));
								 newObjUA.put("movements",objUF.getInt("movements"));
								 newObjUA.put("timePlaying",objUF.getInt("timePlaying"));
								 objectsToAnonymousUser.add(newObjUA);
							 }
							 
							 //salvado masivo de objetos
							 saveObjectsToAnonymousUserPartitioned(objectsToAnonymousUser, 0);

						} else {
							//no hay niveles superiores en FB respecto a UA
							callNativeSyncFinishCallback();	
						}
					} else {
						Exception ex = task.getError();
						Log.w(TAG,"Error obteniendo los niveles pde FB a sincronizar:"+ex.toString(),ex);
						callNativeSyncFinishCallback();	
					}
					return null;
				}
			});
	    } else {
	    	Log.d(TAG,"No conectado a Facebook, no se sincroniza la parte de facebook");
	    	callNativeSyncFinishCallback();
	    }
	}
	
	/*
	 * salvado masivo de objetos con control de numero maximo a enviar
	 */
	private static void saveObjectsToAnonymousUserPartitioned(final List<ParseObject> objectsToAnonymousUser, final int _indexInicial) {
		Log.d(TAG,"saveObjectsToanonymousUser");
		final List<ParseObject> _listaToSend = objectsToAnonymousUser.subList(_indexInicial, Math.min(_indexInicial+MAX_OBJECTS_TO_SEND, objectsToAnonymousUser.size()) );
		 ParseObject.saveAllInBackground(_listaToSend).continueWith(new Continuation<Void, Void>() {
			@Override
			public Void then(Task<Void> task) throws Exception {
				if (!task.isCancelled() && !task.isFaulted()) {
	   				if (_indexInicial+MAX_OBJECTS_TO_SEND < objectsToAnonymousUser.size()) {
	   					//faltan por guardar objetos
	   					saveObjectsToAnonymousUserPartitioned(objectsToAnonymousUser,_indexInicial+MAX_OBJECTS_TO_SEND );
    				} else {
						// Actualizamos el nivel actual del usuario
    					ParseUser user = ParseUser.getCurrentUser();
						int highLevel = user.getInt("highLevel") + objectsToAnonymousUser.size();
						user.put("highLevel", highLevel);
						user.saveInBackground();
						 
    					final int[] _stars = new int[objectsToAnonymousUser.size()];
    					for(int i=0; i<objectsToAnonymousUser.size();i++) {
    						_stars[i] = objectsToAnonymousUser.get(i).getInt("stars");
    					}
    					// Callback de conexión realizada
    					callNativeParseUpdateFacebookCallbackDone(_stars);
    					//callNativeSyncFinishCallback();
    				}
				} else {
					Exception ex = task.getError();
					Log.w(TAG,"Error guardando LevelsInfo:"+ex.toString(),ex);
					callNativeSyncFinishCallback();
				}
				return null;
			}
		});
	
	}
	
	
	public static void syncCloudDataParse() {
		Log.d(TAG,"syncCloudDataParse");
		 // Intena sincronizar la clase de niveles de local a cloud
		ParseQuery<ParseObject> queryLocalToCloud = new ParseQuery<ParseObject>("LevelsInfo");
		queryLocalToCloud.fromPin("NoSyncCloud");
		
		queryLocalToCloud.findInBackground().continueWith(new Continuation<List<ParseObject>, Void>() {
			@Override
			public Void then(Task<List<ParseObject>> task) throws Exception {
				if (!task.isCancelled() && !task.isFaulted()) {
					List<ParseObject> objects = task.getResult();
					if (objects!=null && objects.size()>0) {
			        	Log.d(TAG,"Se han encontrado "+objects.size()+" objetos de nivel no sincronizados...intentando sincronizarlos...");
			        	
			        	//Actualizamos el usuario de todos los objetos por si no se ha podido crear el usuario
			        	ParseUser user = ParseUser.getCurrentUser();
			        	for (ParseObject object : objects) {
							object.put("user", user);
						}
			        	
		                //Intentamos store de todos los objetos
			        	ParseManager.storeUserLevelsInfoParsePartitioned(objects,0);
					} else {
						syncCloudDataFacebookParse();
					}
				} else {
					Exception ex = task.getError();
					Log.w(TAG, "Error en consulta Levels -NoSyncCloud-  Info-syncCloudParse",ex);
					callNativeSyncFinishCallback();
				}
				return null;
			}
		});
	}
	
	public static void syncCloudParse() {
		Log.d(TAG,"syncCloudParse");
		
		final ParseUser user = ParseUser.getCurrentUser();
		
		user.saveInBackground().continueWithTask(new Continuation<Void, Task<ParseObject>>() {
			@Override
			public Task<ParseObject> then(Task<Void> task) throws Exception {
				if (!task.isCancelled() && !task.isFaulted()) {
					ParseQuery< ParseObject> queryLocalFU = new ParseQuery<ParseObject>("UserFacebook");
					queryLocalFU.fromPin("UserFacebookNoSync");
					return queryLocalFU.getFirstInBackground();
				} else {
					Exception ex = task.getError();
					Log.w(TAG,"Error guardando el usuario:"+ex.toString(),ex);
					callNativeSyncFinishCallback();  //ya se enviara desde la siguiente Task
					return null;
				}
			}
		}).continueWith(new Continuation<ParseObject, Void>() {
			@Override
			public Void then(Task<ParseObject> task) throws Exception {
				ParseObject object = task.getResult();
				Exception exTask = task.getError();
				ParseException ex = null;
				if (exTask !=null && exTask instanceof ParseException) {
					ex = (ParseException)exTask;
				}

				if (ex!=null && ex.getCode() == ParseException.OBJECT_NOT_FOUND) {
					syncCloudDataParse();
				} else if (ex==null && object!=null) {
	                  //Creamos el usuario de Facebook si no existe
					 ParseQuery<ParseObject> queryClodFU = new ParseQuery<ParseObject>("UserFacebook");
					 queryClodFU.whereEqualTo("idFacebook", object.getString("idFacebook"));
					 
					 queryClodFU.getFirstInBackground().continueWith(new Continuation<ParseObject, Void>() {
						@Override
						public Void then(Task<ParseObject> task) throws Exception {
							ParseObject object = task.getResult();
							Exception exTask = task.getError();
							ParseException ex = null;
							if (exTask !=null && exTask instanceof ParseException) {
								ex = (ParseException)exTask;
							}
							
							if (ex!=null && ex.getCode()==ParseException.OBJECT_NOT_FOUND) {
								object.put("highLevel",ParseUser.getCurrentUser().getInt("highLevel"));
								object.saveInBackground().continueWithTask(new Continuation<Void, Task<Void>>() {
									@Override
									public Task<Void> then(Task<Void> task) throws Exception {
										if (!task.isCancelled() && !task.isFaulted()) {
											ParseObject.unpinAllInBackground("UserFacebookNoSync");
											syncCloudDataParse();
										} else {
											callNativeSyncFinishCallback();
										}
										return null;
									}
								});
							} else if (ex==null) {
								ParseObject.unpinAllInBackground("UserFacebookNoSync");
								syncCloudDataParse();
							} else {
								callNativeSyncFinishCallback();
							}
							return null;
						}
					});

				} else {
					callNativeSyncFinishCallback();
				}
				return null;
			}
		});
	}
	
	/*
	 * Almacena la información del usuario (boosters, versión, monedas, plataforma...)
	 */
	public static void storeUserInfoParse(int _coins, String _boardBoostersInfo, String _gameBoostersInfo, String _appVersion){
		Log.d(TAG,"storeUserInfoParse");
		Log.d(TAG,"coins:"+_coins);
		Log.d(TAG,"_boardBoostersInfo:"+_boardBoostersInfo);
		Log.d(TAG,"_gameBoostersInfo:"+_gameBoostersInfo);
		Log.d(TAG,"_appVersion:"+_appVersion);
		
		ParseUser user = ParseUser.getCurrentUser();
		if (user!=null) {
			user.put("platform", "ANDROID");
			user.put("boostersBoardInfo", _boardBoostersInfo);
			user.put("boostersGameInfo", _gameBoostersInfo);
			user.put("appVersion", _appVersion);
			user.put("coins", _coins);
			user.saveInBackground();
		} else {
			Log.d(TAG,"Parse user is NULL!!");
		}
	}

	/*
	 * Obtiene la información del usuario
	 */
	public static void verifyUserCloudUpdatesParse() {
		Log.d(TAG,"verifyUserCloudUpdatesParse");
		
		final ParseUser user = ParseUser.getCurrentUser();
		String _nameUser = user.getUsername();
		if (_nameUser==null) {
			Log.d(TAG,"verifyUserCloudUpdatesParse se ha creado el usuario aún");
			//no se ha creado el usuario aun
			return;
		}
		
		ParseQuery<ParseUser> query = ParseUser.getQuery();
		query.whereEqualTo("username", _nameUser);

		query.getFirstInBackground().onSuccess(new Continuation<ParseUser, Void>() {
			@Override
			public Void then(Task<ParseUser> task) throws Exception {
				ParseUser object = task.getResult();
				final int coinsAux = object.getInt("coinsAux");
				if (coinsAux!=0) {
					user.put("coinsAux",0);
					user.saveInBackground().onSuccess(new Continuation<Void, Void>() {
						@Override
						public Void then(Task<Void> task) throws Exception {
							if (task.getError()==null) {
								callNativeUpdateUserCloudCallback(coinsAux);
							}
							return null;
						}
					});
				}
				return null;
			}
		});
	}
	
	/*
	 * Regresa si el usuario esta vinculado a Facebook
	 */
	public static boolean isUserConnectedToFacebookParse(){
		Log.d(TAG,"isUserConnectedToFacebookParse");
		ParseUser user = ParseUser.getCurrentUser();
		if(user==null) {
			return false;
		}
		String facebookIdUser = user.getString("facebookId");
		if (facebookIdUser==null || "".equals(facebookIdUser)) {
			return false;
		}
		return true;
	}

    /*
     * Desvincula lógicamente el usuario de Facebook
     */
	public static void disconnectUserToFacebookParse() {
		Log.d(TAG,"disconnectUserToFacebookParse");
		ParseUser user = ParseUser.getCurrentUser();
		if (user==null) {
			Log.w(TAG, "Error al desconectar usuario facebook:Usuario NULL");
			callNativeParseUnLinkedToFacebookCallbackFail();
			return;
		}
	
		user.put("facebookId", "");
		
		user.saveInBackground().continueWith(new Continuation<Void, Void>() {
			@Override
			public Void then(Task<Void> task) throws Exception {
				if (!task.isCancelled() && !task.isFaulted()) {
		             // Callback de desconexión a Facebook
					callNativeParseUnLinkedToFacebookCallbackDone();
				} else {
					Exception ex = task.getError();
		             // Callback de error de desconexión a Facebook
					Log.w(TAG, "Error al desconectar usuario facebook:"+ex.toString(),ex);
					callNativeParseUnLinkedToFacebookCallbackFail();					
				}
				return null;
			}
		});

	}
	
	/*
	 * Vincula una cuenta de Facebook al usuario
	 */
	public static void connectUserToFacebookParse(final String _userIdFacebook){
		Log.d(TAG,"connectUserToFacebookParse");

		if (ParseUser.getCurrentUser()==null) {
			callNativeParseLinkedToFacebookCallbackFail();
			return;
		}
		
		// Primero verificamos que no existan objetos sin sincronizar

		ParseQuery<ParseObject> queryLocalNoSyncObjs = new ParseQuery<ParseObject>("LevelsInfo");
		queryLocalNoSyncObjs.fromPin("NoSyncCloud");
		
		queryLocalNoSyncObjs.findInBackground().continueWith(new Continuation<List<ParseObject>, Void>() {
			@Override
			public Void then(Task<List<ParseObject>> task) throws Exception {
				List<ParseObject> objects = task.getResult();
				Exception exTask = task.getError();
				ParseException ex = null;
				if (exTask!=null && exTask instanceof ParseException) {
					ex = (ParseException)exTask;
				}
				if (exTask==null && objects.size()>0) {
					Log.w(TAG,"Se han encontrado "+objects.size() +" objetos de nivel no sincronizados...no podemos conectar a Facebook...");
					 // Sincronizamos
		             callNativeSyncCloud();
		             // Callback de error de conexión a Facebook
		             callNativeParseLinkedToFacebookCallbackFail();
				} else if(ex==null && objects.size()==0) {
					// Verificamos si existe el usuario de Facebook
					ParseQuery<ParseObject> queryUserFacebook = new ParseQuery<ParseObject>("UserFacebook");
					queryUserFacebook.whereEqualTo("idFacebook", _userIdFacebook);
					queryUserFacebook.getFirstInBackground().continueWith(new Continuation<ParseObject, Void>() {
						@Override
						public Void then(Task<ParseObject> task) throws Exception {
							final ParseObject object = task.getResult();
							Exception exTask = task.getError();
							ParseException ex = null;
							if (exTask!=null && exTask instanceof ParseException) {
								ex = (ParseException)exTask;
							}
							if (ex!=null && ex.getCode()==ParseException.OBJECT_NOT_FOUND) {
					            // No existe el usuario, creamos el registo para el usuario de Facebook y sincronizamos
								final ParseUser user = ParseUser.getCurrentUser();
								user.put("facebookId",_userIdFacebook);
								user.saveInBackground().continueWith(new Continuation<Void, Void>() {
									@Override
									public Void then(Task<Void> task) throws Exception {
										if (!task.isCancelled() && !task.isFaulted()) {
											//crear el usuario de FB
											ParseObject newUserFacebook = new ParseObject("UserFacebook");
											newUserFacebook.put("idFacebook", _userIdFacebook);
											newUserFacebook.put("highLevel", user.getInt("highLevel"));
											newUserFacebook.saveInBackground().continueWith(new Continuation<Void, Void>() {
												@Override
												public Void then(Task<Void> task) throws Exception {
													if (!task.isCancelled() && !task.isFaulted()) {
														// Sincronizamos el nuevo usuario de Facebook creado copiando todos los datos del usuario anonimo del dispositivo
														ParseQuery<ParseObject > getUserLevels = ParseQuery.getQuery("LevelsInfo");
														getUserLevels.whereEqualTo("user", user).addAscendingOrder("level").setLimit(1000);
														getUserLevels.findInBackground().continueWith(new Continuation<List<ParseObject>, Void>() {
															@Override
															public Void then(Task<List<ParseObject>> task) throws Exception {
																if (!task.isCancelled() && !task.isFaulted()) {
																	List<ParseObject> objects = task.getResult();
																	final List<ParseObject> objectsToFacebookUser = new ArrayList<ParseObject>();
																	for (ParseObject object : objects) {
																		ParseObject objToFacebook = new ParseObject("LevelsInfoFacebook");
																		objToFacebook.put("idFacebook",_userIdFacebook);
																		objToFacebook.put("level",object.getInt("level"));
																		objToFacebook.put("score",object.getInt("score"));
																		objToFacebook.put("stars",object.getInt("stars"));
																		objToFacebook.put("movements",object.getInt("movements"));
																		objToFacebook.put("timePlaying",object.getInt("timePlaying"));
																		objectsToFacebookUser.add(objToFacebook);
																	}
								                                     //Intentamos un guardado de objetos
																	saveObjectsToFacebookUserPartitioned(objectsToFacebookUser,0);
																} else {
					                                                // Callback de error de conexión a Facebook
																	callNativeParseLinkedToFacebookCallbackFail();
																}
																return null;
															}
														});
													} else {
														Exception ex = task.getError();
				                                       //limpiamos el usuario anonimo del facebook, porque no se ha podido crear el de FB
														Log.w(TAG,"Error creando usuario FB:"+ex.toString(),ex);
														user.put("facebookId", "");
														user.saveInBackground();
														callNativeParseLinkedToFacebookCallbackFail();														
													}
													return null;
												}
											});
											
										} else {
											// Callback de error de conexión a Facebook
											callNativeParseLinkedToFacebookCallbackFail();
										}
										return null;
									}
								});
								
							} else if (ex==null) {
						          // Se ha encontrado al usuario, vinculamos y sincronizamos con los datos de Facebook
								final ParseUser user = ParseUser.getCurrentUser();
								user.put("facebookId",_userIdFacebook);
								user.saveInBackground().continueWith(new Continuation<Void, Void>() {
									@Override
									public Void then(Task<Void> task) throws Exception {
										if (!task.isCancelled() && !task.isFaulted()) {
											final int _facebookHighLevel = object.getInt("highLevel");
											int _userHighLevel = user.getInt("highLevel");
											if(_facebookHighLevel > _userHighLevel) {
												// Tenemos que añadir datos al usuario anónimo
												ParseQuery<ParseObject> getUFLevels = ParseQuery.getQuery("LevelsInfoFacebook");
												getUFLevels.whereEqualTo("idFacebook", _userIdFacebook).whereGreaterThan("level", _userHighLevel).addAscendingOrder("level");
												getUFLevels.setLimit(1000);
												getUFLevels.findInBackground().continueWith(new Continuation<List<ParseObject>, Void>() {
													@Override
													public Void then(Task<List<ParseObject>> task) throws Exception {
														List<ParseObject> objectsUF = task.getResult();
														if (!task.isCancelled() && !task.isFaulted() && objectsUF.size()>0) {
															//Gana Facebook, solo actualizamos los objetos restantes
															final List<ParseObject> objectsToAnonymousUser = new ArrayList<ParseObject>();
															for (ParseObject objUF : objectsUF) {
																ParseObject newObjUA = new ParseObject("LevelsInfo");
																newObjUA.put("user",user);
																newObjUA.put("level",objUF.getInt("level"));
																newObjUA.put("score",objUF.getInt("score"));
																newObjUA.put("stars",objUF.getInt("stars"));
																newObjUA.put("movements",objUF.getInt("movements"));
																newObjUA.put("timePlaying",objUF.getInt("timePlaying"));
																objectsToAnonymousUser.add(newObjUA);
															}

															saveObjectsToAnonymousUserFromFBPartitioned(objectsToAnonymousUser, 0);
															
														} else {
															// Callback de error de conexión a Facebook
															callNativeParseLinkedToFacebookCallbackFail();
														}
														return null;
													}
												});
												
											} else if (_facebookHighLevel < _userHighLevel) {
												ParseQuery< ParseObject> getUserLevels = ParseQuery.getQuery("LevelsInfo");
												getUserLevels.whereEqualTo("user", user).whereGreaterThan("level", _facebookHighLevel).setLimit(1000);
												getUserLevels.findInBackground().continueWith(new Continuation<List<ParseObject>, Void>() {
													@Override
													public Void then(Task<List<ParseObject>> task) throws Exception {
														if (!task.isCancelled() && !task.isFaulted()) {
															deleteAllFromLevelsInfoPartitioned(task.getResult(), 0, _facebookHighLevel);
														} else {
															// Callback de error de conexión a Facebook
															callNativeParseLinkedToFacebookCallbackFail();
														}
														return null;
													}
												});
		
											} else {
				                                // Callback de conexión realizada
												callNativeParseLinkedToFacebookCallbackDoneNoChanges();
											}
										} else {
											// Callback de error de conexión a Facebook
											callNativeParseLinkedToFacebookCallbackFail();
										}
										return null;
									}
								});
								
							} else {
								// Callback de error de conexión a Facebook
								callNativeParseLinkedToFacebookCallbackFail();
							}
							
							return null;
						};
						
					});
				} else {
					Log.w(TAG,"Error consultando from Pin:"+ex.toString(),ex);
					callNativeParseLinkedToFacebookCallbackFail();
				}
				
				return null;
			}
		});
	
	}

	/*
	 * guardar de trozo en trozo
	 */
	private static void saveObjectsToFacebookUserPartitioned(final List<ParseObject> objectsToFacebookUser, final int _indexInicial) {
		
		Log.d(TAG,"saveObjectsToFacebookUser");
		final List<ParseObject> _listaToSend = objectsToFacebookUser.subList(_indexInicial, Math.min(_indexInicial+MAX_OBJECTS_TO_SEND, objectsToFacebookUser.size()) );
		
        //Intentamos un guardado de objetos
		ParseObject.saveAllInBackground(_listaToSend).continueWith(new Continuation<Void, Void>() {
			@Override
			public Void then(Task<Void> task) throws Exception {
				if (!task.isCancelled() && !task.isFaulted()) {
					if (_indexInicial+MAX_OBJECTS_TO_SEND < objectsToFacebookUser.size()) {
						//faltan por guardar objetos
						saveObjectsToFacebookUserPartitioned(objectsToFacebookUser, _indexInicial+MAX_OBJECTS_TO_SEND );
					} else {
						callNativeParseLinkedToFacebookCallbackDoneNoChanges();						
					}			
				} else {
					Exception ex = task.getError();
					Log.w(TAG,"Error guardanddo datos de levelsinfoFacebook:"+ex.toString(),ex);
                     // Callback de error de conexión a Facebook
					callNativeParseLinkedToFacebookCallbackFail();																				}
				return null;
			}
		});
	}

	
	/*
	 * salvado masivo de objetos con control de numero maximo a enviar
	 */
	private static void saveObjectsToAnonymousUserFromFBPartitioned(final List<ParseObject> objectsToAnonymousUser, final int _indexInicial) {
		Log.d(TAG,"saveObjectsToAnonymousUserFromFB");
		final List<ParseObject> _listaToSend = objectsToAnonymousUser.subList(_indexInicial, Math.min(_indexInicial+MAX_OBJECTS_TO_SEND, objectsToAnonymousUser.size()) );
		
		ParseObject.saveAllInBackground(_listaToSend).continueWith(new Continuation<Void, Void>() {
			@Override
			public Void then(Task<Void> task) throws Exception {
				if (!task.isCancelled() && !task.isFaulted()) {
					if (_indexInicial+MAX_OBJECTS_TO_SEND < objectsToAnonymousUser.size()) {
	   					//faltan por guardar objetos
						saveObjectsToAnonymousUserFromFBPartitioned(objectsToAnonymousUser,_indexInicial+MAX_OBJECTS_TO_SEND );
    				} else {
						// Actualizamos el nivel actual del usuario
    					ParseUser user = ParseUser.getCurrentUser();
						int highLevel = user.getInt("highLevel") + objectsToAnonymousUser.size();
						user.put("highLevel", highLevel);
						user.saveInBackground();
						
    					final int[] _stars = new int[objectsToAnonymousUser.size()];
    					
    					for(int i=0; i<objectsToAnonymousUser.size();i++) {
    						_stars[i] = objectsToAnonymousUser.get(i).getInt("stars");
    					}
                          // Callback de conexión realizada
    					callNativeParseLinkedToFacebookCallbackDoneAddLevels(_stars);
    				}
				} else {
					// Callback de error de conexión a Facebook
					callNativeParseLinkedToFacebookCallbackFail();
				}
				return null;
			}
		});
	}
	
	/*
	 * eliminar niveles que sobran del usuario anonimo
	 */
	private static void deleteAllFromLevelsInfoPartitioned(final List<ParseObject> levelsToDelete, final int _indexInicial, final int _facebookHighLevel) {
		Log.d(TAG,"deleteAllFromLevelsInfo");
		final List<ParseObject> _listaToDelete = levelsToDelete.subList(_indexInicial, Math.min(_indexInicial+MAX_OBJECTS_TO_SEND, levelsToDelete.size())) ;
		
		ParseObject.deleteAllInBackground(_listaToDelete).continueWith(new Continuation<Void, Void>() {
			@Override
			public Void then(Task<Void> task) throws Exception {
				if (!task.isCancelled() && !task.isFaulted()) {
					if (_indexInicial+MAX_OBJECTS_TO_SEND < levelsToDelete.size()) {
	   					//faltan por guardar objetos
						deleteAllFromLevelsInfoPartitioned(levelsToDelete,_indexInicial+MAX_OBJECTS_TO_SEND, _facebookHighLevel );
    				} else {
    					ParseUser user = ParseUser.getCurrentUser();
    					user.put("highLevel",_facebookHighLevel);
    					user.saveInBackground();
    					callNativeParseLinkedToFacebookAdjustLevelsCallbackDone(_facebookHighLevel);
    				}
				} else {
					// Callback de error de conexión a Facebook
					callNativeParseLinkedToFacebookCallbackFail();
				}
				return null;
			}
		});
	}
	
	/*
	 * Obtiene la información de los amigos del usuario de facebook
	 */
	public static void getFriendsHighLevelParse(final String _friendsIds){
		Log.d(TAG,"getFriendsHighLevelParse");
		
		ParseQuery< ParseObject> userFriends = new ParseQuery<ParseObject>("UserFacebook");
		Collection<String> values = new ArrayList<String>();
		String[] _friends = _friendsIds.split(",");
		for (String _friend : _friends) {
			values.add(_friend);
		}
		
		userFriends.whereContainedIn("idFacebook", values).selectKeys(Arrays.asList(new String[]{"idFacebook","highLevel"}));
		userFriends.setLimit(1000);
		userFriends.findInBackground().onSuccess(new Continuation<List<ParseObject>, Void>() {
			@Override
			public Void then(Task<List<ParseObject>> task) throws Exception {
				List<ParseObject> objects = task.getResult();
				Log.i(TAG,"Succesfully retrieved "+objects.size()+"");
				//llamar a un callback nativo con un array de strings
				final String[] _friendsHighLevel = new String[objects.size()];
				int i=0;
				for (ParseObject friend : objects) {
					String friendHighLevel =  friend.getString("idFacebook") + "=" + friend.getInt("highLevel");
					Log.d(TAG,"Friend highlevel["+i+"]:"+friendHighLevel);
					_friendsHighLevel[i++] = friendHighLevel;
				}
				callNativeGetFriendsHighLevelCallback(_friendsHighLevel);
				return null;
			}
		});
	}

	/*
	 * Obtiene el ranking de amigos de un determinado nivel
	 */
	public static void getRankingLevelParse(final int _level, String _friendsIds, String _meId){
		Log.d(TAG,"getRankingLevelParse");
		
		ParseQuery< ParseObject> userFriends = new ParseQuery<ParseObject>("LevelsInfoFacebook");
		Collection<String> values = new ArrayList<String>();
		String[] _friends = _friendsIds.split(",");
		for (String _friend : _friends) {
			values.add(_friend);
		}
		values.add(_meId);
		
		userFriends.whereContainedIn("idFacebook", values).whereEqualTo("level",_level);
		userFriends.addDescendingOrder("score");
		userFriends.setLimit(4);
		
		userFriends.findInBackground().continueWith(new Continuation<List<ParseObject>, Void>() {
			@Override
			public Void then(Task<List<ParseObject>> task) throws Exception {
				if (!task.isCancelled() && !task.isFaulted()) {
					List<ParseObject> objects = task.getResult();
					final String[] _rankingLevel = new String[objects.size()];
					int i=0;
					for (ParseObject levelInfo : objects) {
						String friendScore =  levelInfo.getString("idFacebook") + "=" + levelInfo.getInt("score");
						Log.d(TAG,"Friend score["+i+"]:"+friendScore);
						_rankingLevel[i++] = friendScore;
					}
					callNativeGetRankingLevelCallback(true, _rankingLevel);				
				} else {
					Exception ex = task.getError();
					//error en la consulta
					Log.w(TAG, "Error en la consulta de niveles:"+ex.toString(), ex);	
					final String[] _rankingLevel = new String[0];
					callNativeGetRankingLevelCallback(false, _rankingLevel);
				}
				return null;
			}
		});
	}

	
    /**
     * Envia un evento de analitica a Parse
     * @param _dimensions array de strings en formato "key=value"
     */
	public static void sendAnalyticEventParse(String _eventName, String[] _dimensions) {
		Log.d(TAG,"sendAnalyticEventParse");

		final Map<String, String> dimensions = new HashMap<String, String>();
		if (_dimensions!=null) {
			for (String dim : _dimensions) {
				String[] datos = dim.split("=");
				if (datos.length==2) {
					dimensions.put(datos[0],datos[1]);
				}
			}
		}
		// Send the dimensions to Parse along with the 'search' event
		ParseAnalytics.trackEventInBackground(_eventName, dimensions);
	}

	
	
	public static void setLevelStatisticsParse(int _level, boolean _win)
	{
		Map<String, Object> params = new HashMap<String, Object>();
		params.put("idLevel", _level);
		params.put("levelWin", _win ? 1: 0 );
		ParseCloud.callFunctionInBackground("levelStatistics", params);
	}
	
	public static void getServerTimeParse()
	{
		Log.e(TAG,"Time Zone JAVA "+TimeZone.getDefault().getID());
		Map<String, Object> params = new HashMap<String, Object>();
		params.put("timeZone",TimeZone.getDefault().getID());
		ParseCloud.callFunctionInBackground("getServerTime", params).continueWith(new Continuation<Object, Void>() {

			@Override
			public Void then(Task<Object> task) throws Exception {
				if (!task.isCancelled() && !task.isFaulted()) {
					String val = ""+task.getResult();
					Log.e("*******","Respuesta server: "+task.getResult()+"");
					callNativeGetServerTimeCallback(false,val);
				}else{
					Exception ex = task.getError();
					Log.w(TAG, "Error al obtener el tiempo del servidor:"+ex.toString(), ex);
					callNativeGetServerTimeCallback(true,"");
				}
			
				return null;
			}
		});
		
	    /*CCLOG("Current System Time Zone %s",[[[NSTimeZone systemTimeZone] name] UTF8String]);
	    [PFCloud callFunctionInBackground:@"getServerTime" withParameters:@{@"timeZone":[[NSTimeZone systemTimeZone] name]} block:^(NSString *object, NSError *error)
	    {
	        if(!error)
	        {
	            // Enviamos callback de respuesta con el string de la hora
	            TTParseManager::getInstance()->getServerTimeCallback(false,[object UTF8String]);
	        }
	        else
	        {
	            // Enviamos callback de error al intentar obtener la hora
	            TTParseManager::getInstance()->getServerTimeCallback(true,"");
	        }
	    }];*/
	}

	
	//llamadas a las funciones nativas
	
	private static void callNativeParseLinkedToFacebookCallbackDoneAddLevels(final int[] _stars) {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				nativeParseLinkedToFacebookCallbackDoneAddLevels(_stars);
			}
		});				
	}
	
	private static void callNativeParseLinkedToFacebookAdjustLevelsCallbackDone(final int _maxLevel) {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				nativeParseLinkedToFacebookAdjustLevelsCallbackDone(_maxLevel);
			}
		});				
	} 
	
	private static void callNativeParseLinkedToFacebookCallbackDoneNoChanges() {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				nativeParseLinkedToFacebookCallbackDoneNoChanges();
			}
		});				
	}

	private static void callNativeParseLinkedToFacebookCallbackFail() {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				nativeParseLinkedToFacebookCallbackFail();
			}
		});	
	}
	
	private static void callNativeParseUnLinkedToFacebookCallbackDone() {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				nativeParseUnLinkedToFacebookCallbackDone();
			}
		});		
    }
	
	private static void callNativeParseUnLinkedToFacebookCallbackFail() {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				nativeParseUnLinkedToFacebookCallbackFail();
			}
		});	
    }
	
	private static void callNativeParseUpdateFacebookCallbackDone(final int[] _stars) {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				nativeParseUpdateFacebookCallbackDone(_stars);
				nativeSyncFinishCallback();
			}
		});			
	}
	
	private static void callNativeGetFriendsHighLevelCallback(final String[] _friendsHighLevelInfo) {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				nativeGetFriendsHighLevelCallback(_friendsHighLevelInfo);
			}
		});			
	}
	
	private static void callNativeGetRankingLevelCallback(final boolean _connection, final String[] _rankingLevelInfo) {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				nativeGetRankingLevelCallback(_connection,_rankingLevelInfo );
			}
		});		
	}
	
	private static void callNativeUpdateUserCloudCallback(final int _coinsAux) {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				nativeUpdateUserCloudCallback(_coinsAux);
			}
		});		
	}
	
	private static void callNativeMigrationDoneCallback() {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				nativeMigrationDoneCallback();
			}
		});				
	}
	private static void callNativeSyncFinishCallback() {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				nativeSyncFinishCallback();
			}
		});				
	}

	private static void callNativeSyncCloud() {
			nativeSyncCloud();
//        Cocos2dxHelper.runOnGLThread(new Runnable() {
//			@Override
//			public void run() {
//			}
//		});				
	}
	
	private static void callNativeGetServerTimeCallback(final boolean _connection, final String _time) {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				Log.e("*******","Llamada a callNativeGetServerTimeCallback"+_connection+","+_time);
				nativeGetServerTimeCallback(_connection,_time);
			}
		});		
	}
	
	
    // native Callbacks 
	//private static native void nativeParseLinkedToFacebookCallbackDone(int[] _stars);//
    
	private static native void nativeParseLinkedToFacebookCallbackDoneAddLevels(int[] _stars);
    private static native void nativeParseLinkedToFacebookCallbackDoneNoChanges();
    private static native void nativeParseLinkedToFacebookAdjustLevelsCallbackDone(int _maxLevel);;
    
	private static native void nativeParseLinkedToFacebookCallbackFail();//
	private static native void nativeParseUnLinkedToFacebookCallbackDone();//
	private static native void nativeParseUnLinkedToFacebookCallbackFail();//
	private static native void nativeParseUpdateFacebookCallbackDone(int[] _stars);//
	private static native void nativeGetFriendsHighLevelCallback(String[] _friendsHighLevelInfo);//
	private static native void nativeGetRankingLevelCallback(boolean _connection,String[] _rankingLevelInfo);//
	private static native void nativeUpdateUserCloudCallback(int _coinsAux);//
	private static native void nativeMigrationDoneCallback();//
	private static native void nativeSyncFinishCallback();
	private static native void nativeSyncCloud();	
	private static native void nativeGetServerTimeCallback(boolean _connection,String _rankingLevelInfo);//
}
