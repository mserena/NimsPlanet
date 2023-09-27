
#import "ServiceManager.h"
#import "NdkGlue.h"
#import "ParamsProvider.h"
#import "CoreService.h"


@interface ServiceManager ()
@property(nonatomic, retain) NSMutableArray *services;
@end

@implementation ServiceManager {

}

+ (id)sharedServiceManager {
    static ServiceManager *sharedServiceManager = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedServiceManager = [[self alloc] init];
    });
    return sharedServiceManager;
}

- (id)init {
    self = [super init];
    if (self) {
        _services = [NSMutableArray array];

        [self registerService:[CoreService sharedCoreService]];

        [[NdkGlue sharedInstance] registerCallHandlerForKey:@"CCServiceManager::setCommonParams" withBlock:^(NSDictionary *parameters, NSMutableDictionary *retParameters) {
            NSDictionary *commonParameters = (NSDictionary *) parameters[@"params"];
            [[ParamsProvider sharedParamsProvider] setParams: commonParameters withKey: @"common"];
        }];
    }

    return self;
}

- (void)registerService:(NSObject *)service {
    [self.services addObject:service];
}

@end
