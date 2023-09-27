#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#include "TTLocalNotification.h"

using namespace std;

void TTLocalNotification::show(std::string message, int interval, int tag)
{
    UILocalNotification *notification = [[UILocalNotification alloc] init];
    
    notification.fireDate = [[NSDate date] dateByAddingTimeInterval:interval];
    notification.timeZone = [NSTimeZone defaultTimeZone];
    notification.alertBody = [NSString stringWithCString:message.c_str()
                                                encoding:NSUTF8StringEncoding];
    notification.alertAction = @"Open";
    notification.soundName = UILocalNotificationDefaultSoundName;
    notification.repeatInterval = 0;
    
    NSNumber* tag1 = [NSNumber numberWithInteger:tag];
    NSDictionary *infoDict = [NSDictionary dictionaryWithObject:tag1 forKey:@"ID"];
    notification.userInfo = infoDict;
    
    [[UIApplication sharedApplication] scheduleLocalNotification:notification];
    
    [notification release];

}

void TTLocalNotification::cancel(int tag)
{
    for(UILocalNotification *notification in [[UIApplication sharedApplication] scheduledLocalNotifications]) {
        if([[notification.userInfo objectForKey:@"ID"] integerValue] == tag) {
            [[UIApplication sharedApplication] cancelLocalNotification:notification];
        }
    }
}

#endif