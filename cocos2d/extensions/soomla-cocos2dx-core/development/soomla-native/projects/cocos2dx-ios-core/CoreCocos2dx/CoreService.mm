
#import "CoreService.h"
#import "NdkGlue.h"
#import "Reward.h"
#import "SoomlaEventHandling.h"
#import "CommonConsts.h"
#import "BadgeReward.h"
#import "RandomReward.h"
#import "SequenceReward.h"
#import "DomainHelper.h"
#import "RewardStorage.h"
#import "Schedule.h"
#import "KeyValueStorage.h"
#import "DomainFactory.h"

@interface CoreService ()
@end

@implementation CoreService {

}

+ (id)sharedCoreService {
    static CoreService *sharedCoreService = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedCoreService = [self alloc];
    });
    return sharedCoreService;
}

+ (void)initialize {
    [super initialize];
    [self initGlue];
    [self initCreators];
}

- (id)init {
    self = [super init];
    if (self) {
        [SoomlaEventHandling observeAllEventsWithObserver:[NdkGlue sharedInstance]
                                                  withSelector:@selector(dispatchNdkCallback:)];
    }

    return self;
}

+ (void)initGlue {
    NdkGlue *ndkGlue = [NdkGlue sharedInstance];

    /* -= Call handlers =- */
    [ndkGlue registerCallHandlerForKey:@"CCCoreService::init" withBlock:^(NSDictionary *parameters, NSMutableDictionary *retParameters) {
        [[CoreService sharedCoreService] init];
    }];
    [ndkGlue registerCallHandlerForKey:@"CCCoreService::getTimesGiven" withBlock:^(NSDictionary *parameters, NSMutableDictionary *retParameters) {
        NSDictionary *rewardDict = parameters[@"reward"];
        Reward *reward = rewardDict ? [[DomainFactory sharedDomainFactory] createWithDict:rewardDict] : nil;
        int res = [RewardStorage getTimesGivenForReward:reward];
        retParameters[@"return"] = @(res);
    }];
    [ndkGlue registerCallHandlerForKey:@"CCCoreService::setRewardStatus" withBlock:^(NSDictionary *parameters, NSMutableDictionary *retParameters) {
        NSDictionary *rewardDict = parameters[@"reward"];
        bool give = [parameters[@"give"] boolValue];
        bool notify = [parameters[@"notify"] boolValue];
        Reward *reward = rewardDict ? [[DomainFactory sharedDomainFactory] createWithDict:rewardDict] : nil;
        [RewardStorage setStatus:give forReward:reward andNotify:notify];
    }];
    [ndkGlue registerCallHandlerForKey:@"CCCoreService::getLastSeqIdxGiven" withBlock:^(NSDictionary *parameters, NSMutableDictionary *retParameters) {
        NSDictionary *rewardDict = parameters[@"reward"];
        Reward *reward = rewardDict ? [[DomainFactory sharedDomainFactory] createWithDict:rewardDict] : nil;
        int res = [RewardStorage getLastSeqIdxGivenForReward:(SequenceReward *) reward];
        retParameters[@"return"] = @(res);
    }];
    [ndkGlue registerCallHandlerForKey:@"CCCoreService::setLastSeqIdxGiven" withBlock:^(NSDictionary *parameters, NSMutableDictionary *retParameters) {
        NSDictionary *rewardDict = parameters[@"reward"];
        int idx = [parameters[@"idx"] intValue];
        Reward *reward = rewardDict ? [[DomainFactory sharedDomainFactory] createWithDict:rewardDict] : nil;
        [RewardStorage setLastSeqIdxGiven:idx ForReward:(SequenceReward *) reward];
    }];

    [ndkGlue registerCallHandlerForKey:@"CCCoreService::getValue" withBlock:^(NSDictionary *parameters, NSMutableDictionary *retParameters) {
        NSString *key = parameters[@"key"];
        NSString *res = [KeyValueStorage getValueForKey:key];
        retParameters[@"return"] = res;
    }];
    [ndkGlue registerCallHandlerForKey:@"CCCoreService::setValue" withBlock:^(NSDictionary *parameters, NSMutableDictionary *retParameters) {
        NSString *key = parameters[@"key"];
        NSString *val = parameters[@"val"];
        [KeyValueStorage setValue:val forKey:key];
    }];
    [ndkGlue registerCallHandlerForKey:@"CCCoreService::deleteKeyValue" withBlock:^(NSDictionary *parameters, NSMutableDictionary *retParameters) {
        NSString *key = parameters[@"key"];
        [KeyValueStorage deleteValueForKey:key];
    }];
    [ndkGlue registerCallHandlerForKey:@"CCCoreService::purge" withBlock:^(NSDictionary *parameters, NSMutableDictionary *retParameters) {
        [KeyValueStorage purge];
    }];

    /* -= Callback handlers =- */
    [ndkGlue registerCallbackHandlerForKey:EVENT_REWARD_GIVEN withBlock:^(NSNotification *notification, NSMutableDictionary *parameters) {
        parameters[@"method"] = @"com.soomla.events.RewardGivenEvent";
        Reward *reward = (notification.userInfo)[DICT_ELEMENT_REWARD];
        parameters[@"reward"] = [[DomainHelper sharedDomainHelper] domainToDict:reward];
    }];
    [ndkGlue registerCallbackHandlerForKey:EVENT_REWARD_TAKEN withBlock:^(NSNotification *notification, NSMutableDictionary *parameters) {
        parameters[@"method"] = @"com.soomla.events.RewardTakenEvent";
        Reward *reward = (notification.userInfo)[DICT_ELEMENT_REWARD];
        parameters[@"reward"] = [[DomainHelper sharedDomainHelper] domainToDict:reward];
    }];

}

+ (void)initCreators {
    [[DomainHelper sharedDomainHelper] registerType:(NSString *)JSON_JSON_TYPE_BADGE
                                      withClassName:NSStringFromClass([BadgeReward class])
                                           andBlock:^id(NSDictionary *dict) {
                                               return [[BadgeReward alloc] initWithDictionary:dict];
                                           }];
    [[DomainHelper sharedDomainHelper] registerType:(NSString *)JSON_JSON_TYPE_RANDOM
                                      withClassName:NSStringFromClass([RandomReward class])
                                           andBlock:^id(NSDictionary *dict) {
                                               return [[RandomReward alloc] initWithDictionary:dict];
                                           }];
    [[DomainHelper sharedDomainHelper] registerType:(NSString *)JSON_JSON_TYPE_SEQUENCE
                                      withClassName:NSStringFromClass([SequenceReward class])
                                           andBlock:^id(NSDictionary *dict) {
                                               return [[SequenceReward alloc] initWithDictionary:dict];
                                           }];
    [[DomainHelper sharedDomainHelper] registerType:(NSString *)JSON_JSON_TYPE_SCHEDULE
                                      withClassName:NSStringFromClass([Schedule class])
                                           andBlock:^id(NSDictionary *dict) {
                                               return [[Schedule alloc] initWithDictionary:dict];
                                           }];
    [[DomainHelper sharedDomainHelper] registerType:(NSString *) JSON_JSON_TYPE_DATE_TIME_RANGE
                                      withClassName:NSStringFromClass([DateTimeRange class])];
}

@end
