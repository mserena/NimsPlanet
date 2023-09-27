

#import <Foundation/Foundation.h>


@interface ParamsProvider : NSObject
+ (id)sharedParamsProvider;

- (void)setParams:(NSDictionary *)params withKey:(NSString *)key;
- (NSDictionary *)getParamsForKey:(NSString *)key;
@end
