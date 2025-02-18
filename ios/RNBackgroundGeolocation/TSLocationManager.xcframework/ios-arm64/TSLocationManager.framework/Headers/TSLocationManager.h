#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <CoreData/CoreData.h>
#import <CoreLocation/CoreLocation.h>
#import <AudioToolbox/AudioToolbox.h>
#import "TSSchedule.h"
#import "TSLocation.h"
#import "TSActivityChangeEvent.h"
#import "TSProviderChangeEvent.h"
#import "TSHttpEvent.h"
#import "TSHeartbeatEvent.h"
#import "TSScheduleEvent.h"
#import "TSGeofenceManager.h"
#import "TSGeofencesChangeEvent.h"
#import "TSPowerSaveChangeEvent.h"
#import "TSConnectivityChangeEvent.h"
#import "TSEnabledChangeEvent.h"
#import "TSGeofenceEvent.h"
#import "TSAuthorizationEvent.h"
#import "TSGeofence.h"
#import "LocationManager.h"
#import "TSConfig.h"
#import "TSCurrentPositionRequest.h"
#import "TSWatchPositionRequest.h"
#import "LogQuery.h"
#import "TSDeviceInfo.h"
#import "TSAuthorization.h"
#import "TSHttpService.h"
#import "SOMotionDetector.h"

FOUNDATION_EXPORT double TSLocationManagerVersionNumber;
FOUNDATION_EXPORT const unsigned char TSLocationManagerVersionString[];
FOUNDATION_EXPORT NSString* TSLocationManagerVersion;

/**
 The main API interface.
 */
@interface TSLocationManager : NSObject <CLLocationManagerDelegate>

#pragma mark - Properties

// Flags
@property (atomic, readonly) BOOL enabled;
/// :nodoc:
@property (atomic, readonly) BOOL isConfigured;
/// :nodoc:
@property (atomic, readonly) BOOL isDebuggingMotionDetection;
/// :nodoc:
@property (atomic, readonly) BOOL isUpdatingLocation;
/// :nodoc:
@property (atomic, readonly) BOOL isRequestingLocation;
/// :nodoc:
@property (atomic, readonly) BOOL isMonitoringSignificantLocationChanges;
/// :nodoc:
@property (atomic, readonly) NSDate *suspendedAt;
/// `YES` when the the app was launched in the background.
@property (atomic, readonly) BOOL isLaunchedInBackground;

// LocationManagers

/// The SDK's `CLLocationManager` instance.
@property (nonatomic, strong, readonly) CLLocationManager *locationManager;

@property (atomic) CLLocationDistance distanceFilter;

/// :nodoc:
@property (nonatomic, strong, readonly) LocationManager *currentPositionManager;
/// :nodoc:
@property (nonatomic, strong, readonly) LocationManager *watchPositionManager;
/// :nodoc:
@property (nonatomic, strong, readonly) LocationManager *stateManager;

// Location Resources

/// The location used to monitor the SDK's stationary geofence.
@property (atomic, strong, readonly) CLLocation *stationaryLocation;
/// The last known location.
@property (atomic, strong, readonly) CLLocation *lastLocation;
/// :nodoc:
@property (atomic, strong, readonly) CLLocation *lastGoodLocation;
/// :nodoc:
@property (atomic, strong, readonly) CLLocation *lastOdometerLocation;

// GeofeneManager

/// :nodoc:
@property (nonatomic, strong, readonly) TSGeofenceManager *geofenceManager;

/// The application's `ViewController` instance.  Used for presenting dialogs.
@property (nonatomic) UIViewController* viewController;
/// :nodoc:
@property (atomic) NSDate *stoppedAt;
/// :nodoc:
@property (atomic) UIBackgroundTaskIdentifier preventSuspendTask;
/// :nodoc:
@property (atomic, readonly) BOOL clientReady;
/// :nodoc:
@property (atomic, readonly) BOOL isAcquiringState;
/// :nodoc:
@property (atomic, readonly) BOOL wasAcquiringState;
/// :nodoc:
@property (atomic, readonly) BOOL isAcquiringBackgroundTime;
/// :nodoc:
@property (atomic, readonly) BOOL isAcquiringStationaryLocation;
/// :nodoc:
@property (atomic, readonly) BOOL isAcquiringSpeed;
/// :nodoc:
@property (atomic, readonly) BOOL isHeartbeatEnabled;

// Events listeners
/// :nodoc:
@property (atomic, readonly) NSMutableSet *currentPositionRequests;
/// :nodoc:
@property (atomic, readonly) NSMutableArray *watchPositionRequests;
/// :nodoc:
@property (atomic, readonly) NSMutableSet *locationListeners;
/// :nodoc:
@property (atomic, readonly) NSMutableSet *motionChangeListeners;
/// :nodoc:
@property (atomic, readonly) NSMutableSet *activityChangeListeners;
/// :nodoc:
@property (atomic, readonly) NSMutableSet *providerChangeListeners;
/// :nodoc:
@property (atomic, readonly) NSMutableSet *httpListeners;
/// :nodoc:
@property (atomic, readonly) NSMutableSet *scheduleListeners;
/// :nodoc:
@property (atomic, readonly) NSMutableSet *heartbeatListeners;
/// :nodoc:
@property (atomic, readonly) NSMutableSet *powerSaveChangeListeners;
/// :nodoc:
@property (atomic, readonly) NSMutableSet *enabledChangeListeners;

/// [Optional] User-supplied block to render location-data for SQLite database / Firebase adapter INSERT.
@property (copy) NSDictionary* (^beforeInsertBlock) (TSLocation *location);

/// Callback for requestPermission.
/// :nodoc:
@property (atomic) TSCallback *requestPermissionCallback;

/// Event Queue
/// :nodoc:
@property (atomic, readonly)  NSMutableSet *eventQueue;
/// :nodoc:
@property (atomic) SOMotionType currentMotionType;

/// Returns the API's singleton instance.
+ (TSLocationManager *)sharedInstance;

#pragma mark - Event Listener Methods

/*
 Adds a location event-listener.
 */
- (void) onLocation:(void(^)(TSLocation* location))success failure:(void(^)(NSError*))failure;
- (void) onHttp:(void(^)(TSHttpEvent* event))success;
- (void) onGeofence:(void(^)(TSGeofenceEvent* event))success;
- (void) onHeartbeat:(void(^)(TSHeartbeatEvent* event))success;
- (void) onMotionChange:(void(^)(TSLocation* event))success;
- (void) onActivityChange:(void(^)(TSActivityChangeEvent* event))success;
- (void) onProviderChange:(void(^)(TSProviderChangeEvent* event))success;
- (void) onGeofencesChange:(void(^)(TSGeofencesChangeEvent* event))success;
- (void) onSchedule:(void(^)(TSScheduleEvent* event))success;
- (void) onPowerSaveChange:(void(^)(TSPowerSaveChangeEvent* event))success;
- (void) onConnectivityChange:(void(^)(TSConnectivityChangeEvent* event))succes;
- (void) onEnabledChange:(void(^)(TSEnabledChangeEvent* event))success;
- (void) onAuthorization:(void(^)(TSAuthorizationEvent*))callback;

- (void) removeListener:(NSString*)event callback:(void(^)(id))callback;
- (void) un:(NSString*)event callback:(void(^)(id))callback;
- (void) removeListeners:(NSString*)event;
- (void) removeListenersForEvent:(NSString*)event;
- (void) removeListeners;

#pragma mark - Core API Methods

- (void) configure:(NSDictionary*)params;

/**
 Signal to the plugin that your app is launched and ready, proving the default [Config].

 The supplied [Config] will be applied **only at first install** of your app — for every launch thereafter,
 the plugin will automatically load its last-known configuration from persistent storage.
 The plugin always remembers the configuration you apply to it.


 ```dart
 BackgroundGeolocation.ready(Config(
  desiredAccuracy: Config.DESIRED_ACCURACY_HIGH,
  distanceFilter: 10,
  stopOnTerminate: false,
  startOnBoot: true,
  url: 'http://your.server.com',
  headers: {
    'my-auth-token': 'secret-token'
  }
 )).then((State state) {
  print('[ready] success: ${state}');
 });
 ```

 **WARNING:** The **`#ready`** method only applies the supplied [Config] for the **first launch of the app** &mdash;
 Forever after, the plugin is going to remember **every configuration change** you apply at runtime (eg: [setConfig]) and reload that *same config* every time your app boots.

 ### The `-[TSConfig reset]` method.

 If you wish, you can use the `-[TSConfig reset]` method to reset all `TSConfig` options to documented default-values (with optional overrides):

 ```dart

 BackgroundGeolocation.reset();
 // Reset to documented default-values with overrides
 BackgroundGeolocation.reset(Config(
  distanceFilter:  10
 ));
 ```

 ## [Config.reset]: true

 Optionally, you can set [Config.reset] to `true`  This is helpful during development.  This will essentially *force* the supplied [Config] to be applied with *each launch* of your application.

 ## Example

 ```dart
 BackgroundGeolocation.ready(Config(
  distanceFilter: 50
 )).then((State state) {
  print('[ready] - ${state}')
 });
 ```
 */
- (void) ready;
- (void) start;
- (void) stop;
- (void) startSchedule;
- (void) stopSchedule;
- (void) startGeofences;
- (NSMutableDictionary*) getState;

#pragma mark - Geolocation Methods

- (void) changePace:(BOOL)value;
- (void) getCurrentPosition:(TSCurrentPositionRequest*)request;
- (void) setOdometer:(CLLocationDistance)odometer request:(TSCurrentPositionRequest*)request;
- (CLLocationDistance)getOdometer;
- (void) watchPosition:(TSWatchPositionRequest*)request;
- (void) stopWatchPosition;
- (NSDictionary*) getStationaryLocation;
- (TSProviderChangeEvent*) getProviderState;
- (void) requestPermission:(void(^)(NSNumber *status))success failure:(void(^)(NSNumber *status))failure;
- (void) requestTemporaryFullAccuracy:(NSString*)purpose success:(void(^)(NSInteger))success failure:(void(^)(NSError*))failure;

#pragma mark - HTTP & Persistence Methods

- (void) sync:(void(^)(NSArray* locations))success failure:(void(^)(NSError* error))failure;
- (void) getLocations:(void(^)(NSArray* locations))success failure:(void(^)(NSString* error))failure;
- (BOOL) clearDatabase;
- (BOOL) destroyLocations;
- (void) destroyLocations:(void(^)(void))success failure:(void(^)(NSString* error))failure;
- (void) destroyLocation:(NSString*)uuid;
- (void) destroyLocation:(NSString*)uuid success:(void(^)(void))success failure:(void(^)(NSString* error))failure;
- (void) insertLocation:(NSDictionary*)params success:(void(^)(NSString* uuid))success failure:(void(^)(NSString* error))failure;
- (void) persistLocation:(TSLocation*)location;
- (int) getCount;

#pragma mark - Application Methods

- (UIBackgroundTaskIdentifier) createBackgroundTask;
- (void) stopBackgroundTask:(UIBackgroundTaskIdentifier)taskId;
- (BOOL) isPowerSaveMode;


- (BOOL) destroyLog;
- (void) playSound:(SystemSoundID)soundId;
- (void) error:(UIBackgroundTaskIdentifier)taskId message:(NSString*)message;
#pragma mark - Geofencing Methods

- (void) addGeofence:(TSGeofence*)geofence success:(void (^)(void))success failure:(void (^)(NSString* error))failure;
- (void) addGeofences:(NSArray*)geofences success:(void (^)(void))success failure:(void (^)(NSString* error))failure;
- (void) removeGeofence:(NSString*)identifier success:(void (^)(void))success failure:(void (^)(NSString* error))failure;
- (void) removeGeofences:(NSArray*)identifiers success:(void (^)(void))success failure:(void (^)(NSString* error))failure;;
- (void) removeGeofences;

- (NSArray*) getGeofences;
- (void) getGeofences:(void (^)(NSArray*))success failure:(void (^)(NSString*))failure;
- (void) getGeofence:(NSString*)identifier success:(void (^)(TSGeofence*))success failure:(void (^)(NSString*))failure;
- (void) geofenceExists:(NSString*)identifier callback:(void (^)(BOOL))callback;

#pragma mark - Sensor Methods

-(BOOL) isMotionHardwareAvailable;
-(BOOL) isDeviceMotionAvailable;
-(BOOL) isAccelerometerAvailable;
-(BOOL) isGyroAvailable;
-(BOOL) isMagnetometerAvailable;

#pragma mark - Application life-cycle callbacks

- (void) onSuspend:(NSNotification *)notification;
- (void) onResume:(NSNotification *)notification;
- (void) onAppTerminate;

# pragma mark - Private Methods
/// :nodoc:
- (void) fireMotionActivityChangeEvent:(TSActivityChangeEvent*)event;
@end

