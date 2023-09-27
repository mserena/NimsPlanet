

#import <Foundation/Foundation.h>


@interface ServiceManager : NSObject
+ (id)sharedServiceManager;

- (void)registerService:(NSObject *)service;
@end
