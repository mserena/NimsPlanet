
#import <MacTypes.h>
#import "ParamsProvider.h"


@interface ParamsProvider ()
@property(nonatomic, retain) id paramsDict;
@end

@implementation ParamsProvider {

}

+ (id)sharedParamsProvider {
    static ParamsProvider *sharedParamsProvider = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedParamsProvider = [[self alloc] init];
    });
    return sharedParamsProvider;
}

- (id)init {
    self = [super init];
    if (self) {
        self.paramsDict = [NSMutableDictionary dictionary];
    }

    return self;
}

- (void)setParams:(NSDictionary *)params withKey:(NSString *)key {
    [self.paramsDict setObject:params forKey:key];
}

- (NSDictionary *)getParamsForKey:(NSString *)key {
    return [self.paramsDict objectForKey:key];
}

@end
