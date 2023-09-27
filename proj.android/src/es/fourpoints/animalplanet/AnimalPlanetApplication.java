/**
 * 
 */
package es.fourpoints.animalplanet;

import com.parse.Parse;
import com.parse.ParseInstallation;
import com.soomla.SoomlaApp;

/**
 * @author albert
 *
 */
public class AnimalPlanetApplication extends SoomlaApp {

	/* (non-Javadoc)
	 * @see com.soomla.SoomlaApp#onCreate()
	 */
	@Override
	public void onCreate() {
		super.onCreate();
		
		// Enable Local Datastore.
		Parse.enableLocalDatastore(this);
		//NIMSPLANET 
//		Parse.initialize(this, "41M0pgdIbXjNyEoe2oZ8xeKCVkZ2B0GRaBcdlAsn", "Xsn8uTe5uIAQ34f5B36cOjQsFjXD7QH4wW7AJa8w");
	
		//NIMSPLANET12		
		Parse.initialize(this, "ampK0P3YFLjwsM5URwFnCT1WkLJJieRlxZrcPaVX", "FZr8WJjQX6fVtnCVTRm0MoXrD4aumAn7B7BC5rYK");

		//NIMSPLANETPRO		
//		Parse.initialize(this, "pFF4vZtoipjEOzonuTd4nqrxb33XwE3D4meCXnIC", "deqiiBUBlsJBLGyaRCeiv7eCH69ANdNY0FL6tzwt");
	
		ParseInstallation currentInstallation = ParseInstallation.getCurrentInstallation();
		currentInstallation.saveInBackground();
		
	}

}
