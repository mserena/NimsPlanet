//Función para evitar duplicados en el campo de nivel LevelsInfo
Parse.Cloud.beforeSave("LevelsInfo", function(request, response) {
    if (!request.object.isNew()) {
      // Let existing object updates go through
      response.success();
	  return;
    }
    
  	var query = new Parse.Query("LevelsInfo");
	query.equalTo("level", request.object.get("level"));
	query.equalTo("user", request.object.get("user"));
	query.first({
    	success: function(object) {
    		if(object)
    		{
    			var scoreOnParse = object.get("score");
    			var scoreOnRequest = request.object.get("score");
    			if(scoreOnParse > scoreOnRequest){
    				request.object.set("score",scoreOnParse);
    				request.object.set("stars",object.get("stars"));
    				request.object.set("movements",object.get("movements"));
    				request.object.set("timePlaying",object.get("timePlaying"));				
    			}
    			//console.log("------> LevelsInfo Se destruye el Objeto "+object.get("objectId")+" nivel: "+object.get("level"));
    			object.destroy();
    		}
    			
    		//Verificamos si hay que actualizar también en la tabla de Facebook
    		var user = request.user;
    		var facebookId = user.get("facebookId");
    		if(facebookId != ""){
    			var query = new Parse.Query("LevelsInfoFacebook");
				query.equalTo("level", request.object.get("level"));
				query.equalTo("idFacebook",facebookId);
				query.first({
					success: function(object) {
						if(object)
						{
							object.set("score",request.object.get("score"));
							object.set("stars",request.object.get("stars"));
							object.set("movements",request.object.get("movements"));
							object.set("timePlaying",request.object.get("timePlaying"));
							object.save({
                    			success:function() {
                         			response.success();
                    			},
                    			error:function(error) {
                         			response.error("Error: Code -> "+error.code+" : Message -> "+error.message);
                    			}
                			});
						}
						else
						{
							var LevelsFacebook = Parse.Object.extend("LevelsInfoFacebook");
							var levelInfo = new LevelsFacebook();
							levelInfo.set("idFacebook", user.get("facebookId"));
							levelInfo.set("level",request.object.get("level"));
							levelInfo.set("score",request.object.get("score"));
							levelInfo.set("stars",request.object.get("stars"));
							levelInfo.set("movements",request.object.get("movements"));
							levelInfo.set("timePlaying",request.object.get("timePlaying"));
							//console.log("------> Intentamos gardar objeto de nivel: "+levelInfo.get("level")+" en LevelsInfoFacebook");
							levelInfo.save({
                    			success:function() {
                         			response.success();
                    			},
                    			error:function(error) {
                         			response.error("Error: Code -> "+error.code+" : Message -> "+error.message);
                    			}
                			});
						}
					},
					error: function(error) {
						response.error("Error: Code -> "+error.code+" : Message -> "+error.message);
					}
				});
    		}else{
    			response.success();	
    		}
    	},
    	error: function(error) {
      		response.error("Error: Code -> "+error.code+" : Message -> "+error.message);
    	}
  	});
});

//Función para evitar duplicados en el campo de nivel LevelsInfoFacebook
Parse.Cloud.beforeSave("LevelsInfoFacebook", function(request, response) {
	var user = request.user;
	var idFacebookRequest = request.object.get("idFacebook");
	if (request.master===false) {
		if(idFacebookRequest !='' && user.get("facebookId") != idFacebookRequest){
			response.error();
			return;
		}		
	}
	response.success();
});

//Función para setear los campos por defecto en los usuarios nuevos
Parse.Cloud.beforeSave(Parse.User, function(request, response) {
	if(request.object.isNew()){
		request.object.set("facebookId","");
  		request.object.set("coins",0);
  		request.object.set("coinsAux",0);
  		request.object.set("highLevel",-1);
  		request.object.set("boostersBoardInfo","0,0,0,0");
  		request.object.set("boostersGameInfo","0,0,0,0,0");
	}
	response.success();
});

//Función para setear los campos por defecto en los usuarios nuevos de Facebook
Parse.Cloud.beforeSave("UserFacebook", function(request, response) {
	var user = request.user;
	var idFacebookRequest = request.object.get("idFacebook");
	if (request.master===false) {
		if(idFacebookRequest !='' && user.get("facebookId") != idFacebookRequest){
			response.error();
			return;
		}
	}
	
	if (!request.object.isNew()) {
      // Let existing object updates go through
      response.success();
	  return;
    }
	
	var query = new Parse.Query("UserFacebook");
	query.equalTo("idFacebook", idFacebookRequest);
	query.first({
		success: function(object) {
			if(object)
			{
			    object.destroy({
                    success:function() {
                         response.success();
                    },
                    error:function(error) {
                         response.error("Error: Code -> "+error.code+" : Message -> "+error.message);
                    }
                });
			}else{
				response.success();
			}
		},
		error: function(error) {
			response.error("Error: Code -> "+error.code+" : Message -> "+error.message);
		}
	});
});

//Función para cálculos en levelStatistics
Parse.Cloud.beforeSave("LevelStatistics", function(request, response) {
	var plays = request.object.get("numPlays");
	var wins = request.object.get("numWins");
	var percentage = (wins/plays)*100;
	request.object.set("winPercent",percentage);
	response.success();
});

//Función para las estadísicas de nivel
Parse.Cloud.define("levelStatistics", function(request, response) {
  var query = new Parse.Query("LevelStatistics");
  query.equalTo("idLevel",request.params.idLevel);
  query.first({
    	success: function(object) {
			if(object){
			    object.increment("numPlays");
			    if(request.params.levelWin){
			    	object.increment("numWins");
			    }else{
			    	object.increment("numLoses");
			    }
			    object.save();
			    response.success();
			}else{
				var LevelStatistics = Parse.Object.extend("LevelStatistics");
				var levelInfo = new LevelStatistics();
				levelInfo.set("idLevel",request.params.idLevel);
				levelInfo.set("numPlays",1);
				if(request.params.levelWin){
					levelInfo.set("numWins",1);
					levelInfo.set("numLoses",0);
				}else{
					levelInfo.set("numWins",0);
					levelInfo.set("numLoses",1);
				}
				levelInfo.save();
				response.success();
			}
    	},
    	error: function(error) {
			response.error("Error: Code -> "+error.code+" : Message -> "+error.message);
		}
  });
});

//Función para obtener el tiempo de servidor
Parse.Cloud.define("getServerTime", function(request, response) {
	var moment = require("cloud/moment/moment-timezone-with-data.js");
	var now = moment().tz(request.params.timeZone).valueOf();
	response.success(now+"");
});

